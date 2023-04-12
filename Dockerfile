FROM debian:buster-slim
ENV DEBIAN_FRONTEND noninteractive

ENV TZ=America/New_York
RUN ln -snf /usr/share/zoneinfo/$TZ /etc/localtime && echo $TZ > /etc/timezone

RUN apt-get -y update && apt-get -y install \
	bc \
	build-essential \
	bzip2 \
	bzr \
	cmake \
	cmake-curses-gui \
	cpio \
	git \
	libncurses5-dev \
	make \
	rsync \
	scons \
	tree \
	unzip \
	wget \
	zip \
	sqlite3 \
  && rm -rf /var/lib/apt/lists/*

RUN mkdir -p /root/workspace
WORKDIR /root

COPY docker/support .
RUN ./setup-toolchain.sh
RUN cat setup-env.sh >> .bashrc
RUN ./setup-sqlite.sh

VOLUME /root/workspace
WORKDIR /root/workspace

CMD ["/bin/bash"]