PROGRAM := httpd

INCLUDEPATH :=

CXXFILES := \
	Main.cpp \
	Configuration.cpp \
	HttpHeader.cpp \
	picohttpparser.c

#
# Required variables:
#  - PROGRAM
#  - CXXFILES
#
# Optional variables:
#  - INCLUDEPATH
#  - CFLAGS
#  - CXXFLAGS
#  - LDFLAGS
#

INCLUDEPATH	:= $(INCLUDEPATH)

EXECUTABLE	:= $(PROGRAM)
TOPBUILDDIR	:= .build
BUILDDIR	:= $(TOPBUILDDIR)/$(PROGRAM)

CC		:= g++
LD		:= g++
CFLAGS		:= -g -Og -std=gnu99 $(CFLAGS)
CXXFLAGS	:= -g -Og $(CXXFLAGS)
LDFLAGS		:= $(LDFLAGS)

OFILES		:= $(CXXFILES:%.cpp=$(BUILDDIR)/%.o)
INCLUDEFLAGS	:= $(INCLUDEPATH:%=-I%)

.PHONY: all clean check




all: $(EXECUTABLE)

$(EXECUTABLE): $(OFILES)
	$(LD) $(LDFLAGS) -o$@ $^

$(BUILDDIR)/%.o: %.cpp
	mkdir -p $(dir $@)
	$(CC) -c $(CXXFLAGS) $(INCLUDEFLAGS) -o$@ $<




clean:
	rm -rf $(BUILDDIR)
	rmdir --ignore-fail-on-non-empty $(TOPBUILDDIR)
	rm -f $(EXECUTABLE)




check:
