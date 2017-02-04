#!/bin/bash
S_PATH=`dirname $(readlink -f $0)`

echo "uploading server code..."
scp -r $S_PATH/bbd_server/* f0lk@178.62.6.44:/opt/bbd_server/
if [ $? != 0 ]
then
	echo "An error has occured uploading!"
	exit 1
fi
echo "finished!"
