
## Compile the executable file
goto the program folder and use below command line
```bash
gcc -o sdlock  main.c libscsi.c

```

## Lock/Unlock SD
```bash
sudo ./sdlock <Option> [<Password>] <Device>
```
example1:
```bash
sudp ./sdlock -s transcend /dev/sda
```
this command will set "transcend" as password for device /dev/sda

example2:
```bash
sudp ./sdlock -e /dev/sda
```
this command will force erase device /dev/sda

## Check SD status

```bash
sudo ./sdlock -t <Device>
```
this command will show the device is lock or not

## Help
```bash
sudo ./sdlock --help 
```
