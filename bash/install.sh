#!/bin/bash

# Ensure script is run with sudo to have the necessary permissions
if [[ $EUID -ne 0 ]]; then
    echo "This script must be run as root" 
    exit 1
fi

# Install Anaconda
echo "Downloading Anaconda..."
curl -O https://repo.anaconda.com/archive/Anaconda3-2023.01-MacOSX-x86_64.sh
echo "Installing Anaconda..."
bash Anaconda3-2023.01-MacOSX-x86_64.sh -b -p $HOME/anaconda3

# Install Visual Studio Code
echo "Installing Visual Studio Code..."
brew install --cask visual-studio-code

# Create code directory and clone the repository
echo "Setting up project directory and cloning repository..."
mkdir -p /Users/$(whoami)/code
cd /Users/$(whoami)/code
git clone https://github.com/petrasvestartas/compas_wood.git
cd compas_wood

# Setup Conda environment
echo "Creating and activating conda environment..."
$HOME/anaconda3/bin/conda create -n compas_wood_3_9_10 python=3.9.10 compas -y
source $HOME/anaconda3/bin/activate compas_wood_3_9_10

# Install Python dependencies
echo "Installing Python dependencies..."
pip install -r requirements.txt
pip install -e .

echo "Environment setup is complete!"
