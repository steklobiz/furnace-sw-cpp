ROOTDIR  := ../..

CXX      := g++
CXXFLAGS := -std=c++17 -Wall -Wextra -Wpedantic \
            -I$(ROOTDIR)/app \
            -I$(ROOTDIR)/app/profiles \
            -I$(ROOTDIR)/core \
            -I$(ROOTDIR)/core/log \
            -I$(ROOTDIR)/platform/pc
LDFLAGS  := -static -pthread

TARGET   := furnace_test.exe
OBJS     := main.o furnace.o profiles.o hal.o

.PHONY: all clean run

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) $(LDFLAGS) -o $@ $^

main.o: main.cpp
	$(CXX) $(CXXFLAGS) -c -o $@ $<

furnace.o: $(ROOTDIR)/app/furnace.cpp
	$(CXX) $(CXXFLAGS) -c -o $@ $<

profiles.o: $(ROOTDIR)/app/profiles/profiles.cpp
	$(CXX) $(CXXFLAGS) -c -o $@ $<

hal.o: $(ROOTDIR)/platform/pc/hal.cpp
	$(CXX) $(CXXFLAGS) -c -o $@ $<

clean:
	rm -f $(OBJS) $(TARGET)

run: $(TARGET)
	./$(TARGET)
