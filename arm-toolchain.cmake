set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_PROCESSOR arm)

# This is the key line that prevents the compiler test from trying to link
set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)

# Optionally, set the specific compilers here as well
set(CMAKE_C_COMPILER arm-none-eabi-gcc)
set(CMAKE_CXX_COMPILER arm-none-eabi-g++)