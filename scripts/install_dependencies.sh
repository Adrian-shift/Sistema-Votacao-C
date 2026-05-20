#!/bin/bash

# 1. Atualizar repositórios e instalar dependências
apt update
apt install -y \
	build-essential \
	gcc \
	make \
	libnewt-dev \
	libsqlite3-dev \
	sqlite3 \
	netcat-openbsd

# 2. Automatizar a resolução do terminal para caber a interface 90x28
echo "Configurando a resolução do terminal..."

# Limpa linhas antigas para evitar duplicar se o script rodar mais de uma vez
sed -i '/^GRUB_GFXMODE=/d' /etc/default/grub
sed -i '/^GRUB_GFXPAYLOAD_LINUX=/d' /etc/default/grub

# Injeta as configurações direto no arquivo do GRUB
echo "GRUB_GFXMODE=1024x768" >> /etc/default/grub
echo "GRUB_GFXPAYLOAD_LINUX=keep" >> /etc/default/grub

# Atualiza o GRUB para aplicar as mudanças no próximo boot
update-grub

echo "--------------------------------------------------------"
echo "Pronto! Dependências instaladas e tela configurada."
echo "IMPORTANTE: Execute o comando 'reboot' para aplicar a nova resolução."
echo "--------------------------------------------------------"