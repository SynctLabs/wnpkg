CC = gcc

FLAGS = -Iinclude/

BUILD_DIR             = build
WNPKG_EXECUTABLE      = wnpkg
WNPKG_EXECUTABLE_DIR  = $(BUILD_DIR)/$(WNPKG_EXECUTABLE)

# For termux
WNPKG_TEMP            = $(HOME)/temp/c/wnpkg
WNPKG_BUILD_DIR       = wnpkg-build

MAIN_SRC = src/main.c
FILE_SRC = src/file.c

FILE_O   = $(BUILD_DIR)/file.o

all: setup $(WNPKG_EXECUTABLE_DIR)

.PHONY: setup clean

$(FILE_O): $(FILE_SRC)
	$(CC) -c $(FILE_SRC) -o $(FILE_O) $(FLAGS)

$(WNPKG_EXECUTABLE_DIR): $(MAIN_SRC) $(FILE_O)
	$(CC) $(MAIN_SRC) $(FILE_O) -o $(WNPKG_EXECUTABLE_DIR) $(FLAGS)

setup:
	mkdir -p build

clean:
	rm -rf $(WNPKG_EXECUTABLE_DIR)

run: $(WNPKG_EXECUTABLE_DIR)
	rm -rf $(WNPKG_BUILD_DIR)
	chmod +x $(WNPKG_EXECUTABLE_DIR)
	./$(WNPKG_EXECUTABLE_DIR) basic_nodejs_proj
	cd $(WNPKG_BUILD_DIR) && chmod +x basic_nodejs_proj && ./basic_nodejs_proj

termux_run: $(WNPKG_EXECUTABLE_DIR)
	rm -rf $(WNPKG_BUILD_DIR)
	mkdir -p $(WNPKG_TEMP)
	cp $(WNPKG_EXECUTABLE_DIR) $(WNPKG_TEMP)/$(WNPKG_EXECUTABLE)
	chmod +x $(WNPKG_TEMP)/$(WNPKG_EXECUTABLE)
	$(WNPKG_TEMP)/$(WNPKG_EXECUTABLE) basic_nodejs_proj
	rm -rf $(WNPKG_TEMP)/$(WNPKG_BUILD_DIR)
	cp -rf $(WNPKG_BUILD_DIR) $(WNPKG_TEMP)/$(WNPKG_BUILD_DIR)
	cd $(WNPKG_TEMP)/$(WNPKG_BUILD_DIR) && chmod +x basic_nodejs_proj && ./basic_nodejs_proj