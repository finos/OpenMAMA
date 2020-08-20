#!/bin/bash

export PATH=/sbin:/bin:/usr/sbin:/usr/bin:/usr/local/bin

UUID=$(uuid)
figlet -c "OpenMAMA Demo" -f small
DISPLAY= USER=$UUID /usr/local/bin/screenfetch -a /etc/openmama/screenfetch-icon-load.sh | sed "s/$UUID.*@/Welcome to /"
echo
echo "OpenMAMA is now installed to /opt/openmama using qpid and /opt/openmama/bin"
echo "is on the system \$PATH so all example OpenMAMA utilities are ready to use."
echo
echo "A market data publisher is also running ready to receive subscriptions"
echo
echo "Try:"
echo -e "\tmamalistenc -S OM -m qpid -tport sub -s DE000CM95AU4.EUR.XPAR"
echo
echo "Or if you are starting out with OpenMAMA, check out:"
echo -e "\thttps://openmama.org/quickstart.html"
echo
