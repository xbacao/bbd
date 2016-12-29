#!/bin/bash
S_PATH=`dirname $(readlink -f $0)`

echo "uploading sql script..."
scp $S_PATH/bbd.sql f0lk@178.62.6.44:/opt/bbd_server/mysql
if [ $? != 0 ]
then
	echo "An error has occured uploading!"
	exit 1
fi
echo "finished!"
