# CMakeLists.txt
cmake_minimum_required(VERSION 3.10)

# Название проекта
project(TorchInference)

# Включение стандартов C++
set(CMAKE_CXX_STANDARD 17)

# Пути к библиотекам Torch
find_package(Torch REQUIRED)

# Установка исходного файла
add_executable(TorchInference main.cpp)

# Линковка с библиотеками Torch
set_property(TARGET TorchInference PROPERTY CXX_STANDARD 17)
target_link_libraries(TorchInference "${TORCH_LIBRARIES}")

# Если используется CUDA, добавьте следующие строки
# find_package(CUDA REQUIRED)
# target_link_libraries(TorchInference "${CUDA_LIBRARIES}")
