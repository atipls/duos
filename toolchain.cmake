set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_PROCESSOR BCM2835)

IF ($TC_PATH)
    STRING(APPEND TC_PATH "/")
ENDIF ()

set(CROSS_COMPILE arm-none-eabi-)

set(CMAKE_C_COMPILER ${CROSS_COMPILE}gcc)
set(CMAKE_CXX_COMPILER ${CROSS_COMPILE}g++)
set(CMAKE_ASM_COMPILER ${CROSS_COMPILE}gcc)

set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)

set(CMAKE_OBJCOPY ${TC_PATH} ${CROSS_COMPILE}objcopy
        CACHE FILEPATH "The toolchain objcopy command " FORCE)

set(CMAKE_OBJDUMP ${TC_PATH} ${CROSS_COMPILE}objdump
        CACHE FILEPATH "The toolchain objdump command " FORCE)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -nostartfiles")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -mfloat-abi=hard")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-exceptions")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-rtti")

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -mfpu=vfp")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -march=armv6zk")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -mtune=arm1176jzf-s")

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}" CACHE STRING "")
set(CMAKE_ASM_FLAGS "${CMAKE_CXX_FLAGS}" CACHE STRING "")

if (NOT RUNNING_IN_QEMU)
    add_definitions(-DRPI0=1)
    set(BOARD rpi0)
else()
    add_definitions(-DRPI2=1)
    set(BOARD rpi2)
endif()