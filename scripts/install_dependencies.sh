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

# 2. Automatizar a resolução do terminal nas configurações do GRUB
echo "Configurando os parâmetros de vídeo do terminal..."

# Remove linhas antigas de GFXMODE e GFXPAYLOAD se existirem (para evitar duplicações)
sed -i '/^GRUB_GFXMODE=/d' /etc/default/grub
sed -i '/^GRUB_GFXPAYLOAD_LINUX=/d' /etc/default/grub

# Adiciona as configurações de tamanho para o GRUB inicial
echo "GRUB_GFXMODE=1024x768" >> /etc/default/grub
echo "GRUB_GFXPAYLOAD_LINUX=keep" >> /etc/default/grub

# 3. MÁGICA DO SCRIPT: Altera a linha do Kernel automaticamente!
# Esse comando procura por GRUB_CMDLINE_LINUX_DEFAULT e injeta o video=1024x768 dentro das aspas,
# não importando o que já esteja escrito lá (como "quiet" ou "splash").
sed -i 's/^GRUB_CMDLINE_LINUX_DEFAULT="\(.*\)"/GRUB_CMDLINE_LINUX_DEFAULT="\1 video=1024x768"/' /etc/default/grub

# 4. Atualiza o GRUB do Debian para aplicar tudo
update-grub

echo "------------------------------------------------------------"
echo " CONFIGURAÇÃO CONCLUÍDA COM SUCESSO!"
echo " Para que o terminal mude de tamanho, a VM precisa reiniciar."
echo " Digite 'reboot' agora para aplicar."
echo "------------------------------------------------------------"