# Use configurable image name
ARG IMAGE=centos:7.5.1804
FROM $IMAGE

# Assign arguments with defaults
ARG VERSION_QPID=0.24.0
ARG VERSION_GTEST=1.8.0
ARG VERSION_APR=1.6.3
ARG WITH_UNITTEST=ON
ARG IMAGE

# Promote arguments to environment variables
ENV VERSION_QPID $VERSION_QPID
ENV VERSION_GTEST $VERSION_GTEST
ENV VERSION_APR $VERSION_APR
ENV WITH_UNITTEST $WITH_UNITTEST
ENV IMAGE $IMAGE

# Additional defaults for underlying scripts
ENV IS_DOCKER_BUILD true
ENV SDKMAN_DIR /usr/local/sdkman
ENV DEPS_DIR /apps/deps
ENV SRC_DIR /apps/src
ENV PREFIX /apps/install
ENV INSTALL_DIR $PREFIX/opt/openmama
ENV RELEASE_DIR /apps/release

# Run script to update dependencies
ADD release_scripts/install-dependencies.sh $SRC_DIR/release_scripts/
RUN $SRC_DIR/release_scripts/install-dependencies.sh

# Add the rest of the source code here so code changes don't trigger dependency regeneration
ADD . $SRC_DIR

# Run out-of-source builds
WORKDIR $SRC_DIR/build

# Configure the build. SDKMAN needs bash. Note strict-prototypes is temporary for cmake on centos bug with -Werror.
RUN cmake -DCMAKE_BUILD_TYPE=RelWithDebInfo \
          -DBIN_GRADLE=$SDKMAN_DIR/candidates/gradle/current/bin/gradle \
          -DCMAKE_INSTALL_PREFIX=$INSTALL_DIR \
          -DWITH_JAVA=ON \
          -DWITH_UNITTEST=$WITH_UNITTEST \
          -DINSTALL_RUNTIME_DEPENDENCIES=ON \
          -DCMAKE_CXX_FLAGS="-Werror -Wno-error=strict-prototypes -Wno-deprecated-declarations" \
          -DCMAKE_C_FLAGS="-Werror -Wno-error=strict-prototypes -Wno-deprecated-declarations" \
          ..

# Perform the build and install
RUN cmake --build . --config RelWithDebInfo --target install

# Perform unit tests
WORKDIR $SRC_DIR
RUN test "$WITH_UNITTEST" = "ON" && OPENMAMA_INSTALL_DIR=$INSTALL_DIR python release_scripts/ci-run.py

# Clean up Unit Test files - we don't need them any more
WORKDIR $INSTALL_DIR
RUN rm -f *.xml bin/UnitTest*

# Include the test data
WORKDIR $INSTALL_DIR/data
RUN curl -sL https://github.com/OpenMAMA/OpenMAMA-testdata/archive/master.tar.gz | tar xz --strip 1

# Move profile to top level
RUN cp profiles/profile.openmama . && rm -rf profiles

# When RPMs are generated, they'll go here
WORKDIR $RELEASE_DIR

# Generate the package (deb / rpm / tarball). Will become script
CMD $SRC_DIR/release_scripts/build-package.sh
