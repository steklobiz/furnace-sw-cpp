CXX      := g++
CXXFLAGS := -std=c++17 -Wall -Wextra -Wpedantic \
            -Iapp -Iapp/ui -Iapp/ui/tui -Iapp/ui/dwin -Iapp/profiles -Iapp/settings -Iapp/config \
            -Icore/log -Icore/log/backends
LDFLAGS := -static

SRCDIR   := app
UIDIR    := app/ui
TUIDIR   := app/ui/tui
DWINDIR  := app/ui/dwin
PROFDIR  := app/profiles
SETDIR   := app/settings
CONFDIR  := app/config
LOGDIR   := core/log
LOGBDIR  := core/log/backends
HALDIR   := platform/pc
BUILDDIR := build
TARGET   := $(BUILDDIR)/app.exe

APP_SRCS  := $(wildcard $(SRCDIR)/*.cpp)
UI_SRCS   := $(wildcard $(UIDIR)/*.cpp)
TUI_SRCS  := $(wildcard $(TUIDIR)/*.cpp)
DWIN_SRCS := $(wildcard $(DWINDIR)/*.cpp)
PROF_SRCS := $(wildcard $(PROFDIR)/*.cpp)
SET_SRCS  := $(wildcard $(SETDIR)/*.cpp)
CONF_SRCS := $(wildcard $(CONFDIR)/*.cpp)
LOG_SRCS  := $(wildcard $(LOGDIR)/*.cpp)
LOGB_SRCS := $(wildcard $(LOGBDIR)/*.cpp)
HAL_SRCS  := $(wildcard $(HALDIR)/*.cpp)
SRCS      := $(APP_SRCS) $(UI_SRCS) $(TUI_SRCS) $(DWIN_SRCS) $(PROF_SRCS) $(SET_SRCS) $(CONF_SRCS) $(LOG_SRCS) $(LOGB_SRCS) $(HAL_SRCS)
OBJS      := $(patsubst $(SRCDIR)/%.cpp, $(BUILDDIR)/%.o, $(APP_SRCS)) \
             $(patsubst $(UIDIR)/%.cpp, $(BUILDDIR)/%.o, $(UI_SRCS)) \
             $(patsubst $(TUIDIR)/%.cpp, $(BUILDDIR)/%.o, $(TUI_SRCS)) \
             $(patsubst $(DWINDIR)/%.cpp, $(BUILDDIR)/%.o, $(DWIN_SRCS)) \
             $(patsubst $(PROFDIR)/%.cpp, $(BUILDDIR)/%.o, $(PROF_SRCS)) \
             $(patsubst $(SETDIR)/%.cpp, $(BUILDDIR)/%.o, $(SET_SRCS)) \
             $(patsubst $(CONFDIR)/%.cpp, $(BUILDDIR)/%.o, $(CONF_SRCS)) \
             $(patsubst $(LOGDIR)/%.cpp, $(BUILDDIR)/log_%.o, $(LOG_SRCS)) \
             $(patsubst $(LOGBDIR)/%.cpp, $(BUILDDIR)/logb_%.o, $(LOGB_SRCS)) \
             $(patsubst $(HALDIR)/%.cpp, $(BUILDDIR)/%.o, $(HAL_SRCS))

.PHONY: all clean compiledb run

all: $(TARGET)

$(TARGET): $(OBJS) | $(BUILDDIR)
	$(CXX) $(CXXFLAGS) $(LDFLAGS) -o $@ $^

$(BUILDDIR)/%.o: $(SRCDIR)/%.cpp | $(BUILDDIR)
	$(CXX) $(CXXFLAGS) -Icore -I$(HALDIR) -c -o $@ $<

$(BUILDDIR)/%.o: $(UIDIR)/%.cpp | $(BUILDDIR)
	$(CXX) $(CXXFLAGS) -Icore -I$(HALDIR) -c -o $@ $<

$(BUILDDIR)/%.o: $(TUIDIR)/%.cpp | $(BUILDDIR)
	$(CXX) $(CXXFLAGS) -Icore -I$(HALDIR) -c -o $@ $<

$(BUILDDIR)/%.o: $(DWINDIR)/%.cpp | $(BUILDDIR)
	$(CXX) $(CXXFLAGS) -Icore -I$(HALDIR) -c -o $@ $<

$(BUILDDIR)/%.o: $(PROFDIR)/%.cpp | $(BUILDDIR)
	$(CXX) $(CXXFLAGS) -Icore -I$(HALDIR) -c -o $@ $<

$(BUILDDIR)/%.o: $(SETDIR)/%.cpp | $(BUILDDIR)
	$(CXX) $(CXXFLAGS) -Icore -I$(HALDIR) -c -o $@ $<

$(BUILDDIR)/%.o: $(CONFDIR)/%.cpp | $(BUILDDIR)
	$(CXX) $(CXXFLAGS) -Icore -I$(HALDIR) -c -o $@ $<

$(BUILDDIR)/log_%.o: $(LOGDIR)/%.cpp | $(BUILDDIR)
	$(CXX) $(CXXFLAGS) -Icore -I$(HALDIR) -c -o $@ $<

$(BUILDDIR)/logb_%.o: $(LOGBDIR)/%.cpp | $(BUILDDIR)
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
