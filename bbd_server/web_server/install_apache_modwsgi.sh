#!/bin/bash
S_PATH=`dirname $(readlink -f $0)`
APACHE=httpd-2.4.25
MOD_WSGI=mod_wsgi-4.5.14
APACHE_DIR=$S_PATH/apache
INSTALL_DIR=$S_PATH/install

echo "installing apache..."
cd $INSTALL_DIR
gzip -d $APACHE.tar.gz
if [ $? != 0 ]
then
	echo "An error has occured unpacking apache!"
	exit 1
fi

tar xvf $APACHE.tar
if [ $? != 0 ]
then
	echo "An error has occured unpacking apache!"
	exit 2
fi

cd $INSTALL_DIR/$APACHE
./configure --prefix=$APACHE_DIR
if [ $? != 0 ]
then
	echo "An error has occured configuring apache!"
	exit 3
fi

make
if [ $? != 0 ]
then
	echo "An error has occured compiling apache!"
	exit 4
fi

make install
if [ $? != 0 ]
then
	echo "An error has occured installing apache!"
	exit 5
fi

cd $INSTALL_DIR
echo "installing mod wsgi..."
tar xvfz $MOD_WSGI.tar.gz
if [ $? != 0 ]
then
	echo "An error has occured unpacking!"
	exit 1
fi

cd $INSTALL_DIR/$MOD_WSGI
./configure
if [ $? != 0 ]
then
	echo "An error has occured configuring mod wsgi!"
	exit 2
fi

make
if [ $? != 0 ]
then
	echo "An error has occured compiling mod wsgi!"
	exit 3
fi

cp $INSTALL_DIR/$MOD_WSGI/src/server/.libs/mod_wsgi.so $APACHE_DIR/modules
if [ $? != 0 ]
then
	echo "An error has occured setting mod_wsgi in apache!"
	exit 5
fi

echo "setting up virtual env..."
cd $INSTALL_DIR/
./setup_venv.sh
if [ $? != 0 ]
then
	echo "An error has occured setting up venv!"
	exit 6
fi

cp httpd.conf $APACHE_DIR/conf
if [ $? != 0 ]
then
	echo "An error has occured setting up httpd.conf!"
	exit 7
fi

echo "creating apache service"
sudo cp $APACHE_DIR/bin/apachectl /etc/init.d/apache_bbd
if [ $? != 0 ]
then
	echo "An error has occured creating apache service!"
	exit 8
fi

sudo update-rc.d apache_bbd defaults
if [ $? != 0 ]
then
	echo "An error has occured creating apache service!"
	exit 9
fi

exit 0
