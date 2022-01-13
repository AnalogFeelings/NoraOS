cmake_minimum_required(VERSION 3.15)

set(TOOLCHAIN "clang")
set(CMAKE_TOOLCHAIN_FILE ${CMAKE_CURRENT_SOURCE_DIR}/cmake/${TOOLCHAIN}-toolchain.cmake)
project(stivale2-cmake-boilerplate VERSION 0.0.1 DESCRIPTION "An example stivale2 kernel with CMake build system.")
enable_language(ASM_NASM)

set(CMAKE_EXECUTABLE_SUFFIX ".elf")
add_executable(kernel src/main.c)

target_compile_options(kernel PUBLIC $<$<COMPILE_LANGUAGE:C,CXX>:
    -fno-stack-protector
    -fno-pic
    -mabi=sysv
    -mno-80387
    -mno-mmx
    -mno-3dnow
    -mno-sse
    -mno-sse2
    -mno-ssse3
    -mno-sse4
    -mno-sse4a
    -mno-sse4.1
    -mno-sse4.2
    -mno-avx
    -mno-avx2
    -mno-avx512f
    -mno-red-zone
    -mcmodel=kernel
>)

target_link_options(kernel PUBLIC
    -T${CMAKE_CURRENT_SOURCE_DIR}/src/linker.ld
    -nostdlib
    -melf_x86_64
    -zmax-page-size=0x1000
    -static
)

target_include_directories(kernel PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/subprojects/stivale
)

set(ISO_IMAGE ${PROJECT_NAME}.iso)
add_custom_command(OUTPUT ${ISO_IMAGE} DEPENDS kernel
    COMMAND make -C ${CMAKE_CURRENT_SOURCE_DIR}/subprojects/limine
    COMMAND mkdir -p ${CMAKE_CURRENT_BINARY_DIR}/sysroot/boot
    COMMAND cp ${CMAKE_CURRENT_BINARY_DIR}/kernel.elf ${CMAKE_CURRENT_SOURCE_DIR}/src/limine.cfg ${CMAKE_CURRENT_SOURCE_DIR}/subprojects/limine/limine.sys ${CMAKE_CURRENT_BINARY_DIR}/sysroot/boot
    COMMAND cp ${CMAKE_CURRENT_SOURCE_DIR}/subprojects/limine/limine-cd.bin ${CMAKE_CURRENT_SOURCE_DIR}/subprojects/limine/limine-eltorito-efi.bin ${CMAKE_CURRENT_BINARY_DIR}/sysroot
    COMMAND xorriso -as mkisofs -b limine-cd.bin -no-emul-boot -boot-load-size 4 -boot-info-table --efi-boot limine-eltorito-efi.bin -efi-boot-part --efi-boot-image --protective-msdos-label ${CMAKE_CURRENT_BINARY_DIR}/sysroot -o ${CMAKE_CURRENT_BINARY_DIR}/${ISO_IMAGE}
    COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/subprojects/limine/limine-install ${CMAKE_CURRENT_BINARY_DIR}/${ISO_IMAGE}
)

add_custom_target(iso ALL DEPENDS ${ISO_IMAGE})
add_custom_target(run DEPENDS ${ISO_IMAGE} COMMAND qemu-system-x86_64 -M q35 -m 2G -cdrom ${ISO_IMAGE})
