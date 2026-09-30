add_executable(test_ui
        test_ui.cpp

        ${CMAKE_SOURCE_DIR}/app/ui/ui.cpp
        ${CMAKE_SOURCE_DIR}/app/aggregator/data_aggregator.cpp
        ${CMAKE_SOURCE_DIR}/app/furnace/furnace.cpp
        ${CMAKE_SOURCE_DIR}/app/profiles/profiles.cpp
        ${CMAKE_SOURCE_DIR}/app/settings/settings.cpp
        ${CMAKE_SOURCE_DIR}/app/tc_parser/tc_parser.cpp
        ${CMAKE_SOURCE_DIR}/app/alarm/alarm.cpp

        ${CMAKE_SOURCE_DIR}/core/pid/pid.cpp
        ${CMAKE_SOURCE_DIR}/platform/pc/hal.cpp
)

target_include_directories(test_ui PRIVATE
        ${CMAKE_SOURCE_DIR}/app
        ${CMAKE_SOURCE_DIR}/app/alarm
        ${CMAKE_SOURCE_DIR}/app/aggregator
        ${CMAKE_SOURCE_DIR}/app/config
        ${CMAKE_SOURCE_DIR}/app/furnace
        ${CMAKE_SOURCE_DIR}/app/profiles
        ${CMAKE_SOURCE_DIR}/app/settings
        ${CMAKE_SOURCE_DIR}/app/tc_parser
        ${CMAKE_SOURCE_DIR}/app/ui
        ${CMAKE_SOURCE_DIR}/core
        ${CMAKE_SOURCE_DIR}/core/pid
        ${CMAKE_SOURCE_DIR}/platform
)

target_compile_definitions(test_ui PRIVATE
        PLATFORM_PC
)