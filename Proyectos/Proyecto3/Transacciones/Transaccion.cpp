#include "Transaccion.hpp"
#include <iostream>
#include <ctime>

using namespace std;

/*
 * Transaccion
 * Constructor por defecto. Inicializa los montos en cero y captura 
 * la fecha y hora actual del sistema en el momento de la creacion.
 */
Transaccion::Transaccion() {
    id = 0;
    idProducto = 0;
    idEntidadRelacionada = 0;
    cantidad = 0;
    precioUnitario = 0.0f;
    montoTotal = 0.0f;
    fecha = time(nullptr); // Captura fecha/hora actual
    tipo = 'V'; // Por defecto Venta
    eliminado = false;
}

/*
 * Transaccion
 * Constructor parametrizado. Asigna los valores basicos y dispara 
 * automaticamente el calculo del monto total para mantener la integridad.
 */
Transaccion::Transaccion(int id, int idProd, int idEnt, int cant, float precio, char t) : Transaccion() {
    setId(id);
    setIdProducto(idProd);
    setIdEntidadRelacionada(idEnt);
    setCantidad(cant);
    setPrecioUnitario(precio);
    setTipo(t);
    calcularMontoTotal();
}

/*
 * calcularMontoTotal
 * Metodo privado de utilidad que garantiza que el total siempre sea 
 * el producto de la cantidad por el precio, evitando errores manuales.
 */
void Transaccion::calcularMontoTotal() {
    montoTotal = (float)cantidad * precioUnitario;
}

/*
 * getId
 * Retorna el ID de la transaccion.
 */
int Transaccion::getId() const { return id; }

/*
 * getMontoTotal
 * Retorna el resultado del calculo cantidad * precio.
 */
float Transaccion::getMontoTotal() const { return montoTotal; }

/*
 * getFecha
 * Retorna el timestamp de la operacion.
 */
time_t Transaccion::getFecha() const { return fecha; }

/*
 * setCantidad
 * Asigna la cantidad de productos y recalcula el monto total.
 */
void Transaccion::setCantidad(int cant) {
    if (cant >= 0) {
        cantidad = cant;
        calcularMontoTotal();
    }
}

/*
 * setPrecioUnitario
 * Asigna el precio al que se realizo la operacion y recalcula el total.
 */
void Transaccion::setPrecioUnitario(float precio) {
    if (precio >= 0) {
        precioUnitario = precio;
        calcularMontoTotal();
    }
}

/*
 * setTipo
 * Define si es una Venta ('V') o Compra ('C').
 */
void Transaccion::setTipo(char t) {
    if (t == 'V' || t == 'C') {
        tipo = t;
    }
}

/*
 * setId
 * Asigna el identificador unico generado por la clase Tienda.
 */
void Transaccion::setId(int nuevoId) {
    if (nuevoId > 0) id = nuevoId;
}

/*
 * setIdProducto
 * Establece el vinculo con el producto afectado.
 */
void Transaccion::setIdProducto(int idProd) {
    idProducto = idProd;
}

/*
 * setIdEntidadRelacionada
 * Establece el vinculo con el Cliente o Proveedor.
 */
void Transaccion::setIdEntidadRelacionada(int idEnt) {
    idEntidadRelacionada = idEnt;
}

/*
 * setEliminado
 * Permite el borrado logico de la transaccion (ej. anulacion de factura).
 */
void Transaccion::setEliminado(bool estado) {
    eliminado = estado;
}

/*
 * isEliminado
 * Indica si la transaccion fue anulada.
 */
bool Transaccion::isEliminado() const { return eliminado; }

/*
 * obtenerTamano
 * Retorna el tamano fijo en memoria para persistencia binaria.
 */
size_t Transaccion::obtenerTamano() {
    return sizeof(Transaccion);
}

// Getters basicos restantes
int Transaccion::getIdProducto() const { return idProducto; }
int Transaccion::getIdEntidadRelacionada() const { return idEntidadRelacionada; }
int Transaccion::getCantidad() const { return cantidad; }
float Transaccion::getPrecioUnitario() const { return precioUnitario; }
char Transaccion::getTipo() const { return tipo; }