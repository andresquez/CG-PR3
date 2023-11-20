cmake_minimum_required(VERSION 3.10)

# set the project name
project(RayCastingPR3)

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
target_link_libraries(SR ${SDL2_LIBRARIES})

# find_package(SDL2_image REQUIRED)
# include_directories(${SDL2_IMAGE_INCLUDE_DIRS})
# target_link_libraries(SR ${SDL2_IMAGE_LIBRARIES})
# include_directories(/opt/homebrew/Cellar/sdl2_image/2.6.3_2/include)


find_package(glm REQUIRED)
include_directories(${GLM_INCLUDE_DIRS})
include_directories(/opt/homebrew/Cellar/glm/0.9.9.8/include)


# link libraries
target_link_libraries(SR ${SDL2_LIBRARIES} ${SDL2_IMAGE_LIBRARIES} ${GLM_LIBRARIES})
