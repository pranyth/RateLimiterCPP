FROM ubuntu:22.04

RUN apt update && apt install -y g++

WORKDIR /app

COPY server.cpp .

RUN g++ server.cpp -o server

EXPOSE 8080

CMD ["./server"]
