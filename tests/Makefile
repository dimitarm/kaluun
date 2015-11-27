# This is makefile skeleton that can be pretty much used as-is.
# I'm sure we can simplify it a bit.

# You can run your binary by typing
# make test
# make test CONF=Size

# Limitations:
# * You need to do a clean when adding a directory in src

# Name
NAME=test

# Flags we will add to the one we pass
CFLAGS+=-Wall -Werror

# Arch flags:
# ARCHFLAGS=-m32

CXXFLAGS:=$(CFLAGS)

CFLAGS+=-std=c0x
CXXFLAGS+=-std=c++0x

# Prefix (for install)
ifeq "$(PREFIX)" ""
	PREFIX:=usr
endif

# Destdir (for install)
ifeq "$(DESTDIR)" ""
	DESTDIR:=/
endif

# Configuration: Debug / Release
ifeq "$(CONF)" ""
	CONF=Debug
endif
ifeq "$(CONF)" "Debug"
    CFLAGS+=-O0 -g3
    CXXFLAGS+=-O0 -g3 
endif
ifeq "$(CONF)" "Size"
	CFLAGS+=-Os
	CXXFLAGS+=-Os
endif
ifeq "$(CONF)" "Release"
    CFLAGS+=-O3 
    CXXFLAGS+=-O3  
    DEFINES+=-DNDEBUG
endif

# Compilation tools
CC:=$(CROSS_COMPILE)gcc
CXX:=$(CROSS_COMPILE)g++
AR:=$(CROSS_COMPILE)ar

# Building directory
BUILD:=build/$(CROSS_COMPILE)$(CONF)

# Final distribuable binaries
DIST:=dist/$(CROSS_COMPILE)$(CONF)
TARGET:=$(DIST)/$(NAME)

# You should probably keep this include as is
INCLUDES:=-I.. 

# Dirty build things
SRCS_CPP:=$(shell ls *.cpp)

 LDLIBSOPTIONS=-lgtest -lpthread

OBJ:=\
$(addprefix $(BUILD)/,$(patsubst %.cpp,%.cpp_o,$(wildcard $(SRCS_CPP)))) 

MAKEFILE_INCLUDE:=\
$(addprefix $(BUILD)/,$(patsubst %.cpp,%.cpp_o.d,$(wildcard $(SRCS_CPP)))) 

ifneq "$(ARCHFLAGS)" ""
    CFLAGS:=$(CFLAGS) $(ARCHFLAGS)
    CXXFLAGS:=$(CXXFLAGS) $(ARCHFLAGS)
    LDFLAGS:=$(LDFLAGS) $(ARCHFLAGS)
endif

all: $(NAME)

$(NAME): $(TARGET)
	cp $(TARGET) $(NAME)

# Final binary
$(TARGET): $(OBJ) $(DIST)
	$(CXX) -o $(TARGET) $(OBJ) $(LDLIBSOPTIONS) $(LDFLAGS)

$(BUILD)/%.cpp_o: %.cpp 
	[ -d $(BUILD)/src ] || mkdir -p $(BUILD)/src
	$(CXX) -c -o $@ $< $(CXXFLAGS) $(INCLUDES) $(DEFINES) 
	$(CXX) -MM -o $@.d -MT $@ $< $(CXXFLAGS) $(INCLUDES) $(DEFINES)

-include $(MAKEFILE_INCLUDE)

$(DIST):
	mkdir -p $(DIST)

clean:
	rm -f $(NAME)	
	rm -Rf build dist
