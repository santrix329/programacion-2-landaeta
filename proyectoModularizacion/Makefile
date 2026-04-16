# --- CONFIGURACIÓN DEL COMPILADOR ---
CXX = g++
CXXFLAGS = -Wall -std=c++11 -Iinclude

# --- NOMBRES Y RUTAS ---
BIN_NAME = rescate_monolito
SRC_DIR = src
INC_DIR = include
OBJ_DIR = obj

# --- DETECCIÓN AUTOMÁTICA DE ARCHIVOS ---
# Esto buscará: academicos.cpp, datos.cpp, log.cpp, reportes.cpp, 
# utilidades.cpp, validacion.cpp y cualquier otro dentro de src/
SOURCES = main.cpp $(wildcard $(SRC_DIR)/*.cpp)

# Transforma la lista de .cpp en una lista de .o dentro de la carpeta obj/
OBJECTS = $(SOURCES:%.cpp=$(OBJ_DIR)/%.o)

# --- REGLA PRINCIPAL ---
all: $(OBJ_DIR) $(BIN_NAME)
	@echo "\n[AUDITOR] Proyecto compilado con exito: ./$(BIN_NAME)"

# --- VINCULACIÓN FINAL (Linker) ---
$(BIN_NAME): $(OBJECTS)
	$(CXX) $(CXXFLAGS) $(OBJECTS) -o $(BIN_NAME)

# --- COMPILACIÓN DE MÓDULOS (.cpp -> .o) ---
$(OBJ_DIR)/$(SRC_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(@D)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# --- COMPILACIÓN DEL MAIN ---
$(OBJ_DIR)/main.o: main.cpp
	@mkdir -p $(@D)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# --- UTILITARIOS ---
$(OBJ_DIR):
	@mkdir -p $(OBJ_DIR)

clean:
	rm -rf $(OBJ_DIR) $(BIN_NAME)
	@echo "[AUDITOR] Limpieza completada. Archivos objeto y ejecutable eliminados."

run: all
	./$(BIN_NAME)

.PHONY: all clean run