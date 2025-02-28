# GNU Make project makefile autogenerated by Premake

ifndef config
  config=debug
endif

ifndef verbose
  SILENT = @
endif

.PHONY: clean prebuild prelink

ifeq ($(config),debug)
  RESCOMP = windres
  TARGETDIR = bin
  TARGET = $(TARGETDIR)/test_implicite_surface
  OBJDIR = obj/debug/test_implicite_surface
  DEFINES += -DDEBUG
  INCLUDES += -I. -I../src -Isrc
  FORCE_INCLUDE +=
  ALL_CPPFLAGS += $(CPPFLAGS) -MD -MP $(DEFINES) $(INCLUDES)
  ALL_CFLAGS += $(CFLAGS) $(ALL_CPPFLAGS) -g -mtune=native -march=native -Wall -Wsign-compare -Wno-unused-parameter -Wno-unused-function -Wno-unused-variable -pipe -g
  ALL_CXXFLAGS += $(CXXFLAGS) $(ALL_CPPFLAGS) -g -std=c++14 -mtune=native -march=native -Wall -Wsign-compare -Wno-unused-parameter -Wno-unused-function -Wno-unused-variable -pipe -g
  ALL_RESFLAGS += $(RESFLAGS) $(DEFINES) $(INCLUDES)
  LIBS += bin/liblibgkit3GL.a -lGLEW -lSDL2 -lGL
  LDDEPS += bin/liblibgkit3GL.a
  ALL_LDFLAGS += $(LDFLAGS) -g
  LINKCMD = $(CXX) -o "$@" $(OBJECTS) $(RESOURCES) $(ALL_LDFLAGS) $(LIBS)
  define PREBUILDCMDS
  endef
  define PRELINKCMDS
  endef
  define POSTBUILDCMDS
  endef
all: prebuild prelink $(TARGET)
	@:

endif

ifeq ($(config),release)
  RESCOMP = windres
  TARGETDIR = bin
  TARGET = $(TARGETDIR)/test_implicite_surface
  OBJDIR = obj/release/test_implicite_surface
  DEFINES +=
  INCLUDES += -I. -I../src -Isrc
  FORCE_INCLUDE +=
  ALL_CPPFLAGS += $(CPPFLAGS) -MD -MP $(DEFINES) $(INCLUDES)
  ALL_CFLAGS += $(CFLAGS) $(ALL_CPPFLAGS) -fopenmp -O3 -mtune=native -march=native -Wall -Wsign-compare -Wno-unused-parameter -Wno-unused-function -Wno-unused-variable -pipe -flto
  ALL_CXXFLAGS += $(CXXFLAGS) $(ALL_CPPFLAGS) -fopenmp -O3 -std=c++14 -mtune=native -march=native -Wall -Wsign-compare -Wno-unused-parameter -Wno-unused-function -Wno-unused-variable -pipe -flto
  ALL_RESFLAGS += $(RESFLAGS) $(DEFINES) $(INCLUDES)
  LIBS += bin/liblibgkit3GL.a -lGLEW -lSDL2 -lGL
  LDDEPS += bin/liblibgkit3GL.a
  ALL_LDFLAGS += $(LDFLAGS) -s -flto=auto
  LINKCMD = $(CXX) -o "$@" $(OBJECTS) $(RESOURCES) $(ALL_LDFLAGS) $(LIBS)
  define PREBUILDCMDS
  endef
  define PRELINKCMDS
  endef
  define POSTBUILDCMDS
  endef
all: prebuild prelink $(TARGET)
	@:

endif

OBJECTS := \
	$(OBJDIR)/test_implicite_surface.o \

RESOURCES := \

CUSTOMFILES := \

SHELLTYPE := posix
ifeq (.exe,$(findstring .exe,$(ComSpec)))
	SHELLTYPE := msdos
endif

$(TARGET): $(GCH) ${CUSTOMFILES} $(OBJECTS) $(LDDEPS) $(RESOURCES) | $(TARGETDIR)
	@echo Linking test_implicite_surface
	$(SILENT) $(LINKCMD)
	$(POSTBUILDCMDS)

$(CUSTOMFILES): | $(OBJDIR)

$(TARGETDIR):
	@echo Creating $(TARGETDIR)
ifeq (posix,$(SHELLTYPE))
	$(SILENT) mkdir -p $(TARGETDIR)
else
	$(SILENT) mkdir $(subst /,\\,$(TARGETDIR))
endif

$(OBJDIR):
	@echo Creating $(OBJDIR)
ifeq (posix,$(SHELLTYPE))
	$(SILENT) mkdir -p $(OBJDIR)
else
	$(SILENT) mkdir $(subst /,\\,$(OBJDIR))
endif

clean:
	@echo Cleaning test_implicite_surface
ifeq (posix,$(SHELLTYPE))
	$(SILENT) rm -f  $(TARGET)
	$(SILENT) rm -rf $(OBJDIR)
else
	$(SILENT) if exist $(subst /,\\,$(TARGET)) del $(subst /,\\,$(TARGET))
	$(SILENT) if exist $(subst /,\\,$(OBJDIR)) rmdir /s /q $(subst /,\\,$(OBJDIR))
endif

prebuild:
	$(PREBUILDCMDS)

prelink:
	$(PRELINKCMDS)

ifneq (,$(PCH))
$(OBJECTS): $(GCH) $(PCH) | $(OBJDIR)
$(GCH): $(PCH) | $(OBJDIR)
	@echo $(notdir $<)
	$(SILENT) $(CXX) -x c++-header $(ALL_CXXFLAGS) -o "$@" -MF "$(@:%.gch=%.d)" -c "$<"
else
$(OBJECTS): | $(OBJDIR)
endif

$(OBJDIR)/test_implicite_surface.o: projets/test_implicite_surface.cpp
	@echo $(notdir $<)
	$(SILENT) $(CXX) $(ALL_CXXFLAGS) $(FORCE_INCLUDE) -o "$@" -MF "$(@:%.o=%.d)" -c "$<"

-include $(OBJECTS:%.o=%.d)
ifneq (,$(PCH))
  -include $(OBJDIR)/$(notdir $(PCH)).d
endif