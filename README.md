# bbd
#client packages
arduino

#compiling
make
make upload

to read input from arduino using serial(arch linux)
busybox microcom -s 9600 /dev/ttyACM0
(install busybox)

mysql_config --cflags
mysql_config --libs
#server packages
libmysqlclient
libmysqlclient-dev
python-pip
pip install virtualenv
python-dev
libpcre3-dev
apache2-dev
#APACHE
https://modwsgi.readthedocs.io/en/develop/user-guides/quick-installation-guide.html

#Note
IN ARCH LINUX CHANGE /usr/share/arduino/hardware/archlinux-arduino to /usr/share/arduino/hardware/arduino

#TODO
change N_VALVES from static in main.cpp
