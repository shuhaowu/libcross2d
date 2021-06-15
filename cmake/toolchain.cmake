# setup toolchains

if (PLATFORM_LINUX)
    set(CMAKE_SYSTEM_NAME "Linux" CACHE STRING "" FORCE)
    set(CMAKE_SYSTEM_PROCESSOR "x86_64" CACHE STRING "" FORCE)
    set(TARGET_PLATFORM linux CACHE STRING "" FORCE)
elseif (PLATFORM_WINDOWS)
    set(CMAKE_SYSTEM_NAME Windows CACHE STRING "" FORCE)
    set(CMAKE_SYSTEM_PROCESSOR "x86_64" CACHE STRING "" FORCE)
    set(PLATFORM_LINUX ON CACHE BOOL "" FORCE)
    set(TARGET_PLATFORM windows CACHE STRING "" FORCE)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -D__WINDOWS__" CACHE STRING "C flags" FORCE)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D__WINDOWS__" CACHE STRING "CXX flags" FORCE)
    set(CMAKE_EXE_LINKER_FLAGS "-static-libgcc -static-libstdc++ -static" CACHE STRING "" FORCE)
elseif (PLATFORM_SWITCH)
    if (DEFINED ENV{DEVKITPRO})
        set(TARGET_PLATFORM switch CACHE STRING "target platform" FORCE)
        set(DEVKITPRO $ENV{DEVKITPRO} CACHE STRING "DEVKITPRO" FORCE)
        if (NOT DEFINED CMAKE_TOOLCHAIN_FILE)
            set(CMAKE_TOOLCHAIN_FILE "${DEVKITPRO}/cmake/Switch.cmake" CACHE PATH "toolchain file")
        endif ()
    else ()
        message(FATAL_ERROR "Please define DEVKITPRO to point to your SDK path!")
    endif ()
elseif (PLATFORM_3DS)
    if (NOT DEFINED CMAKE_TOOLCHAIN_FILE)
        if (DEFINED ENV{DEVKITPRO})
            set(TARGET_PLATFORM 3ds CACHE STRING "" FORCE)
            set(DEVKITPRO $ENV{DEVKITPRO} CACHE STRING "DEVKITPRO" FORCE)
            set(CMAKE_TOOLCHAIN_FILE "${DEVKITPRO}/3ds.cmake" CACHE PATH "toolchain file")
            set(CMAKE_EXE_LINKER_FLAGS_INIT "${CMAKE_EXE_LINKER_FLAGS_INIT} -L${DEVKITPRO}/portlibs/3ds/lib -L${DEVKITPRO}/libctru/lib" CACHE STRING "" FORCE)
            set(CMAKE_EXE_LINKER_FLAGS ${CMAKE_EXE_LINKER_FLAGS_INIT} CACHE STRING "" FORCE)
            set(CMAKE_C_FLAGS "-D_3DS -march=armv6k -mtune=mpcore -mfloat-abi=hard -mtp=soft -mword-relocations -ffunction-sections -fdata-sections -g" CACHE STRING "C flags")
            set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -I${DEVKITPRO}/libctru/include -I${DEVKITPRO}/portlibs/3ds/include" CACHE STRING "C flags" FORCE)
            set(CMAKE_CXX_FLAGS "${CMAKE_C_FLAGS} -fpermissive" CACHE STRING "C++ flags")
        else ()
            message(FATAL_ERROR "Please define DEVKITPRO to point to your SDK path!")
        endif ()
    endif ()
elseif (PLATFORM_VITA)
    if (NOT DEFINED CMAKE_TOOLCHAIN_FILE)
        if (NOT DEFINED ENV{VITASDK})
            set(ENV{VITASDK} "/usr/local/vitasdk")
        endif ()
        set(TARGET_PLATFORM vita CACHE STRING "" FORCE)
        set(VITASDK $ENV{VITASDK} CACHE STRING "VITASDK" FORCE)
        set(CMAKE_TOOLCHAIN_FILE "$ENV{VITASDK}/share/vita.toolchain.cmake" CACHE PATH "toolchain file")
    endif ()
