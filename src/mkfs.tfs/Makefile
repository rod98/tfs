FLAGS   = -I$(GINC_DIR) $(GFLAGS)
LDFLAGS = $(LFLAGS)
EXT     = .c

SRC_DIR = ./src

BLD_DIR = $(GBLD_DIR)/$(TARGET)
BIN_DIR = $(GOUT_DIR)
LIB_DIR = $(GLIB_DIR)

SOURCES = $(wildcard $(SRC_DIR)/*$(EXT))
OBJECTS = $(patsubst $(SRC_DIR)/%$(EXT),$(BLD_DIR)/%.o,$(SOURCES))

all: dir $(BIN_DIR)/$(TARGET)

dir:
	mkdir -p $(BLD_DIR)
	mkdir -p $(BIN_DIR)
	mkdir -p $(LIB_DIR)

$(BIN_DIR)/$(TARGET): $(OBJECTS) $(wildcard $(BLD_DIR)/*.o)
	$(CC) $(FLAGS) $^ $(LDFLAGS) -o $@

$(OBJECTS): $(BLD_DIR)/%.o : $(SRC_DIR)/%$(EXT)
	$(CC) $(FLAGS) -c $< -o $@

clean:
	rm -f $(BLD_DIR)/*.o $(BIN_DIR)/$(TARGET)
