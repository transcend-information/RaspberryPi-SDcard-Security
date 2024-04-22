This document provides a way to set SD Lock/Ulock on RaspberryPi / NVIDIA Jetson Nano

**Note:The project works for Transcend RDF5 reader.**

## Compile the executable file
goto SCSI-SDcard-CMD42 folder and use below command line
```bash
gcc -o sdlock  main.c libscsi.c

```

## Lock/Unlock SD

1. Set password for SD by following command

```bash
sudp ./sdlock --set-pwd <Password> /dev/sdX
```

2. Set password for SD and lock immediately by following command

```bash
sudp ./sdlock -quick-lock <Password> /dev/sdX
```

3. Lock SD by password by following command

```bash
sudp ./sdlock --lock <Password> /dev/sdX
```

4. Unlock SD by password by following command

```bash
sudp ./sdlock --unlock <Password> /dev/sdX
```

5. Clear password by following command

```bash
sudp ./sdlock --clear <Password> /dev/sdX
```

6. Force erase SD card by following command

```bash
sudp ./sdlock --erase  /dev/sdX
```

## Check SD status

- Get SD is Lock/Unlock status

```bash
sudo ./sdlock --status <Device>
```
