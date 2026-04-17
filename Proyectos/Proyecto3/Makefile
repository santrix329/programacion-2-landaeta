CXX = g++
CXXFLAGS = -Wall -std=c++11 -I.
TARGET = SistemaPOS.exe

SRCS = main.cpp \
       Tienda/Tienda.cpp \
       Persistencia/GestorArchivos.cpp \
       Productos/Producto.cpp \
       Productos/operacionesProductos.cpp \
       Proveedores/Proveedores.cpp \
       Proveedores/operacionesProveedores.cpp \
       Transacciones/Transaccion.cpp \
       Transacciones/operacionesTransacciones.cpp \
       Utilidades/Formatos.cpp \
       Utilidades/Validaciones.cpp

OBJS = $(SRCS:.cpp=.o)

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJS)

clean:
	@del /f /q $(subst /,\,$(OBJS)) $(TARGET)

run: $(TARGET)
	./$(TARGET)