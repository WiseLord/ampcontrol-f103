# MPD UART setup

Music Player Daemon is an application for playing music. It can be installed in any Linux, e.g:

`sudo apt install mpd`

MPD itself is a server part that only plays music. To control it, another application (client)
should be used.

Script mpd-uart.py acts like a client for MPD and is responsible for communications between
MPD daemon and Ampcontrol. It needs some python packages installed:

`sudo apt install pyserial`
`sudo apt install python-mpd2`

## Running on Linux PC

The easiest way is using USB-UART dongle. In Linux it is usually visible in system as /dev/ttyUSB0.
So, to run mpd-uart the following command should be executed:

`python3 /path/to/mpd-uart.py -p /dev/ttyUSB0`

Now, when USB-UART is connected to Ampcontrol's USART2, it's possible to control MPD instance on PC.
It works until the script is running.

## Running on Raspberry PI

Raspberry PI have GPIO header than includes UART pins. Firstly, it should be enabled on the board.
For example, in Raspi OS it can be done via "raspi-config" utility. Alternatively, it can be done
manually, by adding "enable_uart=1" parameter to /boot/config.txt file and removing any 'console=XXX`
parameters from /boot/cmdline.txt, except XXX=tty1.

After reboot the header UART (pins 8 and 10) are accessable, and mpd-uart (for instance, placed in
/home/pi/ folder) can be run as:

`python3 /home/pi/mpd-uart.py -p /dev/ttyS0`

Alternatively, the port can be specified in the script mpd-uart.py itself.

Probably, '/dev/serial0' should be used depending on RPi version.

## Daemon mode for mpd-uart.py

It's not convenient to run mpd-uart.py manually. So, it's good idea to run it as a system service.
For Raspi OS, that uses systemd, mpd-uart.service config was written.

To make it work, "mpd-uart.service" file should be placed to /etc/systemd/system/. After the new
service can be enabled for autoboot via the following command:

`sudo systemctl enable mpd-uart.service`
