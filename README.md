# RPLMQoS_BELACEL - Installation and Usage

This project is an implementation of RPL with MQoS (Multi-path Quality of Service) support for Contiki-NG.

 Prerequisites

- Ubuntu (tested on Ubuntu 20.04 LTS and later versions)
- Internet connection
- Administrator rights (sudo)

 Automatic Installation

The project includes an automatic installation script that configures the entire environment.

# Installation Steps

1. Copy the project
   ```bash
   # Copy the RPLMQoS_BELACEL folder to your machine
   # For example, to the desktop:
   cp -r RPLMQoS_BELACEL ~/Desktop/
   ```

2. Run the installation script
   ```bash
   cd ~/Desktop/RPLMQoS_BELACEL
   chmod +x install.sh
   ./install.sh
   ```

The script will automatically:
- Check prerequisites
- Install system dependencies
- Install Contiki-NG
- Install MSP430 tools
- Configure the project
- Compile the project

# Script Messages

The script displays colored messages to track progress:
- 🟢 [INFO] : Normal information messages
- 🟡 [WARNING] : Warnings
- 🔴 [ERROR] : Errors

 Manual Installation

If you prefer to install manually, follow these steps:

1. Install dependencies
   ```bash
   sudo apt-get update
   sudo apt-get install -y build-essential git wget curl python3 python3-pip
   ```

2. Install Contiki-NG
   ```bash
   cd ~
   git clone https://github.com/contiki-ng/contiki-ng.git
   cd contiki-ng
   git submodule update --init --recursive
   ```

3. Install MSP430 tools
   ```bash
   cd ~/contiki-ng/tools
   sudo ./install-msp430.sh
   ```

4. Configure the project
   ```bash
   # Copy the project to the examples folder
   cp -r RPLMQoS_BELACEL ~/contiki-ng/examples/
   cd ~/contiki-ng/examples/RPLMQoS_BELACEL
   ```

5. Compile
   ```bash
   make TARGET=sky
   ```

 Project Structure

```
RPLMQoS_BELACEL/
├── install.sh           # Automatic installation script
├── clean.sh            # Cleaning script
├── Makefile            # Compilation configuration
├── project-conf.h      # Project configuration
├── sky-conf.h          # Sky platform configuration
├── compilation_log.txt # Compilation log
├── src/                # Source files
├── include/            # Header files
├── contiki_deps/       # Contiki dependencies
├── tests/              # Test files
└── figures/            # Project figures
```

 Cleaning the Project

To clean the project and remove non-essential files:
```bash
./clean.sh
```

 Troubleshooting

If you encounter problems:

1. Compilation errors
   - Check the `compilation_log.txt` file
   - Ensure Contiki-NG is properly installed
   - Verify MSP430 tools are installed

2. Permission errors
   - Run the script as a normal user (not root)
   - Check file permissions

3. Dependency errors
   - Verify all dependencies are installed
   - Run `sudo apt-get update` before installation


