#include "ssl_wrapper.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

SSL_CTX* init_client_ssl(const char* ca_cert_file)
{
    SSL_CTX* ctx;
    
    SSL_library_init();
    SSL_load_error_strings();
    OpenSSL_add_all_algorithms();
    
    const SSL_METHOD* method = TLS_client_method();
    
    ctx = SSL_CTX_new(method);
    if(!ctx)
    {
        fprintf(stderr, "[ERRO] Falha ao criar SSL_CTX\n");
        ERR_print_errors_fp(stderr);
        return NULL;
    }
    
    SSL_CTX_set_min_proto_version(ctx, TLS1_2_VERSION);
    SSL_CTX_set_max_proto_version(ctx, TLS1_3_VERSION);
    
    if(ca_cert_file)
    {
        if(SSL_CTX_load_verify_locations(ctx, ca_cert_file, NULL) <= 0)
        {
            fprintf(stderr, "[ERRO] Falha ao carregar certificado CA: %s\n", ca_cert_file);
            ERR_print_errors_fp(stderr);
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

SSL* ssl_connect(SSL_CTX* ctx, int socket_fd)
{
    SSL* ssl = SSL_new(ctx);
    if(!ssl)
    {
        fprintf(stderr, "[ERRO] Falha ao criar SSL\n");
        ERR_print_errors_fp(stderr);
        return NULL;
    }
    
    SSL_set_fd(ssl, socket_fd);
    
    if(SSL_connect(ssl) <= 0)
    {
        fprintf(stderr, "[ERRO] Falha no handshake SSL\n");
        ERR_print_errors_fp(stderr);
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
