
# KR260 board setup 
This document outlines how to bring up the Xilinx Kria KR260 SoC+FPGA board, which acts as the main compute unit for the autonomous loader. The easiest option is to utilize the a Linux image which contains most of the necessary components. Alternatively, the Linux image can be customized and built from scratch using Xilinx tools.

## (Step 1) Preparing the Linux image and base platform files:
### Option 1: using the prebuilt Linux image:
Extract the prebuilt Linux image by running the commands:
```bash
cd {PATH_TO_THIS_REPO}/software/kr260_platform/prebuilt
tar -xf petalinux-sdimage.wic.tar.xz
```
All of the necessary files can now be found in “{PATH_TO_THIS_REPO}/software/kr260_platform/prebuilt

### Option 2: building the Linux image from scratch:

<details>

<summary><strong>Click for detailed steps</strong></summary>  

### Requirements:
 - Xilinx Vitis 2023.2 [(Download)](https://www.xilinx.com/support/download/index.html/content/xilinx/en/downloadNav/vitis/2023-2.html)
 - Xilinx Petalinux tools 2023.2 [(Download)](https://www.xilinx.com/support/download/index.html/content/xilinx/en/downloadNav/vitis/2023-2.html)

### Creating a petalinux project:
Run the following commands:
```bash
cd {PATH_TO_THIS_REPO}/software/kr260_platform
source {PATH_TO_PETALINUX_INSTALL_DIR}/settings.sh
source {PATH_TO_VITIS_INSTALL_DIR}/2023.2/settings64.sh
petalinux-create --type project -s ./kr260.bsp --name linux_os
cd linux_os
petalinux-config --get-hw-description ../
petalinux-config -c kernel
petalinux-config -c rootfs
```

TODO: add script to configure packagegroups

### Building the petalinux project:
```bash
cd {PATH_TO_THIS_REPO}/software/kr260_platform/linux_os
petalinux-build
petalinux-build --sdk
petalinux-package --boot --u-boot --force
petalinux-package --wic --images-dir images/linux/ --bootfiles "ramdisk.cpio.gz.u-boot,boot.scr,Image,system.dtb,system-zynqmp-sck-kr-g-revB.dtb"
mkdir ../self-built
cp ./images/linux/petalinux-sdimage.wic ../self-built/petalinux-sdimage.wic
``` 

The Linux image can now be found in  "{PATH_TO_THIS_REPO}/software/kr260_platform/self-built/petalinux-sdimage.wic".

### Creating a devicetree and binary files for the base platform programmable logic:
```bash
cd {PATH_TO_THIS_REPO}/software/kr260_platform
xsct < create_base_platform_dt.tcl
./compile_base_platform_dt.sh
./package_base_platform.sh
mv base_platform self-built/base_platform
```

The base platform files can now be found in "{PATH_TO_THIS_REPO}/software/kr260_platform/self-built/base_platform".

</details>

## (Step 2) Installing the Linux image and base platform files
### Boot the KR260 board with Linux
Follow these steps to boot the Linux image:
1. Use your tool of choice (e.g. [BalenaEtcher](https://etcher.balena.io/)) to flash the SD card with the Linux image (petalinux-sdimage.wic)
2. Connect the KR260 board to your computer using the micro-USB port
3. Insert the SD card in the KR260 board and cycle the power
4. Launch a serial terminal such as GtkTerm (sudo apt install gtkterm)
5. Cycle through the ttyUSB serial ports until you find the correct/responsive one (ctrl+shift+s in GtkTerm)
6. Login with username: petalinux and create a new password

### Setup SSH connection to the KR260 board
Next we will setup a SSH connection to the board. First, connect an Ethernet cable or a WIFI adapter to the KR260 board and ensure you have a valid internet connection. Next, find out the IP address of the board by the following commands on the serial terminal: 
```bash
ifconfig
```
Note down the inet address of either eth0 or eth1 (depending on which port you used), e.g. "192.168.1.67", and run the following command on your **local/host** machine:
```bash
export kria_ip={MY_INET_ADDRESS}
echo -e "\nHost kria\n  HostName $kria_ip\n  User petalinux" >> ~/.ssh/config
```
If you have not already generated a SSH key on your machine, then run the command:
```bash
ssh-keygen
```
Then add the SSH key to your KR260 board by running the following command on your **local/host** machine:
```bash
ssh-copy-id petalinux@kria
```
Verify that you can establish an SSH connection using "ssh kria", and disconnect the micro-USB cable from the KR260 board.

### Install the base platform files
Finally, we will setup the programmable logic for the base platform.

First, run the following commands on the **local/host** machine:
```bash
cd {PATH_TO_THIS_REPO}/software/kr260_platform
scp -r prebuilt/ kria:~/base_platform/
```
Next, establish a SSH connection to the KR260 board and run the following commands on the KR260 board:
```bash
mv ~/base_platform /lib/firmware/xilinx/base_platform
```

## (Step 3) Installing required libraries and packages on the board:
