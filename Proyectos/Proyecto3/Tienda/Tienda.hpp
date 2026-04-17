#ifndef TIENDA_HPP
#define TIENDA_HPP

#include "../Persistencia/Constantes.hpp"
#include <cstring>

/* ========================================================================
    ENTIDAD: TIENDA
    Responsabilidad: Administrar el estado global del sistema, incluyendo
    contadores de IDs para garantizar la integridad referencial.
   ======================================================================== */
class Tienda {
private:
    char nombre[Constantes::TAM_NOMBRE];
    int contadorClientes;
    int contadorProductos;
    int contadorProveedores;
    int contadorTransacciones;

public:
    // Constructor (Punto 9.1)
    Tienda();

    // Getters constantes (Punto 9.3)
    const char* getNombre() const;
    int getContadorClientes() const;
    int getContadorProductos() const;
    int getContadorProveedores() const;
    int getContadorTransacciones() const;

    // Setters con validacion
    void setNombre(const char* nuevoNombre);

    /*
     * Generadores de IDs (Logica de Negocio)
     * Estos metodos incrementan el contador y retornan el nuevo valor.
     */
    int generarIdCliente();
    int generarIdProducto();
    int generarIdProveedor();
    int generarIdTransaccion();

    // Metodos para persistencia del estado de la tienda
    static size_t obtenerTamano();
};

#endif