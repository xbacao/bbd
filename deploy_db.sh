echo "uploading sql script..."
scp mySQL/bbd.sql f0lk@139.59.170.48:/home/f0lk/bbd_server
if [ $? != 0 ]
then
	echo "An error has occured uploading!"
	exit 1
fi
echo "finished!"
