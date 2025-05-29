FROM debian:bullseye-slim
 
RUN apt-get update && apt-get install -y \
    build-essential \
    wget \
    git \
    clang \
    make \
    cmake \
    meson \
    ninja-build \
    libpqxx-dev \
    libboost-all-dev \
    libpthread-stubs0-dev \
    && apt-get clean 
 
WORKDIR /app
COPY . .
 
RUN wget https://github.com/CrowCpp/Crow/releases/download/v1.2.1.2/crow_all.h -O include/crow.h && \ 
    meson setup build && ninja -C build
 
EXPOSE 3000
 
CMD ["./build/crow-server"]
