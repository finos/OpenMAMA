#!/bin/bash

# Start SSH daemon
/usr/sbin/sshd -D > /root/sshd.log 2>&1 &

# Start capturereplay service
/root/openmama-capturereplayd.sh > /root/openmama-capturereplay.log 2>&1 &

# Block so container doesn't exit
tail -F /root/openmama-capturereplay.log

