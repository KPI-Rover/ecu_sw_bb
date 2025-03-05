set(CMAKE_SYSTEM_NAME Linux)
set(CMAKE_SYSTEM_PROCESSOR arm)

# Set the cross-compiler
set(CMAKE_C_COMPILER /usr/bin/arm-linux-gnueabihf-gcc-8)
set(CMAKE_CXX_COMPILER /usr/bin/arm-linux-gnueabihf-g++-8)

# Set the system root for the cross-compiler
set(CMAKE_FIND_ROOT_PATH /usr/arm-linux-gnueabihf)

# Only search for libraries in the cross-compilation path
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
