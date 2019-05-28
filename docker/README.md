# Official OpenMAMA Docker Image

The OpenMAMA docker image is a CentOS 7 based image containing a default installation
of OpenMAMA. It may be used to run OpenMAMA based applications within isolated
containers.

## Setting up docker for communications

If you are using docker compose or already have a docker network defined you can skip
this part. Docker recommend that user-defined networks are used to facilitate communications
between docker instances. This provides the primary benefit of automatic DNS resolution
for your docker images. The simplest way to do this is:

    docker network create openmama-demo

## Using the Docker Image

To run the OpenMAMA image directly with defaults, you can run any OpenMAMA based application:

    docker run --net=openmama-demo --rm --name demo-sub -it openmama/openmama:latest mamalistenc -?

Note that addressing will be done within the docker network itself though,
so double-connection based messaging middlewares like qpid may struggle
resolving appropriate request / reply addressing when working from outside
of the docker network (since the `reply_url` host may not be accessible
from within the docker network).

There are also a few demo commands you can run. Note that for all these commands,
the use of `--name` determines the DNS name inside the docker network which
is used in the default properties, so it must remain unchanged.

To run a basic demo publisher:

    docker run --net=openmama-demo --rm --name demo-basic-pub -it openmama/openmama:latest demo-basic-pub

To run a basic demo subscriber:

    docker run --net=openmama-demo --rm --name demo-basic-sub -it openmama/openmama:latest demo-basic-sub

To run a market data demo subscriber:

    docker run --net=openmama-demo --rm --name demo-pub -it openmama/openmama:latest demo-pub

To run a market data demo subscriber:

    docker run --net=openmama-demo --rm --name demo-sub -it openmama/openmama:latest demo-sub

### Mount Points

The image ships with a default `mama.docker.properties`, but you can provide your own by
providing a docker mount containing your own configuration files to the environment.

## Extending the OpenMAMA Image

You may inherit this image and run whatever software you like:

```
FROM openmama:latest

ADD yourappname /

ENTRYPOINT ["/yourappname"]
```

This allows you to use OpenMAMA's public release without providing your own
OpenMAMA build.

## Building Docker Images

If you want to build this docker image yourself rather than using the ones from
dockerhub, you can build from the `docker` subdirectory in the `OpenMAMA`
repository using:

    docker build -t your-tag-name .