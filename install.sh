#!/bin/bash

# Couleurs pour les messages
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

# Fonction pour afficher les messages
print_message() {
    echo -e "${GREEN}[INFO]${NC} $1"
}

print_error() {
    echo -e "${RED}[ERROR]${NC} $1"
}

print_warning() {
    echo -e "${YELLOW}[WARNING]${NC} $1"
}

# Vérification des prérequis
check_prerequisites() {
    print_message "Vérification des prérequis..."
    
    # Vérifier si on est root
    if [ "$EUID" -eq 0 ]; then
        print_error "Ne pas exécuter ce script en tant que root"
        exit 1
    fi

    # Vérifier les commandes nécessaires
    commands=("git" "make" "gcc" "python3" "pip3")
    for cmd in "${commands[@]}"; do
        if ! command -v $cmd &> /dev/null; then
            print_error "$cmd n'est pas installé"
            exit 1
        fi
    done
}

# Installation des dépendances système
install_system_dependencies() {
    print_message "Installation des dépendances système..."
    sudo apt-get update
    sudo apt-get install -y build-essential git wget curl python3 python3-pip
}

# Installation de Contiki-NG
install_contiki_ng() {
    print_message "Installation de Contiki-NG..."
    
    # Vérifier si Contiki-NG existe déjà
    if [ ! -d "$HOME/contiki-ng" ]; then
        cd $HOME
        git clone https://github.com/contiki-ng/contiki-ng.git
        cd contiki-ng
        git submodule update --init --recursive
    else
        print_warning "Contiki-NG est déjà installé"
    fi
}

# Installation des outils MSP430
install_msp430_tools() {
    print_message "Installation des outils MSP430..."
    cd $HOME/contiki-ng/tools
    sudo ./install-msp430.sh
}

# Configuration du projet
setup_project() {
    print_message "Configuration du projet..."
    
    # Vérifier si le projet existe déjà
    if [ -d "$HOME/contiki-ng/examples/RPLMQoS_BELACEL" ]; then
        print_warning "Le projet existe déjà. Suppression..."
        rm -rf $HOME/contiki-ng/examples/RPLMQoS_BELACEL
    fi
    
    # Copier le projet
    cp -r $(dirname "$0") $HOME/contiki-ng/examples/
    
    # Définir les permissions
    sudo chown -R $USER:$USER $HOME/contiki-ng/examples/RPLMQoS_BELACEL
}

# Compilation du projet
compile_project() {
    print_message "Compilation du projet..."
    cd $HOME/contiki-ng/examples/RPLMQoS_BELACEL
    make TARGET=sky > compilation_log.txt 2>&1
    
    if [ $? -eq 0 ]; then
        print_message "Compilation réussie !"
    else
        print_error "Erreur lors de la compilation. Vérifiez compilation_log.txt"
    fi
}

# Fonction principale
main() {
    print_message "Début de l'installation..."
    
    check_prerequisites
    install_system_dependencies
    install_contiki_ng
    install_msp430_tools
    setup_project
    compile_project
    
    print_message "Installation terminée !"
    print_message "Le projet est disponible dans : $HOME/contiki-ng/examples/RPLMQoS_BELACEL"
}

# Exécution du script
main 