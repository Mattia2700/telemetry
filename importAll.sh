#!/usr/bin/env bash

device="ubuntu@192.168.1.104"      ## TELEMETRY pwd telemetrypi

FOLDERNAME="$(date +"%d-%b-%Y__%H-%M-%S")"
SOURCEPATH="~/logs/"
DESTPATH="/home/filippo/Desktop/CANDUMP_DEFAULT_FOLDER/"

echo $FOLDERNAME

ssh $device "
cd ~/logs &&
rm *.zip
zip -r -9 $FOLDERNAME'.zip' * &&
rm -r */" &&

scp $device:~/logs/$FOLDERNAME'.zip' $DESTPATH &&
unzip $DESTPATH$FOLDERNAME'.zip' -d $DESTPATH$FOLDERNAME &&
rm $DESTPATH$FOLDERNAME'.zip'
