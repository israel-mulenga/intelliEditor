#!/bin/bash

# Détection de la distribution
if [ -f /etc/debian_version ]; then
    echo "Distribution basée sur Debian/Ubuntu détectée..."
    sudo apt update
    sudo apt install -y build-essential cmake pkg-config \
                        libgtk-3-dev libgtksourceview-4-dev \
                        libhunspell-dev libcjson-dev libpcre2-dev libcurl4-openssl-dev
elif [ -f /etc/fedora-release ]; then
    echo "Distribution basée sur Fedora détectée..."
    sudo dnf install -y gcc gcc-c++ cmake make pkg-config \
                        gtk3-devel gtksourceview4-devel \
                        hunspell-devel cJSON-devel pcre2-devel libcurl-devel
else
    echo "Distribution non supportée automatiquement. Installez manuellement : gtk3, hunspell, cjson, pcre2, libcurl."
fi

echo "Installation des dépendances terminée !"