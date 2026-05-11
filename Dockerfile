FROM gcc:latest
WORKDIR /app

COPY . /app
RUN g++ -o collatz code.cpp

CMD ["./collatz"]
