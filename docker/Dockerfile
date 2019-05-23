FROM centos:centos7.6.1810

# Set up cloudsmith repository
RUN curl -1sLf \
  'https://dl.cloudsmith.io/public/openmama/openmama/cfg/setup/bash.rpm.sh' \
  | bash

# Go ahead and install openmama. Note config will be in /opt/openmama/config.
RUN yum install -y openmama

# Add profile script for setting PATHs etc
ADD profile.openmama.sh /etc/profile.d/

# Add mama.properties with environment variable overrides
ADD mama.docker.properties /opt/openmama/config/mama.properties

# Add entrypoint script to image
ADD docker-entrypoint.sh /

# Defer to entrypoint bash script to figure out how to proceed
ENTRYPOINT [ "/docker-entrypoint.sh" ]