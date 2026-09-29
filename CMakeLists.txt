cmake_minimum_required(VERSION 3.22)
project(furnace CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Default to PC if not specified. We can override it with -DTARGET_PLATFORM=STM32
set(TARGET_PLATFORM "PC" CACHE STRING "Target platform: PC or STM32")

# Common include dirs for both targets
set(COMMON_INCLUDES
        ${CMAKE_SOURCE_DIR}/app
        ${CMAKE_SOURCE_DIR}/app/alarm
        ${CMAKE_SOURCE_DIR}/app/aggregator
        ${CMAKE_SOURCE_DIR}/app/config
        ${CMAKE_SOURCE_DIR}/app/furnace
        ${CMAKE_SOURCE_DIR}/app/profiles
        ${CMAKE_SOURCE_DIR}/app/settings
        ${CMAKE_SOURCE_DIR}/app/tc_parser
        ${CMAKE_SOURCE_DIR}/app/ui
        ${CMAKE_SOURCE_DIR}/app/ui/tui
        ${CMAKE_SOURCE_DIR}/app/ui/dwin
        ${CMAKE_SOURCE_DIR}/core
        ${CMAKE_SOURCE_DIR}/core/pid
        ${CMAKE_SOURCE_DIR}/core/scheduler
        ${CMAKE_SOURCE_DIR}/platform
)

# -----------------------------------------------------------------------------
# PC target
# -----------------------------------------------------------------------------
if(TARGET_PLATFORM STREQUAL "PC")

    add_executable(furnace_pc
            main.cpp
            app/app.cpp
            platform/pc/hal.cpp
            core/pid/pid.cpp
            core/scheduler/scheduler.cpp
            app/aggregator/data_aggregator.cpp
            app/alarm/alarm.cpp
            app/furnace/furnace.cpp
            app/profiles/profiles.cpp
            app/settings/settings.cpp
            app/tc_parser/tc_parser.cpp
            app/ui/ui.cpp
    )

    if(PC_UI STREQUAL "TUI")

        target_sources(furnace_pc PRIVATE
                app/ui/tui/tui.cpp
        )

        target_compile_definitions(furnace_pc PRIVATE
                UI_BACKEND_TUI
        )

    elseif(PC_UI STREQUAL "DWIN")

        target_sources(furnace_pc PRIVATE
                app/ui/dwin/dwin_protocol.cpp
                app/ui/dwin/dwin_renderer.cpp
                app/ui/dwin/dwin_transport.cpp
        )

        target_compile_definitions(furnace_pc PRIVATE
                UI_BACKEND_DWIN
        )

    else()

        message(FATAL_ERROR "Unknown PC_UI: ${PC_UI}")

    endif()

    target_compile_definitions(furnace_pc PRIVATE
            PLATFORM_PC
    )

    target_include_directories(furnace_pc PRIVATE ${COMMON_INCLUDES})

    # -----------------------------------------------------------------------------
    # STM32 target
    # -----------------------------------------------------------------------------
elseif(TARGET_PLATFORM STREQUAL "STM32")

    add_executable(furnace_mcu
            main.cpp
            app/app.cpp
            platform/mcu/hal.cpp
            platform/mcu/startup.cpp
            # Add your CMSIS/HAL sources here too, e.g.:
            # Drivers/CMSIS/Device/ST/STM32G0xx/Source/Templates/system_stm32g0xx.c
    )

    target_include_directories(furnace_mcu PRIVATE
            ${COMMON_INCLUDES}
            ${CMAKE_SOURCE_DIR}/drivers
            ${CMAKE_SOURCE_DIR}/drivers/systick
            ${CMAKE_SOURCE_DIR}/platform/mcu
            # Add CMSIS / HAL include paths here
    )

    target_compile_definitions(furnace_mcu PRIVATE
            PLATFORM_STM32
    )

    # --- Compile flags ---
    target_compile_options(furnace_mcu PRIVATE
            -mcpu=cortex-m0plus
            -mthumb
            -Os
            -Wall -Wextra
            -ffreestanding
            -fno-exceptions
            -fno-rtti
            -ffunction-sections
            -fdata-sections
    )

    # --- Link flags ---
    target_link_options(furnace_mcu PRIVATE
            -mcpu=cortex-m0plus
            -mthumb
            -T${CMAKE_SOURCE_DIR}/linker/stm32g030f6p6.ld
            --specs=nano.specs
            --specs=nosys.specs
            -nostartfiles
            -Wl,--gc-sections
            -Wl,-Map=${CMAKE_CURRENT_BINARY_DIR}/furnace_mcu.map
            -Wl,--print-memory-usage
    )

    # Optional: produce .hex and .bin
    add_custom_command(TARGET furnace_mcu POST_BUILD
            COMMAND ${CMAKE_OBJCOPY} -O ihex   $<TARGET_FILE:furnace_mcu> furnace_mcu.hex
            COMMAND ${CMAKE_OBJCOPY} -O binary $<TARGET_FILE:furnace_mcu> furnace_mcu.bin
    )

else()
    message(FATAL_ERROR "Unknown TARGET_PLATFORM: ${TARGET_PLATFORM}")
endif()

if(TARGET_PLATFORM STREQUAL "PC")
    add_subdirectory(tests/dwin_renderer)
endif()