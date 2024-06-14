FROM gcc:latest

RUN apt update -y \
    && apt-get install -y libx11-dev \
    && mkdir simpleX11

ADD . /simpleX11

WORKDIR /simpleX11