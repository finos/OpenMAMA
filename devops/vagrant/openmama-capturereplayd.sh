#!/bin/bash

export WOMBAT_PATH=/opt/openmama/config
export LD_LIBRARY_PATH=/opt/openmama/lib

while ! test -f /opt/openmama/bin/capturereplayc
do
    # Capturereplay not yet installed - give it a break
    sleep 1
done

# Start capturereplay service and restart if it bombs out
while true
do
    /opt/openmama/bin/capturereplayc -S OM -f /opt/openmama/data/playbacks/openmama_utpcasheuro_capture.5000.10.qpid.mplay -dictionary /opt/openmama/data/dictionaries/data.dict -m qpid -tport pub -r > /root/capturereplay.log 2>&1 &
done

