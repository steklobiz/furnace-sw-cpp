ROOTDIR  := ../..

CXX      := g++
CXXFLAGS := -std=c++17 -Wall -Wextra -Wconversion \
            -I$(ROOTDIR)/core
LDFLAGS  := -static

TARGET   := ring_buffer_test.exe
OBJS     := ring_buffer.o

.PHONY: all clean run

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) $(LDFLAGS) -o $@ $^

ring_buffer.o: ring_buffer.cpp
	$(CXX) $(CXXFLAGS) -c -o $@ $<

clean:
	rm -f $(OBJS) $(TARGET)

run: $(TARGET)
	./$(TARGET)