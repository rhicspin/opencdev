#!/bin/sh
echo 'APT::Default-Release "precise";' > /etc/apt/apt.conf.d/01ubuntu
echo 'deb http://old-releases.ubuntu.com/ubuntu raring main' >> /etc/apt/sources.list
echo 'Package: libboost*' >> /etc/apt/preferences
echo 'Pin: release n=raring' >> /etc/apt/preferences
echo 'Pin-Priority: 900' >> /etc/apt/preferences
