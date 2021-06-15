CC      = gcc
SRC_DIR = ./src
INC_DIR = ./include
BLD_DIR = ./build
BIN_DIR = ./bin
LIB_DIR = ./lib

MKDIRS = mkfs.tfs mount.tfs
GFLAGS = -O0 -Wall -I$(LOC_DIR)/include/ -I/usr/include/fuse -D_FILE_OFFSET_BITS=64 -lfuse -pthread 
LFLAGS = 

GOUT_DIR = $(PWD)/$(BIN_DIR)
GINC_DIR = $(PWD)/$(INC_DIR)
GLIB_DIR = $(PWD)/$(LIB_DIR)
GBLD_DIR = $(PWD)/$(BLD_DIR)

all: $(MKDIRS)

clean: 
	rm   -rf $(BIN_DIR)
	rm   -rf $(BLD_DIR)
	rm   -rf $(LIB_DIR)
	
$(MKDIRS) : 
	cd $(SRC_DIR)/$@ && make             \
	"GINC_DIR"=$(GINC_DIR)               \
	"GBLD_DIR"=$(GBLD_DIR)               \
	"GOUT_DIR"=$(GOUT_DIR)               \
	"GLIB_DIR"=$(GLIB_DIR)               \
	"GFLAGS"="$(GFLAGS)"                 \
	"LFLAGS"="$(LFLAGS)"                 \
	"CC"=$(CC)                           \
	"TARGET"=$@
