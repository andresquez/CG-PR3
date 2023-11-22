cmake_minimum_required(VERSION 3.10)

# set the project name
project(SR)

# specify the C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(CMAKE_BUILD_TYPE Release)

# set the profiling flag
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pg")

# find all .cpp files in the src directory
file(GLOB SOURCE_FILES ${PROJECT_SOURCE_DIR}/src/*.cpp)

# add the executable
add_executable(SR ${SOURCE_FILES})

# find and include SDL2 and GLM
find_package(SDL2 REQUIRED)
include_directories(${SDL2_INCLUDE_DIRS})
target_link_libraries(SR PRIVATE ${SDL2_LIBRARIES})


# Especifica la ruta donde se encuentra instalada SDL2_image
set(SDL2_IMAGE_INCLUDE_DIRS /opt/homebrew/Cellar/sdl2_image/2.6.3_2/include/SDL2)
set(SDL2_IMAGE_LIBRARIES /opt/homebrew/lib/libSDL2_image.dylib)

# Asegúrate de que las rutas sean válidas
include_directories(${SDL2_IMAGE_INCLUDE_DIRS})
include_directories(/opt/homebrew/Cellar/sdl2_image/2.6.3_2/include/SDL2)


# Agrega las rutas de inclusión y enlace para SDL2_image
target_include_directories(SR PRIVATE ${SDL2_IMAGE_INCLUDE_DIRS})
target_link_libraries(SR PRIVATE ${SDL2_IMAGE_LIBRARIES})


find_package(glm REQUIRED)
include_directories(${GLM_INCLUDE_DIRS})
include_directories(/opt/homebrew/Cellar/glm/0.9.9.8/include)

# link libraries
target_link_libraries(SR PRIVATE ${GLM_LIBRARIES})
