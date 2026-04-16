#ifndef INTERFAZ_HPP
#define INTERFAZ_HPP

#include "../tienda/Tienda.hpp"

class Interfaz {
private:
    // Submenús encapsulados (privados porque solo Interfaz los llama)
    static void menuProductos(Tienda& tienda);
    static void menuProveedores(Tienda& tienda);
    static void menuClientes(Tienda& tienda);
    static void menuTransacciones(Tienda& tienda);
    static void menuMantenimiento(Tienda& tienda);

public:
    // Punto de entrada principal que será llamado desde main.cpp
    static void ejecutar();
};

#endif // INTERFAZ_HPP