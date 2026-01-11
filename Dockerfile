FROM ubuntu:22.04

ENV DEBIAN_FRONTEND=noninteractive

RUN apt-get update && apt-get install -y \
    build-essential \
    g++ \
    clang \
    make \
    cmake \
    git \
    siege \
    curl \
    valgrind \
    net-tools \
    vim \
    zsh \
    wget \
    ca-certificates \
    && rm -rf /var/lib/apt/lists/*

RUN sh -c "$(curl -fsSL https://raw.githubusercontent.com/ohmyzsh/ohmyzsh/master/tools/install.sh)" "" --unattended
RUN 
WORKDIR /usr/src/webserv

EXPOSE 8080 80

CMD ["/bin/zsh"]
