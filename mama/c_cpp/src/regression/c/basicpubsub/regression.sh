#!/bin/bash

EXIT_STATUS=1

export LD_LIBRARY_PATH=$OPENMAMA_PATH/lib:$AVIS_PATH/lib

# Check that the environment is set
if [ -z $AVIS_PATH ]; then
	echo "Build regression test failed"
	echo "Need to set AVIS_PATH, please check documentation for details"
	exit 1
elif [ -z $OPENMAMA_PATH ]; then
	echo "Build regression test failed"
	echo "Need to set OPENMAMA_PATH, please check documentation for details"
	exit 1
elif [ -z $PYTHON_PATH ]; then
	echo "Build regression test failed"
	echo "Need to set PYTHON_PATH, please check documentation for details"
	exit 1
fi

echo "Removing previous run outputs"
if [ -f sub_out ]; then
	rm sub_out
fi
if [ -f inbox_out ]; then
	rm inbox_out
fi

# pub/sub test

echo "Running Pub/Sub Tests"

echo "starting publisher"
nohup ../tools/basicpub -m avis > /dev/null & 
pub_pid=$!

echo "starting subscriber"
nohup ../tools/basicsub -m avis > sub_out &
subsc_pid=$!

counter=0
while [ $counter -le 50 ]
do
    sleep 1
    echo -n "."
    result=`ps -p $pub_pid`
    pub_status=$?
    if [ $pub_status -eq 1 ]; then
        break
    else
        counter=$(($counter+1))
    fi
done

echo " "

result=`ps -p $pub_pid`
pub_status=$?

result=`ps -p $subsc_pid`
sub_status=$?

if [ $pub_status -ne 1 ]; then

	kill $pub_pid
fi
if [ $sub_status -ne 1 ]; then

	kill $subsc_pid

fi

$PYTHON_PATH comparelog.py expectedsub_out sub_out
pubsub_result=$?
if [ $pubsub_result -eq 0 ]; then
        echo "Pass:Pub/Sub tests"
	EXIT_STATUS=0
else
        echo "Fail:Pub/Sub tests"
	EXIT_STATUS=1
fi


#request/repy test

echo "Running request/reply test"

echo "starting publisher"
nohup ../tools/basicpub -m avis > /dev/null &
pub1_pid=$!
echo "starting inbox"
nohup ../tools/basicinbox -m avis > inbox_out  2>&1 &
inbox_pid=$!

counter=0
while [ $counter -le 10 ]
do
    sleep 1 
    echo -n "."
    result=`ps -p $inbox_pid`
    inbox_status=$?
    if [ $inbox_status -eq 1 ]; then
	kill $pub1_pid 
	break
    else
	counter=$(($counter+1))
    fi
done
echo " "
result=`grep "Received reply" inbox_out`
inbox_test_result=$?

if [ $inbox_test_result -eq 0 ]; then
	echo "Pass:Request/Reply test"
	 
else
	echo "Fail:Request/Reply test"
	EXIT_STATUS=1
fi
exit $EXIT_STATUS
