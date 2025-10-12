cmake_minimum_required(VERSION 3.10)
project(mi_proyecto)

# Usa C++17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Archivos fuente
set(SOURCES
    main.cpp
    include/processing.cpp
)

# Incluir encabezados
include_directories(include)

# Si usas OpenCV (opcional)
find_package(OpenCV REQUIRED)
include_directories(${OpenCV_INCLUDE_DIRS})

# Crear el ejecutable
add_executable(programa ${SOURCES})

# Vincular OpenCV (si lo usas)
target_link_libraries(programa ${OpenCV_LIBS})
