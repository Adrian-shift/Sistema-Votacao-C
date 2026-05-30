#!/bin/bash

set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
ROOT_DIR="$(cd "$SCRIPT_DIR/.." && pwd)"

SERVER_CERT_DIR="$ROOT_DIR/server/certs"
CLIENT_CERT_DIR="$ROOT_DIR/client/certs"
SERVER_DNS="${SERVER_DNS:-}"
SERVER_IP="${SERVER_IP:-}"

echo "Gerando certificados SSL/TLS para o sistema de votacao..."

mkdir -p "$SERVER_CERT_DIR"
mkdir -p "$CLIENT_CERT_DIR"

cat > "$SERVER_CERT_DIR/ca.cnf" <<'EOF'
[req]
distinguished_name = dn
x509_extensions = v3_ca
prompt = no

[dn]
C = BR
ST = SP
L = Sao Paulo
O = SistemaVotacao
OU = CA
CN = SistemaVotacaoCA

[v3_ca]
basicConstraints = critical,CA:TRUE
keyUsage = critical,keyCertSign,cRLSign
subjectKeyIdentifier = hash
authorityKeyIdentifier = keyid:always,issuer
EOF

cat > "$SERVER_CERT_DIR/server.cnf" <<'EOF'
[req]
distinguished_name = dn
req_extensions = v3_req
prompt = no

[dn]
C = BR
ST = SP
L = Sao Paulo
O = SistemaVotacao
OU = Server
CN = localhost

[v3_req]
basicConstraints = critical,CA:FALSE
keyUsage = critical,digitalSignature,keyEncipherment
extendedKeyUsage = serverAuth
subjectAltName = @alt_names

[alt_names]
DNS.1 = localhost
IP.1 = 127.0.0.1
EOF

if [[ -n "$SERVER_DNS" && "$SERVER_DNS" != "localhost" ]]; then
    echo "DNS.2 = $SERVER_DNS" >> "$SERVER_CERT_DIR/server.cnf"
fi

if [[ -n "$SERVER_IP" && "$SERVER_IP" != "127.0.0.1" ]]; then
    echo "IP.2 = $SERVER_IP" >> "$SERVER_CERT_DIR/server.cnf"
fi

openssl genrsa -out "$SERVER_CERT_DIR/ca.key" 4096

openssl req \
    -new \
    -x509 \
    -sha256 \
    -days 365 \
    -key "$SERVER_CERT_DIR/ca.key" \
    -out "$SERVER_CERT_DIR/ca.crt" \
    -config "$SERVER_CERT_DIR/ca.cnf"

openssl genrsa -out "$SERVER_CERT_DIR/server.key" 2048

openssl req \
    -new \
    -key "$SERVER_CERT_DIR/server.key" \
    -out "$SERVER_CERT_DIR/server.csr" \
    -config "$SERVER_CERT_DIR/server.cnf"

openssl x509 \
    -req \
    -sha256 \
    -days 365 \
    -in "$SERVER_CERT_DIR/server.csr" \
    -CA "$SERVER_CERT_DIR/ca.crt" \
    -CAkey "$SERVER_CERT_DIR/ca.key" \
    -CAcreateserial \
    -out "$SERVER_CERT_DIR/server.crt" \
    -extfile "$SERVER_CERT_DIR/server.cnf" \
    -extensions v3_req

cp "$SERVER_CERT_DIR/ca.crt" "$CLIENT_CERT_DIR/ca.crt"

rm -f \
    "$SERVER_CERT_DIR/server.csr" \
    "$SERVER_CERT_DIR/ca.srl" \
    "$SERVER_CERT_DIR/ca.cnf" \
    "$SERVER_CERT_DIR/server.cnf"

chmod 600 "$SERVER_CERT_DIR/server.key"
chmod 600 "$SERVER_CERT_DIR/ca.key"
chmod 644 "$SERVER_CERT_DIR/server.crt"
chmod 644 "$SERVER_CERT_DIR/ca.crt"
chmod 644 "$CLIENT_CERT_DIR/ca.crt"

echo "Certificados gerados com sucesso!"
echo "Servidor: server/certs/server.crt e server/certs/server.key"
echo "Cliente: client/certs/ca.crt"
