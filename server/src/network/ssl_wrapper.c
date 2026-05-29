#include "ssl_wrapper.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

SSL_CTX* init_server_ssl(const char* cert_file, const char* key_file)
{
    SSL_CTX* ctx;
    
    SSL_library_init();
    SSL_load_error_strings();
    OpenSSL_add_all_algorithms();
    
    const SSL_METHOD* method = TLS_server_method();
    
    ctx = SSL_CTX_new(method);
    if(!ctx)
    {
        fprintf(stderr, "[ERRO] Falha ao criar SSL_CTX\n");
        ERR_print_errors_fp(stderr);
        return NULL;
    }
    
    SSL_CTX_set_min_proto_version(ctx, TLS1_2_VERSION);
    SSL_CTX_set_max_proto_version(ctx, TLS1_3_VERSION);
    
    if(SSL_CTX_use_certificate_file(ctx, cert_file, SSL_FILETYPE_PEM) <= 0)
    {
        fprintf(stderr, "[ERRO] Falha ao carregar certificado: %s\n", cert_file);
        ERR_print_errors_fp(stderr);
        SSL_CTX_free(ctx);
        return NULL;
    }
    
    if(SSL_CTX_use_PrivateKey_file(ctx, key_file, SSL_FILETYPE_PEM) <= 0)
    {
        fprintf(stderr, "[ERRO] Falha ao carregar chave privada: %s\n", key_file);
        ERR_print_errors_fp(stderr);
        SSL_CTX_free(ctx);
        return NULL;
    }
    
    if(!SSL_CTX_check_private_key(ctx))
    {
        fprintf(stderr, "[ERRO] Chave privada não corresponde ao certificado\n");
        SSL_CTX_free(ctx);
        return NULL;
    }
    
    return ctx;
}

SSL* ssl_accept(SSL_CTX* ctx, int client_socket)
{
    SSL* ssl = SSL_new(ctx);
    if(!ssl)
    {
        fprintf(stderr, "[ERRO] Falha ao criar SSL\n");
        ERR_print_errors_fp(stderr);
        return NULL;
    }
    
    SSL_set_fd(ssl, client_socket);
    
    if(SSL_accept(ssl) <= 0)
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
