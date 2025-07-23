CC = gcc

FLAGS = -Iinclude/ -fsanitize=address

BUILD_DIR             = build
LNPKG_EXECUTABLE      = lnpkg
LNPKG_EXECUTABLE_DIR  = $(BUILD_DIR)/$(LNPKG_EXECUTABLE)

# For termux
LNPKG_TEMP            = $(HOME)/temp/c/lnpkg
LNPKG_BUILD_DIR       = lnpkg-build

MAIN_SRC    = src/main.c
FILE_SRC    = src/file.c
BUILDER_SRC = src/builder.c

FILE_O    = $(BUILD_DIR)/file.o
BUILDER_O = $(BUILD_DIR)/builder.o

OBJS     = $(FILE_O) $(NODE_O) $(BUILDER_O)

all: setup $(LNPKG_EXECUTABLE_DIR)

.PHONY: setup clean

$(FILE_O): $(FILE_SRC)
	$(CC) -c $(FILE_SRC) -o $(FILE_O) $(FLAGS)

$(BUILDER_O): $(BUILDER_SRC)
	$(CC) -c $(BUILDER_SRC) -o $(BUILDER_O) $(FLAGS)

$(LNPKG_EXECUTABLE_DIR): setup $(MAIN_SRC) $(OBJS)
	$(CC) $(MAIN_SRC) $(OBJS) -o $(LNPKG_EXECUTABLE_DIR) $(FLAGS)

setup:
	mkdir -p build

clean:
	rm -rf $(LNPKG_EXECUTABLE_DIR)

runtest:
	cd $(LNPKG_BUILD_DIR) && chmod +x basic_nodejs_proj && ./basic_nodejs_proj

run: $(LNPKG_EXECUTABLE_DIR)
	chmod +x $(LNPKG_EXECUTABLE_DIR)
	./$(LNPKG_EXECUTABLE_DIR) basic_nodejs_proj
	$(MAKE) runtest

termux_runtest:
	rm -rf $(LNPKG_TEMP)/$(LNPKG_BUILD_DIR)
	cp -rf $(LNPKG_BUILD_DIR) $(LNPKG_TEMP)/$(LNPKG_BUILD_DIR)
	cd $(LNPKG_TEMP)/$(LNPKG_BUILD_DIR) && chmod +x basic_nodejs_proj && ./basic_nodejs_proj

termux_run: $(LNPKG_EXECUTABLE_DIR)
	mkdir -p $(LNPKG_TEMP)
	cp $(LNPKG_EXECUTABLE_DIR) $(LNPKG_TEMP)/$(LNPKG_EXECUTABLE)
	chmod +x $(LNPKG_TEMP)/$(LNPKG_EXECUTABLE)
	$(LNPKG_TEMP)/$(LNPKG_EXECUTABLE) basic_nodejs_proj
	$(MAKE) termux_runtest