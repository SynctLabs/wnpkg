CC = gcc

FLAGS = -Iinclude/ -fsanitize=address

BUILD_DIR             = build
WNPKG_EXECUTABLE      = wnpkg
WNPKG_EXECUTABLE_DIR  = $(BUILD_DIR)/$(WNPKG_EXECUTABLE)

# For termux
WNPKG_TEMP            = $(HOME)/temp/c/wnpkg
WNPKG_BUILD_DIR       = wnpkg-build

MAIN_SRC    = src/main.c
FILE_SRC    = src/file.c
BUILDER_SRC = src/builder.c

FILE_O    = $(BUILD_DIR)/file.o
BUILDER_O = $(BUILD_DIR)/builder.o

OBJS     = $(FILE_O) $(NODE_O) $(BUILDER_O)

all: setup $(WNPKG_EXECUTABLE_DIR)

.PHONY: setup clean

$(FILE_O): $(FILE_SRC)
	$(CC) -c $(FILE_SRC) -o $(FILE_O) $(FLAGS)

$(BUILDER_O): $(BUILDER_SRC)
	$(CC) -c $(BUILDER_SRC) -o $(BUILDER_O) $(FLAGS)

$(WNPKG_EXECUTABLE_DIR): setup $(MAIN_SRC) $(OBJS)
	$(CC) $(MAIN_SRC) $(OBJS) -o $(WNPKG_EXECUTABLE_DIR) $(FLAGS)

setup:
	mkdir -p build

clean:
	rm -rf $(WNPKG_EXECUTABLE_DIR)

runtest:
	cd $(WNPKG_BUILD_DIR) && chmod +x basic_nodejs_proj && ./basic_nodejs_proj

run: $(WNPKG_EXECUTABLE_DIR)
	rm -rf $(WNPKG_BUILD_DIR)
	chmod +x $(WNPKG_EXECUTABLE_DIR)
	./$(WNPKG_EXECUTABLE_DIR) basic_nodejs_proj
	$(MAKE) runtest

termux_runtest:
	rm -rf $(WNPKG_TEMP)/$(WNPKG_BUILD_DIR)
	cp -rf $(WNPKG_BUILD_DIR) $(WNPKG_TEMP)/$(WNPKG_BUILD_DIR)
	cd $(WNPKG_TEMP)/$(WNPKG_BUILD_DIR) && chmod +x basic_nodejs_proj && ./basic_nodejs_proj

termux_run: $(WNPKG_EXECUTABLE_DIR)
	rm -rf $(WNPKG_BUILD_DIR)
	mkdir -p $(WNPKG_TEMP)
	cp $(WNPKG_EXECUTABLE_DIR) $(WNPKG_TEMP)/$(WNPKG_EXECUTABLE)
	chmod +x $(WNPKG_TEMP)/$(WNPKG_EXECUTABLE)
	$(WNPKG_TEMP)/$(WNPKG_EXECUTABLE) basic_nodejs_proj
	$(MAKE) termux_runtest