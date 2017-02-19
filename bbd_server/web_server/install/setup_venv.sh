#!/bin/bash
S_PATH=`dirname $(readlink -f $0)`
V_ENV_PATH=$S_PATH/..

cd $V_ENV_PATH

virtualenv venv
if [ $? != 0 ]
then
	echo "An error has occured creating virtualenv!"
	exit 1
fi

source $V_ENV_PATH/venv/bin/activate
if [ $? != 0 ]
then
	echo "An error has occured activating venv!"
	exit 1
fi

pip install -r $S_PATH/requirements.txt
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
