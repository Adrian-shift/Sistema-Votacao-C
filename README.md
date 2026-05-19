# Sistema de Votação Distribuída

Projeto acadêmico desenvolvido em C no Debian Linux.

## Tecnologias

- C
- TCP/IP
- pthread
- Newt
- SQLite3

## Instalação

### Clonar

```bash
git clone https://github.com/SEU_USUARIO/votacao-distribuida.git
```

### Instalar dependências

```bash
cd votacao-distribuida
./scripts/install_dependencies.sh
```

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
make run
```

Cliente:

```bash
make run
```

Use:

- IP: 127.0.0.1
- Porta: 8080
- Eleitor: 101

---

# Funcionalidades

- Dashboard Newt
- TCP/IP
- Multithreading
- SQLite
- ACK/NACK
- Recibos
- Controle duplicidade
