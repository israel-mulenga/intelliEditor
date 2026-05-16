#!/bin/bash

set -e

pkg_list_debian=(build-essential cmake pkg-config libgtk-3-dev libgtksourceview-4-dev libhunspell-dev libcjson-dev libpcre2-dev libcurl4-openssl-dev)
pkg_list_fedora=(gcc gcc-c++ cmake make pkg-config gtk3-devel gtksourceview4-devel hunspell-devel cjson-devel pcre2-devel libcurl-devel)

install_debian() {
    echo "Distribution basée sur Debian/Ubuntu détectée..."
    sudo apt update
    sudo apt install -y "${pkg_list_debian[@]}"
}

install_fedora() {
    echo "Distribution basée sur Fedora détectée..."
    sudo dnf install -y "${pkg_list_fedora[@]}"
}

echo "Détection de la distribution..."

if [ -f /etc/debian_version ]; then
    install_debian
elif [ -f /etc/fedora-release ]; then
    install_fedora
elif [ -f /etc/arch-release ] || grep -qi arch /etc/os-release 2>/dev/null; then
    echo "Distribution basée sur Arch détectée..."
    sudo pacman -Syu --noconfirm base-devel cmake pkgconf gtk3 gtksourceview4 hunspell cjson pcre curl
elif grep -qi "^ID=alpine" /etc/os-release 2>/dev/null; then
    echo "Distribution basée sur Alpine détectée..."
    sudo apk add build-base cmake pkgconfig gtk+3.0 gtksourceview4 hunspell cjson pcre libcurl
else
    echo "Distribution non supportée automatiquement."
    if [ -n "$FLATPAK_RUNTIME" ] || grep -q 'org.freedesktop.platform' /etc/os-release 2>/dev/null; then
        echo "Vous êtes dans un runtime Flatpak. Le script ne peut pas installer de paquets ici."
        echo "Installez les dépendances sur l'hôte ou utilisez un SDK approprié."
    fi
    echo "Packages nécessaires : gtk3, libgtksourceview-4, hunspell, cjson, pcre2, libcurl, cmake, pkg-config."
    echo "Exemple Debian/Ubuntu : sudo apt install ${pkg_list_debian[*]}"
    echo "Exemple Fedora : sudo dnf install ${pkg_list_fedora[*]}"
    exit 1
fi

echo "Installation des dépendances terminée !"
