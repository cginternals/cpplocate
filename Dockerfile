ARG BASE=willyscheibel/cpp-base:latest
ARG BASE_DEV=willyscheibel/cpp-base:dev
ARG PROJECT_NAME=cpplocate

# BUILD

FROM $BASE_DEV AS build

ARG PROJECT_NAME
ARG COMPILER_FLAGS="-j 4"

ENV cpplocate_DIR="$WORKSPACE/$PROJECT_NAME"

WORKDIR $WORKSPACE/$PROJECT_NAME

ADD cmake cmake
ADD docs docs
ADD deploy deploy
ADD source source
ADD CMakeLists.txt CMakeLists.txt
ADD configure configure
ADD $PROJECT_NAME-config.cmake $PROJECT_NAME-config.cmake
ADD $PROJECT_NAME-logo.png $PROJECT_NAME-logo.png
ADD $PROJECT_NAME-logo.svg $PROJECT_NAME-logo.svg
# Special File
ADD liblocate-config.cmake liblocate-config.cmake
ADD LICENSE LICENSE
ADD README.md README.md
ADD AUTHORS AUTHORS

RUN ./configure
RUN CMAKE_OPTIONS="-DOPTION_BUILD_TESTS=Off -DCMAKE_INSTALL_PREFIX=$WORKSPACE/$PROJECT_NAME-install" ./configure
RUN cmake --build build -- $COMPILER_FLAGS
RUN cmake --build build --target install

# DEPLOY

FROM $BASE AS deploy

ARG PROJECT_NAME
ARG WORKSPACE

COPY --from=build $WORKSPACE/$PROJECT_NAME-install $WORKSPACE/$PROJECT_NAME

ENV LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$WORKSPACE/$PROJECT_NAME/lib
