# -------- Generated by configure -----------

CXX := g++
CXXFLAGS :=    -g
LD := g++
LIBS +=  -L/usr/local/lib -Wl,-rpath,/usr/local/lib -lSDL -lpthread -lz
RANLIB := ranlib
INSTALL := install
AR := ar cru
MKDIR := mkdir -p
ECHO := printf
CAT := cat
RM := rm -f
RM_REC := rm -f -r
ZIP := zip -q
CP := cp
WIN32PATH=
STRIP := strip
WINDRES := windres

MODULES +=  src/unix src/cheat
MODULE_DIRS += 
EXEEXT := 

PREFIX := /media/cryptofs/apps/usr/palm/applications/de.daemon.stella
BINDIR := /media/cryptofs/apps/usr/palm/applications/de.daemon.stella/bin
DOCDIR := /media/cryptofs/apps/usr/palm/applications/de.daemon.stella/share/doc/stella
DATADIR := /media/cryptofs/apps/usr/palm/applications/de.daemon.stella/share
PROFILE := 

HAVE_GCC3 = 1

INCLUDES += -Isrc/emucore -Isrc/emucore/m6502/src -Isrc/emucore/m6502/src/bspf/src -Isrc/common -Isrc/gui -I/usr/local/include/SDL -D_GNU_SOURCE=1 -D_REENTRANT -Isrc/unix -Isrc/cheat
OBJS += 
DEFINES +=  -DUNIX -DBSPF_UNIX -DHAVE_GETTIMEOFDAY -DHAVE_INTTYPES -DWINDOWED_SUPPORT -DSOUND_SUPPORT -DSNAPSHOT_SUPPORT -DCHEATCODE_SUPPORT
LDFLAGS += -L/usr/local/lib -Wl,-rpath-link,/usr/local/lib -Wl,-rpath,/usr/local/lib
