#!/bin/bash
S_PATH=`dirname $(readlink -f $0)`

virtualenv venv
if [ $? != 0 ]
then
	echo "An error has occured creating virtualenv!"
	exit 1
fi

cp -r $S_PATH/bbd_web $S_PATH/venv
if [ $? != 0 ]
then
	echo "An error has occured copying web server!"
	exit 1
fi


source $S_PATH/venv/bin/activate
if [ $? != 0 ]
then
	echo "An error has occured activating venv!"
	exit 1
fi

pip install -r $S_PATH/venv/bbd_web/requirements.txt
if [ $? != 0 ]
then
	echo "An error has occured installing pip packages!"
	exit 1
fi

deactivate
if [ $? != 0 ]
then
	echo "An error has occured deactivating!"
	exit 1
fi

echo "done!"