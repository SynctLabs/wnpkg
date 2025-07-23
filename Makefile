# Compilador e flags
CC = gcc
FLAGS = -Iinclude/

# Pastas e arquivos
BUILD_DIR             = build
WNPKG_EXECUTABLE      = wnpkg
WNPKG_EXECUTABLE_DIR  = $(BUILD_DIR)/$(WNPKG_EXECUTABLE)
WNPKG_BUILD_DIR       = wnpkg-build
WNPKG_TEMP            = $(HOME)/temp/c/wnpkg

# Códigos fonte
MAIN_SRC = src/main.c
FILE_SRC = src/file.c
FILE_O   = $(BUILD_DIR)/file.o

# Alvo padrão
all: setup $(WNPKG_EXECUTABLE_DIR)

# Configurações gerais
.PHONY: setup clean run termux_run

# Compilar os arquivos
$(FILE_O): $(FILE_SRC)
	$(CC) -c $(FILE_SRC) -o $(FILE_O) $(FLAGS)

$(WNPKG_EXECUTABLE_DIR): $(MAIN_SRC) $(FILE_O)
	$(CC) $(MAIN_SRC) $(FILE_O) -o $(WNPKG_EXECUTABLE_DIR) $(FLAGS)

# Criar pasta de build
setup:
	mkdir $(BUILD_DIR)

# Limpar binário
clean:
	rm -rf $(WNPKG_EXECUTABLE_DIR)

# Rodar normalmente no Linux
run: $(WNPKG_EXECUTABLE_DIR)
	chmod +x $(WNPKG_EXECUTABLE_DIR)
	./$(WNPKG_EXECUTABLE_DIR) basic_nodejs_proj
	cd $(WNPKG_BUILD_DIR) && chmod +x basic_nodejs_proj && ./basic_nodejs_proj

# Rodar no Termux
termux_run: $(WNPKG_EXECUTABLE_DIR)
	mkdir -p $(WNPKG_TEMP)
	cp $(WNPKG_EXECUTABLE_DIR) $(WNPKG_TEMP)/$(WNPKG_EXECUTABLE)
	chmod +x $(WNPKG_TEMP)/$(WNPKG_EXECUTABLE)
	$(WNPKG_TEMP)/$(WNPKG_EXECUTABLE) basic_nodejs_proj
	rm -rf $(WNPKG_TEMP)/$(WNPKG_BUILD_DIR)
	cp -rf $(WNPKG_BUILD_DIR) $(WNPKG_TEMP)/$(WNPKG_BUILD_DIR)
	cd $(WNPKG_TEMP)/$(WNPKG_BUILD_DIR) && chmod +x basic_nodejs_proj && ./basic_nodejs_proj
