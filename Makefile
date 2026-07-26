CXX      := g++
CXXFLAGS := -std=c++17 -Wall -Wextra -Wpedantic -Iapp -Iapp/ui
LDFLAGS := -static

SRCDIR   := app
UIDIR    := app/ui
HALDIR   := platform/pc
BUILDDIR := build
TARGET   := $(BUILDDIR)/fsm.exe

APP_SRCS := $(wildcard $(SRCDIR)/*.cpp)
UI_SRCS  := $(wildcard $(UIDIR)/*.cpp)
HAL_SRCS := $(wildcard $(HALDIR)/*.cpp)
SRCS     := $(APP_SRCS) $(UI_SRCS) $(HAL_SRCS)
OBJS     := $(patsubst $(SRCDIR)/%.cpp, $(BUILDDIR)/%.o, $(APP_SRCS)) \
            $(patsubst $(UIDIR)/%.cpp, $(BUILDDIR)/%.o, $(UI_SRCS)) \
            $(patsubst $(HALDIR)/%.cpp, $(BUILDDIR)/%.o, $(HAL_SRCS))

.PHONY: all clean compiledb run

all: $(TARGET)

$(TARGET): $(OBJS) | $(BUILDDIR)
	$(CXX) $(CXXFLAGS) $(LDFLAGS) -o $@ $^

$(BUILDDIR)/%.o: $(SRCDIR)/%.cpp | $(BUILDDIR)
	$(CXX) $(CXXFLAGS) -Icore -I$(HALDIR) -c -o $@ $<

$(BUILDDIR)/%.o: $(UIDIR)/%.cpp | $(BUILDDIR)
	$(CXX) $(CXXFLAGS) -Icore -I$(HALDIR) -c -o $@ $<

$(BUILDDIR)/%.o: $(HALDIR)/%.cpp | $(BUILDDIR)
	$(CXX) $(CXXFLAGS) -Icore -I$(HALDIR) -c -o $@ $<

$(BUILDDIR):
	mkdir -p $(BUILDDIR)

clean:
	rm -rf $(BUILDDIR)

run: $(TARGET)
	$(TARGET)

compiledb: | $(BUILDDIR)
	compiledb -n make all
