This document provides a way to use the LOCK / UNLOCK command (CMD42) and Secure Erase via Transcend RDF5 Card Reader on Raspberry Pi / NVIDIA Jetson Nano.

After the host sets the password and sends it to the card,  the card will be locked. Data will be read and write protected, and can be viewed and changed only by entering the correct password. Where an incorrect password is provided, the card will remain locked.

A password-protected card will be automatically locked after power reset. To permanently unlock a card, please remove the password. 

## Hardware requirements	
1. Supported Platform
   - Raspberry Pi 4 or later
   - NVIDIA Jetson Nano
     
2. Supported SD cards
   - Transcend Embedded SD card / mirco SD

3. Supported Card Reader
   - Transcend RDF5

## Compile the executable file

Go to the project folder and use the command line below to build an executable file named 'sdsecure'.

```bash
gcc main.c libscsi.c -o sdsecure
```
## Usage with the RDF5 Card Reader

Please change the < device > parameter by the USB card reader device. ex. /dev/sda  
**Note:The following functions have to work with Transcend RDF5 card reader.**

## Lock/Unlock SD card

1. Set password for SD card 

```bash
sudo ./sdsecure --set-pwd <Password> <device>
```
![alt text](../captures/img_setpw.PNG?raw=true)  


2. After setting the password, lock SD card with the password

```bash
sudo ./sdsecure --lock <Password> <device>
```
![alt text](https://github.com/transcend-information/RaspberryPi-SDcard-Security/blob/raspberryPi_document/captures/img_lock.PNG?raw=true)  


3. Quick-lock command : replace the steps of setting the password and locking the SD card. The SD card will be locked immediately.

```bash
sudo ./sdsecure --quick-lock <Password> <device>
```
![alt text](https://github.com/transcend-information/RaspberryPi-SDcard-Security/blob/raspberryPi_document/captures/img_quicklock.PNG?raw=true)  


4. Unlock the SD card with the password

```bash
sudo ./sdsecure --unlock <Password> <device>
```
![alt text](https://github.com/transcend-information/RaspberryPi-SDcard-Security/blob/raspberryPi_document/captures/img_unlock.PNG?raw=true)  


5. Remove the password of the SD card using the following command.

```bash
sudo ./sdsecure --clear <Password> <device>
```
![alt text](https://github.com/transcend-information/RaspberryPi-SDcard-Security/blob/raspberryPi_document/captures/img_clearpw.PNG?raw=true)  


## Secure Erase SD card

Force erase all data on the SD card 

```bash
sudo ./sdsecure --erase <device>
```
![alt text](https://github.com/transcend-information/RaspberryPi-SDcard-Security/blob/raspberryPi_document/captures/img_erase.PNG?raw=true)  


## Check SD card status

```bash
sudo ./sdsecure --status <device>
```
![alt text](https://github.com/transcend-information/RaspberryPi-SDcard-Security/blob/raspberryPi_document/captures/img_status.PNG?raw=true)  

