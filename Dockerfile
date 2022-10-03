FROM ubuntu:latest

EXPOSE 80

RUN apt-get update  \
    && apt-get install -y cmake  \
    && apt-get install -y g++  \
    && apt-get install -y make  \
    && apt-get install -y nlohmann-json3-dev

COPY . /app

WORKDIR /app/build

RUN cmake .. && cmake --build .

ENTRYPOINT [ "/app/build/highload_srv" ]
