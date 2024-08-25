# Specify the system you're cross-compiling for
set(CMAKE_SYSTEM_NAME Windows)
set(CMAKE_SYSTEM_VERSION 10)

# Specify the cross-compiler
set(CMAKE_C_COMPILER x86_64-w64-mingw32-gcc)
set(CMAKE_CXX_COMPILER x86_64-w64-mingw32-g++)

# Define the search paths for include files and libraries
set(CMAKE_PREFIX_PATH /home/mason/quasi-msys2/root/ucrt64)
set(CMAKE_INCLUDE_PATH /home/mason/quasi-msys2/root/ucrt64/include)
set(CMAKE_LIBRARY_PATH /home/mason/quasi-msys2/root/ucrt64/lib)

# Adjust the behavior of the find commands to search for files in the target system first
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)

# Optional: Specify the linker flags if you need to link against specific Windows libraries
set(CMAKE_EXE_LINKER_FLAGS "-static-libgcc -static-libstdc++")
