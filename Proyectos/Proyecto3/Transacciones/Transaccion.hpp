#ifndef TRANSACCION_HPP
#define TRANSACCION_HPP

#include "../Persistencia/Constantes.hpp"
#include <ctime>

/* ========================================================================
    ENTIDAD: TRANSACCION
    Responsabilidad: Registrar los movimientos de inventario (Ventas/Compras),
    vinculando productos con clientes o proveedores.
   ======================================================================== */
class Transaccion {
private:
    int id;
    int idProducto;
    int idEntidadRelacionada; // ID del Cliente (Venta) o Proveedor (Compra)
    int cantidad;
    float precioUnitario;
    float montoTotal;
    time_t fecha;
    char tipo;                // 'V' para Venta, 'C' para Compra
    bool eliminado;

public:
    // Constructores (Punto 9.1)
    Transaccion();
    Transaccion(int id, int idProd, int idEnt, int cant, float precio, char tipo);

    // Getters constantes (Punto 9.3)
    int getId() const;
    int getIdProducto() const;
    int getIdEntidadRelacionada() const;
    int getCantidad() const;
    float getPrecioUnitario() const;
    float getMontoTotal() const;
    time_t getFecha() const;
    char getTipo() const;
    bool isEliminado() const;

    // Setters con logica de calculo
    void setId(int nuevoId);
    void setIdProducto(int idProd);
    void setIdEntidadRelacionada(int idEnt);
    void setCantidad(int cant);
    void setPrecioUnitario(float precio);
    void setTipo(char t);
    void setEliminado(bool estado);

    // Metodos de utilidad
    void calcularMontoTotal();
    void mostrarResumen() const;
    
    static size_t obtenerTamano();
};

#endif