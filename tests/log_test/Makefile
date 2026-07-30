ROOTDIR  := ../..

CXX      := g++
CXXFLAGS := -std=c++17 -Wall -Wextra -Wconversion \
            -I$(ROOTDIR)/core/log
LDFLAGS  := -static

TARGET   := log_test.exe
OBJS     := main.o

.PHONY: all clean run

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) $(LDFLAGS) -o $@ $^

main.o: main.cpp
	$(CXX) $(CXXFLAGS) -c -o $@ $<

clean:
	rm -f $(OBJS) $(TARGET)

run: $(TARGET)
	./$(TARGET)
