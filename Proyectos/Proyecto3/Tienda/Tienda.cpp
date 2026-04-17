#include "Tienda.hpp"
#include "../Persistencia/Constantes.hpp"
#include <cstring>

/*
 * Tienda
 * Constructor por defecto. Inicializa el nombre de la tienda y pone todos
 * los contadores de IDs en cero para un sistema que inicia desde vacio.
 */
Tienda::Tienda() {
    strncpy(nombre, "Sistema de Gestion POS", Constantes::TAM_NOMBRE - 1);
    nombre[Constantes::TAM_NOMBRE - 1] = '\0';
    contadorClientes = 0;
    contadorProductos = 0;
    contadorProveedores = 0;
    contadorTransacciones = 0;
}

/*
 * getNombre
 * Retorna un puntero constante al nombre de la tienda.
 */
const char* Tienda::getNombre() const {
    return nombre;
}

/*
 * setNombre
 * Permite modificar el nombre del establecimiento validando que la 
 * cadena no este vacia.
 */
void Tienda::setNombre(const char* nuevoNombre) {
    if (nuevoNombre != nullptr && strlen(nuevoNombre) > 0) {
        strncpy(nombre, nuevoNombre, Constantes::TAM_NOMBRE - 1);
        nombre[Constantes::TAM_NOMBRE - 1] = '\0';
    }
}

/*
 * generarIdCliente
 * Incrementa el contador de clientes y retorna el nuevo ID unico 
 * para ser asignado a un registro.
 */
int Tienda::generarIdCliente() {
    return ++contadorClientes;
}

/*
 * generarIdProducto
 * Incrementa el contador de productos y retorna el nuevo ID unico.
 */
int Tienda::generarIdProducto() {
    return ++contadorProductos;
}

/*
 * generarIdProveedor
 * Incrementa el contador de proveedores y retorna el nuevo ID unico.
 */
int Tienda::generarIdProveedor() {
    return ++contadorProveedores;
}

/*
 * generarIdTransaccion
 * Incrementa el contador de transacciones (ventas/compras) y retorna el ID.
 */
int Tienda::generarIdTransaccion() {
    return ++contadorTransacciones;
}

/*
 * getContadorClientes
 * Retorna el valor actual del contador de clientes (util para reportes).
 */
int Tienda::getContadorClientes() const {
    return contadorClientes;
}

/*
 * getContadorProductos
 * Retorna el valor actual del contador de productos.
 */
int Tienda::getContadorProductos() const {
    return contadorProductos;
}

/*
 * getContadorProveedores
 * Retorna el valor actual del contador de proveedores.
 */
int Tienda::getContadorProveedores() const {
    return contadorProveedores;
}

/*
 * getContadorTransacciones
 * Retorna el valor actual del contador de transacciones.
 */
int Tienda::getContadorTransacciones() const {
    return contadorTransacciones;
}

/*
 * obtenerTamano
 * Retorna el tamaño en bytes de la clase Tienda para facilitar la 
 * lectura/escritura del objeto de configuracion en archivos binarios.
 */
size_t Tienda::obtenerTamano() {
    return sizeof(Tienda);
}