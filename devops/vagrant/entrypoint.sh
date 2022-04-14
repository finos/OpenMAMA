#!/bin/bash

export WOMBAT_PATH=/opt/openmama/config
export LD_LIBRARY_PATH=/opt/openmama/lib

# Start SSH daemon
/usr/sbin/sshd -D > /root/sshd.log 2>&1 &

# Start capturereplay service
/opt/openmama/bin/capturereplayc -S OM -f /opt/openmama/data/playbacks/openmama_utpcasheuro_capture.5000.10.qpid.mplay -dictionary /opt/openmama/data/dictionaries/data.dict -m qpid -tport pub -r > /root/capturereplay.log 2>&1 &

# Block so container doesn't exit
tail -F /root/capturereplay.log

