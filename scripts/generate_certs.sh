#!/bin/bash

# Script para gerar certificados SSL/TLS auto-assinados para o sistema de votação

echo "Gerando certificados SSL/TLS para o sistema de votação..."

# Criar diretório de certificados
mkdir -p server/certs
mkdir -p client/certs

# Gerar chave privada da CA
openssl genrsa -out server/certs/ca.key 4096

# Gerar certificado da CA
openssl req -new -x509 -days 365 -key server/certs/ca.key -out server/certs/ca.crt -subj "/C=BR/ST=SP/L=Sao Paulo/O=SistemaVotacao/OU=CA/CN=SistemaVotacaoCA"

# Gerar chave privada do servidor
openssl genrsa -out server/certs/server.key 2048

# Gerar CSR do servidor
openssl req -new -key server/certs/server.key -out server/certs/server.csr -subj "/C=BR/ST=SP/L=Sao Paulo/O=SistemaVotacao/OU=Server/CN=localhost"

# Assinar certificado do servidor com a CA
openssl x509 -req -days 365 -in server/certs/server.csr -CA server/certs/ca.crt -CAkey server/certs/ca.key -CAcreateserial -out server/certs/server.crt

# Copiar certificado CA para o cliente
cp server/certs/ca.crt client/certs/ca.crt

# Limpar arquivos temporários
rm server/certs/server.csr
rm server/certs/ca.srl 2>/dev/null

# Definir permissões
chmod 600 server/certs/server.key
chmod 600 server/certs/ca.key
chmod 644 server/certs/server.crt
chmod 644 server/certs/ca.crt
chmod 644 client/certs/ca.crt

echo "Certificados gerados com sucesso!"
echo "Servidor: server/certs/server.crt e server/certs/server.key"
echo "Cliente: client/certs/ca.crt"
