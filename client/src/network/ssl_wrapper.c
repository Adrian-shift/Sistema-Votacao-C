#include "ssl_wrapper.h"

#include <limits.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#ifndef PATH_MAX
#define PATH_MAX 4096
#endif

static void set_error_message(
    char* error_buffer,
    size_t error_buffer_size,
    const char* message
)
{
    if(!error_buffer || error_buffer_size == 0)
    {
        return;
    }

    snprintf(
        error_buffer,
        error_buffer_size,
        "%s",
        message ? message : "erro desconhecido"
    );
}

static void capture_openssl_error(
    char* error_buffer,
    size_t error_buffer_size,
    const char* fallback_message
)
{
    unsigned long error_code;

    if(!error_buffer || error_buffer_size == 0)
    {
        return;
    }

    error_code = ERR_get_error();
    if(error_code == 0)
    {
        set_error_message(
            error_buffer,
            error_buffer_size,
            fallback_message
        );
        return;
    }

    ERR_error_string_n(
        error_code,
        error_buffer,
        error_buffer_size
    );
}

static void capture_ssl_handshake_error(
    SSL* ssl,
    int result,
    char* error_buffer,
    size_t error_buffer_size,
    const char* fallback_message
)
{
    unsigned long error_code;
    int ssl_error;

    if(!error_buffer || error_buffer_size == 0)
    {
        return;
    }

    ssl_error = SSL_get_error(ssl, result);
    error_code = ERR_get_error();

    if(error_code != 0)
    {
        ERR_error_string_n(
            error_code,
            error_buffer,
            error_buffer_size
        );
        return;
    }

    {
        long verify_result = SSL_get_verify_result(ssl);

        if(verify_result != X509_V_OK)
        {
            snprintf(
                error_buffer,
                error_buffer_size,
                "%s: %s",
                fallback_message,
                X509_verify_cert_error_string(verify_result)
            );
            return;
        }
    }

    if(ssl_error == SSL_ERROR_SYSCALL && errno != 0)
    {
        snprintf(
            error_buffer,
            error_buffer_size,
            "%s: %s",
            fallback_message,
            strerror(errno)
        );
        return;
    }

    snprintf(
        error_buffer,
        error_buffer_size,
        "%s (SSL_get_error=%d)",
        fallback_message,
        ssl_error
    );
}

static int resolve_runtime_path(
    const char* relative_path,
    char* resolved_path,
    size_t resolved_path_size
)
{
    char exe_path[PATH_MAX];
    char candidate[PATH_MAX];
    ssize_t len;
    char* slash;

    if(!relative_path || !resolved_path || resolved_path_size == 0)
    {
        return 0;
    }

    if(access(relative_path, R_OK) == 0)
    {
        snprintf(resolved_path, resolved_path_size, "%s", relative_path);
        return 1;
    }

    len = readlink("/proc/self/exe", exe_path, sizeof(exe_path) - 1);
    if(len < 0)
    {
        return 0;
    }

    exe_path[len] = '\0';

    slash = strrchr(exe_path, '/');
    if(!slash)
    {
        return 0;
    }

    *slash = '\0';

    if(snprintf(
        candidate,
        sizeof(candidate),
        "%s/../%s",
        exe_path,
        relative_path
    ) >= (int)sizeof(candidate))
    {
        return 0;
    }

    if(access(candidate, R_OK) != 0)
    {
        return 0;
    }

    snprintf(resolved_path, resolved_path_size, "%s", candidate);
    return 1;
}

SSL_CTX* init_client_ssl(
    const char* ca_cert_file,
    char* error_buffer,
    size_t error_buffer_size
)
{
    SSL_CTX* ctx;
    char resolved_ca_cert_file[PATH_MAX];

    if(error_buffer && error_buffer_size > 0)
    {
        error_buffer[0] = '\0';
    }

    SSL_library_init();
    SSL_load_error_strings();
    OpenSSL_add_all_algorithms();

    const SSL_METHOD* method = TLS_client_method();

    ctx = SSL_CTX_new(method);
    if(!ctx)
    {
        capture_openssl_error(
            error_buffer,
            error_buffer_size,
            "Falha ao criar SSL_CTX"
        );
        return NULL;
    }

    SSL_CTX_set_min_proto_version(ctx, TLS1_2_VERSION);
    SSL_CTX_set_max_proto_version(ctx, TLS1_3_VERSION);

    if(ca_cert_file)
    {
        if(!resolve_runtime_path(
            ca_cert_file,
            resolved_ca_cert_file,
            sizeof(resolved_ca_cert_file)
        ))
        {
            set_error_message(
                error_buffer,
                error_buffer_size,
                "Certificado da CA nao encontrado"
            );
            SSL_CTX_free(ctx);
            return NULL;
        }

        if(SSL_CTX_load_verify_locations(
            ctx,
            resolved_ca_cert_file,
            NULL
        ) <= 0)
        {
            capture_openssl_error(
                error_buffer,
                error_buffer_size,
                "Falha ao carregar certificado da CA"
            );
            SSL_CTX_free(ctx);
            return NULL;
        }

        SSL_CTX_set_verify(ctx, SSL_VERIFY_PEER, NULL);
    }
    else
    {
        SSL_CTX_set_verify(ctx, SSL_VERIFY_NONE, NULL);
    }

    return ctx;
}

SSL* ssl_connect(
    SSL_CTX* ctx,
    int socket_fd,
    char* error_buffer,
    size_t error_buffer_size
)
{
    SSL* ssl = SSL_new(ctx);
    int result;

    if(error_buffer && error_buffer_size > 0)
    {
        error_buffer[0] = '\0';
    }

    if(!ssl)
    {
        capture_openssl_error(
            error_buffer,
            error_buffer_size,
            "Falha ao criar SSL"
        );
        return NULL;
    }

    SSL_set_fd(ssl, socket_fd);

    result = SSL_connect(ssl);

    if(result <= 0)
    {
        capture_ssl_handshake_error(
            ssl,
            result,
            error_buffer,
            error_buffer_size,
            "Falha no handshake SSL"
        );
        SSL_free(ssl);
        return NULL;
    }

    return ssl;
}

void ssl_cleanup(SSL* ssl, SSL_CTX* ctx)
{
    if(ssl)
    {
        SSL_shutdown(ssl);
        SSL_free(ssl);
    }

    if(ctx)
    {
        SSL_CTX_free(ctx);
    }
}

int ssl_send(SSL* ssl, const char* data, int len)
{
    return SSL_write(ssl, data, len);
}

int ssl_recv(SSL* ssl, char* buffer, int len)
{
    return SSL_read(ssl, buffer, len);
}
