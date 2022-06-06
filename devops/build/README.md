To build a repository, use the tag name in the definition that it's based on:

    docker build -t openmama/openmama-dev:ubuntu-20.04 -f Dockerfile.dependencies . --build-arg IMAGE=ubuntu:20.04
    docker push openmama/openmama-dev:ubuntu-20.04

Docker images currently supported:

* ubuntu:18.04
* ubuntu:20.04
* ubuntu:22.04
* centos:7
* quay.io/centos/centos:stream8
* quay.io/centos/centos:stream9