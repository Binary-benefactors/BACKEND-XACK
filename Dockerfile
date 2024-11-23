# Базовый образ
FROM ubuntu:20.04

# Установка зависимостей
RUN apt-get update && apt-get install -y --no-install-recommends \
    build-essential \
    cmake \
    wget \
    git \
    libtorch-dev \
    libopencv-dev \
    pkg-config \
    && rm -rf /var/lib/apt/lists/*

# Установка LibTorch для CPU
RUN wget https://download.pytorch.org/libtorch/cpu/libtorch-shared-with-deps-1.12.1%2Bcpu.zip -O /tmp/libtorch.zip && \
    unzip /tmp/libtorch.zip -d /opt && rm /tmp/libtorch.zip
ENV Torch_DIR=/opt/libtorch/share/cmake/Torch

# Копирование проекта
WORKDIR /app
COPY . .

# Сборка проекта
RUN cmake -DCMAKE_PREFIX_PATH=/opt/libtorch . && \
    cmake --build .

# Точка входа
CMD ["./TorchInference"]
