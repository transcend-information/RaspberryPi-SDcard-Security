This document describes how to use functions which our tool provides.

**Note:The following functions have to work with Transcend RDF5 card reader.**

## Compile the executable file
Go to the project folder and use the command line below to build an executable file named 'sdsecure'.
```bash
gcc main.c libscsi.c -o sdsecure
```

## Lock/Unlock SD card

1. Set password for SD card using the following command

```bash
sudo ./sdsecure --set-pwd <Password> /dev/sdX
```

2. After setting the password, lock SD card with the password using the following command

```bash
sudo ./sdsecure --lock <Password> /dev/sdX
```

3. You can also use the following command to replace the steps of setting the password and locking the SD card. The SD card will be locked immediately.

```bash
sudo ./sdsecure --quick-lock <Password> /dev/sdX
```

4. Unlock the SD card with the password using the following command

```bash
sudo ./sdsecure --unlock <Password> /dev/sdX
```

5. Remove the password of the SD card using the following command.

```bash
sudp ./sdsecure --clear <Password> /dev/sdX
```

## Erase SD card

Force erase all data on the SD card using the following command

```bash
sudp ./sdsecure --erase /dev/sdX
```

## Check SD card status

- The following command will display the SD lock/unlock status

```bash
sudo ./sdsecure --status <Device>
```
