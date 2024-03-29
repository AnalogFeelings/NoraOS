cmake_minimum_required(VERSION 3.15)

set(TOOLCHAIN "clang")
set(CMAKE_TOOLCHAIN_FILE ${CMAKE_CURRENT_SOURCE_DIR}/cmake/${TOOLCHAIN}-toolchain.cmake)
project(NoraOS VERSION 0.0.1 DESCRIPTION "A megalithic kernel thing.")
enable_language(ASM_NASM)

set(CMAKE_EXECUTABLE_SUFFIX ".elf")
file(GLOB_RECURSE SOURCES RELATIVE ${CMAKE_SOURCE_DIR} "src/*.c" "src/*.asm")
add_executable(nora ${SOURCES})

target_compile_options(nora PUBLIC $<$<COMPILE_LANGUAGE:C,CXX>:
    -g
    -std=gnu99
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
    -masm=intel
>)

target_link_options(nora PUBLIC
    -T${CMAKE_CURRENT_SOURCE_DIR}/src/linker.ld
    -nostdlib
    -melf_x86_64
    -zmax-page-size=0x1000
    -static
)

target_include_directories(nora PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

# Stack Overflow is a blessing
# =============================
# Get the current working branch
execute_process(
    COMMAND git rev-parse --abbrev-ref HEAD
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    OUTPUT_VARIABLE NOS_BRANCH
    OUTPUT_STRIP_TRAILING_WHITESPACE)

# Get the latest commit hash
execute_process(
    COMMAND git rev-parse --short HEAD
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    OUTPUT_VARIABLE NOS_COMMIT_HASH
    OUTPUT_STRIP_TRAILING_WHITESPACE)

add_compile_definitions(NOS_BRANCH=\"${NOS_BRANCH}\" NOS_COMMIT_HASH=\"${NOS_COMMIT_HASH}\")

set(ISO_IMAGE ${PROJECT_NAME}.iso)
add_custom_command(OUTPUT ${ISO_IMAGE} DEPENDS nora
    COMMAND make -C ${CMAKE_CURRENT_SOURCE_DIR}/subprojects/limine
    COMMAND mkdir -p ${CMAKE_CURRENT_BINARY_DIR}/sysroot/boot
    COMMAND cp ${CMAKE_CURRENT_BINARY_DIR}/nora.elf ${CMAKE_CURRENT_SOURCE_DIR}/src/limine.cfg ${CMAKE_CURRENT_SOURCE_DIR}/subprojects/limine/limine.sys ${CMAKE_CURRENT_BINARY_DIR}/sysroot/boot
    COMMAND cp ${CMAKE_CURRENT_SOURCE_DIR}/subprojects/limine/limine-cd.bin ${CMAKE_CURRENT_SOURCE_DIR}/subprojects/limine/limine-cd-efi.bin ${CMAKE_CURRENT_BINARY_DIR}/sysroot
    COMMAND xorriso -as mkisofs -b limine-cd.bin -no-emul-boot -boot-load-size 4 -boot-info-table --efi-boot limine-cd-efi.bin -efi-boot-part --efi-boot-image --protective-msdos-label ${CMAKE_CURRENT_BINARY_DIR}/sysroot -o ${CMAKE_CURRENT_BINARY_DIR}/${ISO_IMAGE}
    COMMAND chmod +x ${CMAKE_CURRENT_SOURCE_DIR}/subprojects/limine/limine-deploy
    COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/subprojects/limine/limine-deploy ${CMAKE_CURRENT_BINARY_DIR}/${ISO_IMAGE}
)

add_custom_target(iso ALL DEPENDS ${ISO_IMAGE})
add_custom_target(run DEPENDS ${ISO_IMAGE} COMMAND qemu-system-x86_64 -M q35 -serial stdio -m 2G -cdrom ${ISO_IMAGE})
