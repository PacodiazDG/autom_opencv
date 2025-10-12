cmake_minimum_required(VERSION 3.10)
project(mi_proyecto)

# Estándar de C++
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Incluir archivos fuente automáticamente desde src/
file(GLOB SOURCES
    src/*.cpp
    main.cpp
)

# Incluir encabezados
include_directories(include)

# Si usas OpenCV
find_package(OpenCV REQUIRED)
include_directories(${OpenCV_INCLUDE_DIRS})

# Crear ejecutable
add_executable(programa ${SOURCES})

# Vincular OpenCV si es necesario
target_link_libraries(programa ${OpenCV_LIBS})
