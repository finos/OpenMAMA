#!/bin/bash

# Make all unhandled erroneous return codes fatal
set -e

grep -E -v "^#" ./supported-tags.txt | while read -r TAG
do
    BASE_TAG=$(basename "$TAG" | sed 's/:/-/')
    docker build -t "openmama/openmama-dev:${BASE_TAG}" -f Dockerfile.dependencies . --build-arg IMAGE="$TAG"
    docker push "openmama/openmama-dev:${BASE_TAG}"
done
