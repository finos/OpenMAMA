#!/bin/bash

# Make all unhandled erroneous return codes fatal
set -e


RHN_USER=${RHN_USER:-openmama}
RHN_PASS=${RHN_PASS:-password}

if [[ -z "$RHN_PASS" ]]
then
    echo "RHN_PASS value not provided to match RHN_USER=$RHN_USER - failing since UBI won't build" 1>&2
    exit $LINENO
fi

grep -E -v "^#" ./supported-tags.txt | while read -r TAG
do
    echo -e "\n\nBuilding for TAG=$TAG\n"
    BASE_TAG=$(echo "$TAG" | sed -e 's#/ubi-minimal##' -e 's#^.*/##' -e 's/:/-/')
    echo docker build -t "finos/openmama:${BASE_TAG}" -f Dockerfile.dependencies . --build-arg IMAGE="$TAG" --build-arg RHN_USER="$RHN_USER" --build-arg RHN_PASS="$RHN_PASS"
    echo docker push "finos/openmama:${BASE_TAG}"
    echo docker run --rm -v `pwd`/../..:/src -w /src "finos/openmama:${BASE_TAG}" /src/devops/build/ci-run.sh
done
