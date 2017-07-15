FROM gcc4.9-dev

USER root

RUN apt-get install -y cmake

## Install websocket library

RUN cd /usr/local/src \
    && git clone https://github.com/warmcat/libwebsockets.git \
    && cd libwebsockets \
    && mkdir build \
    && cd build \
    && cmake .. \
    && make \
    && make install

## Install MQTT client library

RUN cd /usr/local/src \
    && git clone https://github.com/eclipse/paho.mqtt.c.git \
    && cd paho.mqtt.c \
    && make \
    && make install

## install JSON library

RUN cd /usr/local/src \
    && git clone https://github.com/json-c/json-c.git \
    && cd json-c \
    && sh autogen.sh \
    && ./configure \
    && make \
    && make install
