add_executable(test_dwin_renderer
        test_dwin.cpp

        ${CMAKE_SOURCE_DIR}/app/aggregator/data_aggregator.cpp
        ${CMAKE_SOURCE_DIR}/app/alarm/alarm.cpp
        ${CMAKE_SOURCE_DIR}/app/furnace/furnace.cpp
        ${CMAKE_SOURCE_DIR}/app/profiles/profiles.cpp
        ${CMAKE_SOURCE_DIR}/app/settings/settings.cpp
        ${CMAKE_SOURCE_DIR}/app/tc_parser/tc_parser.cpp
        ${CMAKE_SOURCE_DIR}/app/ui/ui.cpp

        ${CMAKE_SOURCE_DIR}/app/ui/dwin/dwin_protocol.cpp
        ${CMAKE_SOURCE_DIR}/app/ui/dwin/dwin_renderer.cpp
        ${CMAKE_SOURCE_DIR}/app/ui/dwin/dwin_transport.cpp

        ${CMAKE_SOURCE_DIR}/core/pid/pid.cpp

        ${CMAKE_SOURCE_DIR}/platform/pc/hal.cpp
        test_dwin.cpp
)

target_compile_definitions(test_dwin_renderer PRIVATE
        PLATFORM_PC
        UI_BACKEND_DWIN
)

target_include_directories(test_dwin_renderer PRIVATE
        ${CMAKE_SOURCE_DIR}/app
        ${CMAKE_SOURCE_DIR}/app/alarm
        ${CMAKE_SOURCE_DIR}/app/aggregator
        ${CMAKE_SOURCE_DIR}/app/config
        ${CMAKE_SOURCE_DIR}/app/furnace
        ${CMAKE_SOURCE_DIR}/app/profiles
        ${CMAKE_SOURCE_DIR}/app/settings
        ${CMAKE_SOURCE_DIR}/app/tc_parser
        ${CMAKE_SOURCE_DIR}/app/ui
        ${CMAKE_SOURCE_DIR}/app/ui/dwin
        ${CMAKE_SOURCE_DIR}/core
        ${CMAKE_SOURCE_DIR}/core/pid
        ${CMAKE_SOURCE_DIR}/platform
)