# RPLMQoS_BELACEL

**Author:** BELACEL Madani

---

## Project Overview

RPLMQoS_BELACEL is an implementation of the RPL routing protocol enhanced with Multi-path Quality of Service (MQoS) support, designed for Contiki-NG operating system to improve QoS in Low-Power and Lossy Networks (LLNs).

---

## Prerequisites

- Ubuntu (tested on Ubuntu 20.04 LTS and later)  
- Internet connection  
- Administrator privileges (sudo)  

---

## Automatic Installation

This project provides an automatic installation script to set up the entire environment quickly and easily.

### Installation Steps

1. Copy the project folder to your machine, for example, to your Desktop:

   ```bash
   cp -r RPLMQoS_BELACEL ~/Desktop/
   ```

2. Run the installation script:

   ```bash
   cd ~/Desktop/RPLMQoS_BELACEL
   chmod +x install.sh
   ./install.sh
   ```

The script will perform the following tasks automatically:

- Verify prerequisites  
- Install required system dependencies  
- Install Contiki-NG  
- Install MSP430 tools  
- Configure the project  
- Compile the project  

### Script Messages

The script displays colored messages to indicate status:

- 🟢 [INFO] : Informational messages  
- 🟡 [WARNING] : Warnings to be aware of  
- 🔴 [ERROR] : Errors encountered during installation  

---

## Manual Installation

If you prefer manual installation, follow these steps:

1. Install dependencies:

   ```bash
   sudo apt-get update
   sudo apt-get install -y build-essential git wget curl python3 python3-pip
   ```

2. Clone and set up Contiki-NG:

   ```bash
   cd ~
   git clone https://github.com/contiki-ng/contiki-ng.git
   cd contiki-ng
   git submodule update --init --recursive
   ```

3. Install MSP430 tools:

   ```bash
   cd ~/contiki-ng/tools
   sudo ./install-msp430.sh
   ```

4. Configure the project:

   ```bash
   cp -r RPLMQoS_BELACEL ~/contiki-ng/examples/
   cd ~/contiki-ng/examples/RPLMQoS_BELACEL
   ```

5. Compile the project:

   ```bash
   make TARGET=sky
   ```

---

## Project Structure

```
RPLMQoS_BELACEL/
├── install.sh           # Automatic installation script
├── clean.sh             # Script to clean build files and temporary data
├── Makefile             # Build configuration
├── project-conf.h       # Project-specific configuration
├── sky-conf.h           # Platform-specific configuration (Sky motes)
├── compilation_log.txt  # Log file of the last compilation
├── src/                 # Source code files
├── include/             # Header files
├── contiki_deps/        # Contiki dependencies
├── tests/               # Test scripts and files
└── figures/             # Project figures and diagrams
```

---

## Cleaning the Project

To remove compiled files and clean the workspace, run:

```bash
./clean.sh
```

---

## Troubleshooting

### Compilation Errors

- Check the `compilation_log.txt` file for detailed error messages.  
- Verify that Contiki-NG and MSP430 tools are correctly installed.  

### Permission Issues

- Avoid running scripts as root; use a normal user account.  
- Ensure scripts have executable permissions (`chmod +x`).  

### Dependency Issues

- Make sure all required packages are installed.  
- Run `sudo apt-get update` before starting installation.
