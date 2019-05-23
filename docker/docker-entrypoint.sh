#!/bin/bash -l
# Note that we use -l with the shebang to ensure login scripts are parsed for environment setup


# Host names for the demo transports
export DEMO_PUBLISHER_HOST=${DEMO_PUBLISHER_HOST:-demo-pub}
export DEMO_SUBSCRIBER_HOST=${DEMO_SUBSCRIBER_HOST:-demo-sub}
export DEMO_BASIC_PUBLISHER_HOST=${DEMO_BASIC_PUBLISHER_HOST:-demo-basic-pub}
export DEMO_BASIC_SUBSCRIBER_HOST=${DEMO_BASIC_SUBSCRIBER_HOST:-demo-basic-sub}

# Defaults for demo
OM_DICTIONARY_FILE=${OM_DICTIONARY_FILE:-/opt/openmama/data/dictionaries/data.dict}
OM_PLAYBACK_FILE=${OM_PLAYBACK_FILE:-/opt/openmama/data/playbacks/openmama_utpcasheuro_capture.5000.10.qpid.mplay}
OM_SYMBOL=${OM_SYMBOL:-DE000CM95AU4.EUR.XPAR}
OM_MIDDLEWARE=${OM_MIDDLEWARE:-qpid}
OM_SOURCE=${OM_SOURCE:-OM}
OM_BASIC_SYMBOL=${OM_BASIC_SYMBOL:-DEMO_TOPIC}

# Run demo commands or allow caller to provide their own
if [ -z "$1" ] || [ "$1" = "demo-pub" ]; then
    # Market data publisher
    set -x
    capturereplayc \
        -dictionary $OM_DICTIONARY_FILE \
        -m $OM_MIDDLEWARE \
        -S $OM_SOURCE \
        -tport demo_pub \
        -f $OM_PLAYBACK_FILE \
        -v -v -v -v
elif [ "$1" = "demo-sub" ]; then
    # Market data subscriber
    set -x
    mamalistenc \
        -m $OM_MIDDLEWARE \
        -S $OM_SOURCE \
        -tport demo_sub \
        -s $OM_SYMBOL \
        -v -v -v -v
elif [ "$1" = "demo-basic-pub" ]; then
    # Basic publisher
    set -x
    mamapublisherc \
        -s $OM_BASIC_SYMBOL \
        -tport demo_basic_pub \
        -m $OM_MIDDLEWARE \
        -v -v -v -v
elif [ "$1" = "demo-basic-sub" ]; then
    # Basic subscriber
    set -x
    mamasubscriberc \
        -s $OM_BASIC_SYMBOL \
        -tport demo_basic_sub \
        -m $OM_MIDDLEWARE \
        -v -v -v -v
else
    # Just run the arguments provided
    echo "Running $@..."
    exec "$@"
fi

