#ifndef INTERFAZ_HPP
#define INTERFAZ_HPP

#include "../tienda/Tienda.hpp"

/* ========================================================================
    MODULO: INTERFAZ DE USUARIO
    Descripción: Encapsula la navegación, los menús y la gestión de la
                 entrada del usuario. Actúa como el cerebro del flujo.
   ======================================================================== */

class Interfaz {
private:
    /* ---- Métodos Privados de Navegación (Submenús) ---- */
    // Encapsulamiento: Solo la propia clase Interfaz maneja su flujo interno.
    static void menuProductos(Tienda& tienda);
    static void menuProveedores(Tienda& tienda);
    static void menuClientes(Tienda& tienda);
    static void menuTransacciones(Tienda& tienda);
    static void menuMantenimiento(Tienda& tienda);

    /* ---- Auxiliares de Presentación ---- */
    static void mostrarEncabezado(const char* titulo);

public:
    /**
     * ejecutar
     * Método principal que lanza el bucle del programa.
     * Recibe la referencia de la Tienda para que todos los módulos
     * compartan los mismos contadores y datos.
     */
    static void ejecutar(Tienda& tienda);
};

#endif // INTERFAZ_HPP