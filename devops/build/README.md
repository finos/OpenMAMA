To build a repository, use the tag name in the definition that it's based on:

    docker build -t openmama/openmama-dev:ubuntu-20.04 -f Dockerfile.dependencies . --build-arg IMAGE=ubuntu:20.04
    docker push openmama/openmama-dev:ubuntu-20.04

Tags currently supported:

* ubuntu:16.04
* ubuntu:18.04
* ubuntu:20.04
* centos:6
* centos:7
* centos:8
* fedora:30
* fedora:31
* fedora:32
