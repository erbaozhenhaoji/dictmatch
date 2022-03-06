#!/bin/sh
cd $1/rpm
rpm_create -p /home/a -v $3 -r $4 $2.spec -k
