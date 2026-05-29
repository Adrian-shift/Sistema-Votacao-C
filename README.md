# Sistema de Votação Distribuída

Projeto acadêmico desenvolvido em C no Debian Linux.

## Tecnologias

- C
- TCP/IP
- pthread
- Newt
- SQLite3
- OpenSSL (SSL/TLS 1.2/1.3)

## Instalação

### Clonar

```bash
git clone https://github.com/Adrian-shift/Sistema-Votacao-C.git
```

### Instalar dependências

```bash
cd Sistema-Votacao-C

chmod +x scripts/install_dependencies.sh # Permissão para o script
./scripts/install_dependencies.sh
```

### Gerar certificados SSL/TLS

```bash
chmod +x scripts/generate_certs.sh
./scripts/generate_certs.sh # OpenSSL para criptografia
```

Isso cria:
- `server/certs/server.crt` e `server/certs/server.key` (certificado do servidor)
- `client/certs/ca.crt` (certificado da CA para verificação)

---

# Servidor

## Compilar

```bash
cd server
make
```

## Executar

```bash
make run
```

---

# Cliente

## Compilar

```bash
cd client
make
```

## Executar

```bash
make run
```

---

# Teste rápido

Servidor:

```bash
cd server
make run
```

Cliente:

```bash
cd client
make run
```

Use:

- IP: (da maquina servidor)
- Porta: 8080
- Eleitor: 101

---

# Funcionalidades

- Dashboard Newt
- TCP/IP com SSL/TLS 1.2/1.3
- Multithreading
- SQLite
- ACK/NACK
- Recibos
- Controle duplicidade
- Criptografia de ponta a ponta (OpenSSL)

---

# Criptografia

O sistema usa OpenSSL para criptografia de ponta a ponta:

- **TLS 1.2/1.3**: Protocolos modernos e seguros
- **Verificação de certificados**: Cliente valida o certificado do servidor
- **Auto-assinado**: Certificados auto-assinados para ambiente de teste
- **Tráfego criptografado**: Todos os votos são enviados via canal seguro
