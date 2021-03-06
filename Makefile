include build/common_head.mak

CROSS_COMPILE?=
ifneq (,$(CROSS_COMPILE))
CXX:=$(CROSS_COMPILE)g++
endif

EXT?=.exe
BIN_HOST?=bin_host
TARGETS+=$(BIN_HOST)

all: true_all

PKGS:=\
	sdl2\

PKG_CFLAGS:=$(shell pkg-config $(PKGS) --cflags)
PKG_LDFLAGS:=$(shell pkg-config $(PKGS) --libs || echo "ERROR")

ifeq (ERROR,$(PKG_LDFLAGS))
  $(error At least one library was not found in the build environment)
endif

DBGFLAGS?=-g

CXXFLAGS+=-Wall -Wextra
CXXFLAGS+=-Isrc
CXXFLAGS+=-I.
CXXFLAGS+=-Iengine/include
CXXFLAGS+=-std=c++14
CXXFLAGS+=$(PKG_CFLAGS)
LDFLAGS+=$(PKG_LDFLAGS)

CXXFLAGS+=-O3

CXXFLAGS+=$(DBGFLAGS)
LDFLAGS+=$(DBGFLAGS)

#------------------------------------------------------------------------------

ENGINE_ROOT:=engine
include $(ENGINE_ROOT)/project.mk

#------------------------------------------------------------------------------

SRCS_GAME:=\
	src/entities/all.cpp\
	src/entities/amulet.cpp\
	src/entities/bonus.cpp\
	src/entities/door.cpp\
	src/entities/editor.cpp\
	src/entities/explosion.cpp\
	src/entities/hero.cpp\
	src/entities/move.cpp\
	src/entities/moving_platform.cpp\
	src/entities/finish.cpp\
	src/entities/switch.cpp\
	src/entity_factory.cpp\
	src/game.cpp\
	src/state_ending.cpp\
	src/state_playing.cpp\
	src/state_splash.cpp\
	src/convex.cpp\
	src/room_loader.cpp\
	src/physics.cpp\
	src/resources.cpp\

#------------------------------------------------------------------------------

SRCS:=\
	$(SRCS_GAME)\
	$(SRCS_ENGINE)\

$(BIN)/rel/game$(EXT): $(SRCS:%=$(BIN)/%.o)
	@mkdir -p $(dir $@)
	$(CXX) $^ -o '$@' $(LDFLAGS)

TARGETS+=$(BIN)/rel/game$(EXT)

#------------------------------------------------------------------------------
include assets/project.mk

#------------------------------------------------------------------------------

SRCS_TESTS:=\
	$(SRCS_GAME)\
	$(filter-out $(ENGINE_ROOT)/src/main.cpp, $(SRCS_ENGINE))\
	engine/tests/tests.cpp\
	engine/tests/tests_main.cpp\
	engine/tests/audio.cpp\
	engine/tests/base64.cpp\
	engine/tests/decompress.cpp\
	engine/tests/json.cpp\
	engine/tests/util.cpp\
	engine/tests/png.cpp\
	tests/entities.cpp\
	tests/physics.cpp\
	tests/trace.cpp\

$(BIN)/tests$(EXT): $(SRCS_TESTS:%=$(BIN)/%.o)
	@mkdir -p $(dir $@)
	$(CXX) $^ -o '$@' $(LDFLAGS)

TARGETS+=$(BIN)/tests$(EXT)

include build/common.mak
