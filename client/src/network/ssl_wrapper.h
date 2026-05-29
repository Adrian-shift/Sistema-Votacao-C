#ifndef SSL_WRAPPER_H
#define SSL_WRAPPER_H

#include <openssl/ssl.h>
#include <openssl/err.h>

SSL_CTX* init_client_ssl(const char* ca_cert_file);
SSL* ssl_connect(SSL_CTX* ctx, int socket_fd);
void ssl_cleanup(SSL* ssl, SSL_CTX* ctx);
int ssl_send(SSL* ssl, const char* data, int len);
int ssl_recv(SSL* ssl, char* buffer, int len);

#endif
