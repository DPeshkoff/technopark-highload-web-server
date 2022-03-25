FROM ubuntu:latest

EXPOSE 8000
RUN apt-get update && \
    apt-get install -y software-properties-common && \
    add-apt-repository ppa:ubuntu-toolchain-r/test && \
    apt-get update && \
    apt-get install -y \
       gcc-11 g++-11 \
      cmake

COPY . /app

WORKDIR /app/build

RUN update-alternatives --install /usr/bin/gcc gcc /usr/bin/gcc-11 60 --slave /usr/bin/g++ g++ /usr/bin/g++-11 && \
    update-alternatives --config gcc

RUN cmake .. && cmake --build .

ENTRYPOINT [ "/app/build/hw2" ]
