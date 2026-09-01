CXX := g++

CXXFLAGS := \
    -std=c++17 \
    -Wall \
    -Wextra \
    -Wpedantic \
    -DPLATFORM_PC

INCLUDES := \
    -I../../app \
    -I../../app/furnace \
    -I../../app/profiles \
    -I../../app/settings \
    -I../../app/tc_parser \
    -I../../app/config \
    -I../../core \
    -I../../core/pid \
    -I../../core/log \
    -I../../platform \
    -I../../platform/pc

TARGET := furnace_test

SOURCES := \
    main.cpp \
    ../../app/furnace/furnace.cpp \
    ../../app/profiles/profiles.cpp \
    ../../app/settings/settings.cpp \
    ../../app/tc_parser/tc_parser.cpp \
    ../../core/pid/pid.cpp \
    ../../platform/pc/hal.cpp

$(TARGET): $(SOURCES)
	$(CXX) $(CXXFLAGS) $(INCLUDES) $(SOURCES) -o $(TARGET)

run: $(TARGET)
	./$(TARGET)

clean:
	rm -f $(TARGET)