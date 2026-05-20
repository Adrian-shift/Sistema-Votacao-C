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

# 2. Configurar o tamanho da tela no GRUB
echo "Configurando os parâmetros de vídeo do terminal..."

# Limpa configurações antigas do script para evitar duplicados
sed -i '/^GRUB_GFXMODE=/d' /etc/default/grub
sed -i '/^GRUB_GFXPAYLOAD_LINUX=/d' /etc/default/grub

echo "GRUB_GFXMODE=1024x768" >> /etc/default/grub
echo "GRUB_GFXPAYLOAD_LINUX=keep" >> /etc/default/grub

# 3. MÁGICA: Forçar o Kernel a NÃO resetar a tela usando 'nomodeset'
# Primeiro limpamos qualquer 'video=' ou 'nomodeset' anterior para o script ser reutilizável
sed -i 's/ video=1024x768//g' /etc/default/grub
sed -i 's/ nomodeset//g' /etc/default/grub

# Injeta o 'nomodeset' dentro das aspas do GRUB_CMDLINE_LINUX_DEFAULT
sed -i 's/^GRUB_CMDLINE_LINUX_DEFAULT="\(.*\)"/GRUB_CMDLINE_LINUX_DEFAULT="\1 nomodeset"/' /etc/default/grub

# 4. Atualizar o sistema de boot do Debian
update-grub

echo "------------------------------------------------------------"
echo " CONFIGURAÇÃO ATUALIZADA COM NOMODESET!"
echo " Agora sim, a resolução vai ficar travada mesmo após o login."
echo " Digite 'reboot' para aplicar e testar."
echo "------------------------------------------------------------"