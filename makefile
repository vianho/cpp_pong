CC			:= g++
CPPFLAGS	:= -Wall
LDLIBS		:= -lSDL2 -lSDL2_ttf -lSDL2_mixer
TARGET		:= pong

SRCDIR		:= src
BUILDDIR	:= obj
TARGETDIR	:= bin

SRCEXT		:= cpp
OBJEXT		:= o

SOURCES		:= $(shell find src -type f -name "*.$(SRCEXT)")
OBJECTS		:= $(patsubst $(SRCDIR)/%,$(BUILDDIR)/%,$(SOURCES:.$(SRCEXT)=.$(OBJEXT)))

all: directories $(TARGET)

directories:
	@mkdir -p $(TARGETDIR)
	@mkdir -p $(BUILDDIR)

$(TARGET) : $(OBJECTS)
	$(CC) -o $(TARGETDIR)/$(TARGET) $^ $(LDLIBS)

$(BUILDDIR)/%.$(OBJEXT) : $(SRCDIR)/%.$(SRCEXT)
	$(CC) $(CPPFLAGS) -c -o $@ $<

clean:
	rm -r $(BUILDDIR)/*.$(OBJEXT)