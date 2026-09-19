ROOTDIR  := ../..

CXX      := g++
CXXFLAGS := -std=c++17 -Wall -Wextra -Wconversion \
            -I$(ROOTDIR)/platform \
            -I$(ROOTDIR)/app/ui/dwin \
            -I$(ROOTDIR)/core
LDFLAGS  := -static

TARGET    := dwin_transport_test.exe
OBJS      := dwin_transport_test.o dwin_transport_pc.o

.PHONY: all clean run

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) $(LDFLAGS) -o $@ $^

dwin_transport_test.o: dwin_transport_test.cpp
	$(CXX) $(CXXFLAGS) -c -o $@ $<

dwin_transport_pc.o: $(ROOTDIR)/platform/pc/dwin_transport.cpp
	$(CXX) $(CXXFLAGS) -c -o $@ $<

clean:
	rm -f $(OBJS) $(TARGET)

run: $(TARGET)
	./$(TARGET)