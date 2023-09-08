# systemd USB monitor example

Simple example about systemd (sd-event &amp; sd-device) to monitor USB devices on connection

## Compile and Run

Compile with **gcc**:

```
gcc usb_sd_device_monitor.c -o usbthingy `pkg-config --libs --cflags libsystemd`
```
And run it:

`./usbthingy`

**Note:** It's quite possible that on **Ubuntu** you'll need to install the `libsystem-dev` package. At least was my case for **Ubuntu 23.04** (lunar)

`sudo apt install libsystemd-dev` 

Seems that on **Arch Linux** is already installed.

