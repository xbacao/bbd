#!/bin/bash
S_PATH=`dirname $(readlink -f $0)`

echo "Seting up bbd server, make sure all packages in readme are installed!"

echo "Setting up web_server..."
/bin/bash $S_PATH/web_server/install_apache_modwsgi.sh
if [ $? != 0 ]
then
	echo "Error setting up web server!"
	exit 1
fi

echo "Setting up db..."
/bin/bash $S_PATH/mySQL/setup_db.sh
if [ $? != 0 ]
then
	echo "Error setting up db!"
	exit 2
fi

echo "Setting up tcp server..."
cd $S_PATH/daemon
make -j
if [ $? != 0 ]
then
	echo "Error setting up tcp server!"
	exit 3
fi

echo "done"
