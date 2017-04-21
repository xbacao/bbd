#!/bin/bash
S_PATH=`dirname $(readlink -f $0)`
DB_USER=root
DB_PASS=morcao123

mysql --user=$DB_USER --password=$DB_PASS < $S_PATH/bbd.sql
if [ $? != 0 ]
then
	echo "Error setting up db!"
	exit 2
fi

exit 0
