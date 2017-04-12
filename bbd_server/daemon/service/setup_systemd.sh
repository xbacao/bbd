#!/bin/bash
S_PATH=`dirname $(readlink -f $0)`
BIN_FILE=bbda_server
BIN_FOLDER=/opt/bbd_server
SERVICE=bbda_server.service

if [[ $EUID -ne 0 ]]; then
	echo "This script must be run as root" 1>&2
	exit 1
fi

if [ ! -f $S_PATH/../$BIN_FILE ]; then
	echo "Binary not found! (execute make)"
	exit 2
fi

if [ ! -d "$BIN_FOLDER/logs" ]; then
	mkdir $BIN_FOLDER/logs
fi
if [ ! -d "$BIN_FOLDER/run" ]; then
	mkdir $BIN_FOLDER/run
fi

cp $S_PATH/$SERVICE /etc/systemd/system
if [ $? != 0 ]
then
	echo "An error has occured copying the $SERVICE file!"
	exit 4
fi

systemctl enable $SERVICE
if [ $? != 0 ]
then
	echo "An error has occured enabling the $SERVICE service!"
	exit 6
fi