elseif (PLATFORM_DREAMCAST)
    set(TARGET_PLATFORM dreamcast CACHE STRING "" FORCE)
    set(CMAKE_SYSTEM_NAME "Generic")
    set(CMAKE_SYSTEM_PROCESSOR "sh")
    set(SH_ELF_PREFIX /opt/toolchains/dc/sh-elf)
    set(KOS_BASE $ENV{KOS_BASE} CACHE STRING "" FORCE)
    set(KOS_ARCH "dreamcast" CACHE STRING "" FORCE)
    set(KOS_SUBARCH "pristine" CACHE STRING "" FORCE)
    set(KOS_AS "${SH_ELF_PREFIX}/bin/sh-elf-as" CACHE STRING "" FORCE)
    set(KOS_AFLAGS "-little" CACHE STRING "" FORCE)
    set(KOS_LD "${SH_ELF_PREFIX}/bin/sh-elf-ld" CACHE STRING "" FORCE)
    set(KOS_OBJCOPY "${SH_ELF_PREFIX}/bin/sh-elf-objcopy" CACHE STRING "" FORCE)
    set(CMAKE_OBJCOPY ${SH_ELF_PREFIX}/bin/sh-elf-objcopy CACHE FILEPATH "")
    set(CMAKE_C_COMPILER ${SH_ELF_PREFIX}/bin/sh-elf-gcc CACHE FILEPATH "")
    set(CMAKE_CXX_COMPILER ${SH_ELF_PREFIX}/bin/sh-elf-g++ CACHE FILEPATH "")
    set(CMAKE_AR ${SH_ELF_PREFIX}/bin/sh-elf-ar CACHE FILEPATH "")
    set(CMAKE_ASM_COMPILER ${SH_ELF_PREFIX}/bin/sh-elf-gcc CACHE FILEPATH "")
    set(CMAKE_LINKER ${SH_ELF_PREFIX}/bin/sh-elf-ld CACHE FILEPATH "")
    set(KOS_INC_PATHS "-I${KOS_BASE}/include -I${KOS_BASE}/kernel/arch/dreamcast/include -I${KOS_BASE}/addons/include -I${KOS_BASE}/../kos-ports/include")
    set(KOS_LIB_PATHS "-L${KOS_BASE}/lib/${KOS_ARCH} -L${KOS_BASE}/addons/lib/${KOS_ARCH} -L${KOS_BASE}/../kos-ports/lib")
    set(KOS_LIBS "-Wl,--start-group -lc -lgcc -Wl,--end-group")
    set(KOS_CFLAGS "-D__DREAMCAST__ -ml -m4-single-only -ffunction-sections -fdata-sections -D_arch_dreamcast -D_arch_sub_pristine -Wall -g -fno-builtin")
    set(CMAKE_C_FLAGS "${KOS_CFLAGS} ${KOS_INC_PATHS}" CACHE STRING "C flags" FORCE)
    set(CMAKE_CXX_FLAGS "${KOS_CFLAGS} ${KOS_INC_PATHS} -fno-operator-names -fno-rtti -fno-exceptions" CACHE STRING "C++ flags" FORCE)
    set(CMAKE_C_FLAGS_RELEASE "-O3 -DNDEBUG" CACHE STRING "CMAKE_C_FLAGS_RELEASE" FORCE)
    set(CMAKE_CXX_FLAGS_RELEASE "-O3 -DNDEBUG" CACHE STRING "CMAKE_CXX_FLAGS_RELEASE" FORCE)
    set(CMAKE_C_LINK_FLAGS "${KOS_LIB_PATHS} ${KOS_LIBS} -T${KOS_BASE}/utils/ldscripts/shlelf.xc ${KOS_LIBS}" CACHE STRING "" FORCE)
    set(CMAKE_CXX_LINK_FLAGS "${KOS_LIB_PATHS} ${KOS_LIBS} -T${KOS_BASE}/utils/ldscripts/shlelf.xc ${KOS_LIBS}" CACHE STRING "" FORCE)
    set_property(DIRECTORY PROPERTY TARGET_SUPPORTS_SHARED_LIBS FALSE)
    set(CMAKE_FIND_ROOT_PATH ${SH_ELF_PREFIX} $ENV{KOS_BASE}/ $ENV{KOS_BASE}/addons $ENV{KOS_BASE}/../kos-ports)
    set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
    set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
    set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
    set(CMAKE_EXECUTABLE_SUFFIX_C ".elf" CACHE STRING "" FORCE)
    set(CMAKE_EXECUTABLE_SUFFIX_CXX ".elf" CACHE STRING "" FORCE)
endif ()

message(STATUS "C2D: target platform: ${TARGET_PLATFORM}, cmake version: ${CMAKE_VERSION}")
