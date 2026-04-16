#include "Interfaz.hpp"
#include <iostream>
#include <cstdlib> // Para system("cls") o system("clear")

// Inclusiones de los módulos de operaciones (Lógica de negocio)
#include "../productos/operacionesProductos.hpp"
#include "../proveedores/operacionesProveedores.hpp"
#include "../clientes/operacionesClientes.hpp"
#include "../transacciones/operacionesTransacciones.hpp"
// Asumimos un archivo para operaciones de mantenimiento basado en tu main original
#include "../persistencia/operacionesMantenimiento.hpp" 

// Utilidades para lectura de datos y formato
#include "../utilidades/Formatos.hpp" 

using namespace std;

// --- Colores para la consola (ANSI escape codes) ---
#define RST    "\033[0m"    
#define NEG    "\033[1m"    
#define ROJO   "\033[31m"   
#define VERDE  "\033[32m"   
#define AMARI  "\033[33m"   
#define AZUL   "\033[34m"   
#define MAGEN  "\033[35m"   
#define CIAN   "\033[36m"   

void Interfaz::ejecutar() {
    Tienda miTienda; // Instancia principal que se pasará a las operaciones
    char opcionPrincipal = ' ';

    do {
        system("cls"); // O system("clear") en Linux/Mac

        cout << endl << AZUL << NEG;
        cout << "  ==============================================================" << endl;
        cout << "      SISTEMA DE GESTION DE INVENTARIO  v3.0 (POO)             " << endl;
        cout << "      Tienda: La Bodeguita 2.0                                 " << endl;
        cout << "  ==============================================================" << endl;
        cout << RST << endl;

        cout << "  " << CIAN  << "1." << RST << " Gestion de Productos"     << endl;
        cout << "  " << CIAN  << "2." << RST << " Gestion de Proveedores"   << endl;
        cout << "  " << CIAN  << "3." << RST << " Gestion de Clientes"      << endl;
        cout << "  " << CIAN  << "4." << RST << " Gestion de Transacciones" << endl;
        cout << "  " << MAGEN << "5." << RST << " Mantenimiento y Reportes" << endl;
        cout << "  " << ROJO  << "6." << RST << " Salir del Sistema"        << endl;

        cout << endl << "  Seleccione una opcion: ";
        cin >> opcionPrincipal;
        Formatos::limpiarBuffer();

        switch (opcionPrincipal) {
            case '1': menuProductos(miTienda); break;
            case '2': menuProveedores(miTienda); break;
            case '3': menuClientes(miTienda); break;
            case '4': menuTransacciones(miTienda); break;
            case '5': menuMantenimiento(miTienda); break;
            case '6':
                cout << endl << VERDE << NEG << "  Hasta luego! Gracias por usar el sistema." << RST << endl;
                break;
            default:
                cout << endl << ROJO << "  Opcion no valida. Seleccione del 1 al 6." << RST << endl;
                Formatos::pausar();
                break;
        }

    } while (opcionPrincipal != '6');
}

void Interfaz::menuProductos(Tienda& tienda) {
    int opP = -1;
    do {
        system("cls");
        cout << endl << AZUL << NEG;
        cout << "  =========================================" << endl;
        cout << "      GESTION DE PRODUCTOS               " << endl;
        cout << "  =========================================" << endl;
        cout << RST << endl;

        cout << "  " << CIAN << "1." << RST << " Registrar nuevo producto"   << endl;
        cout << "  " << CIAN << "2." << RST << " Buscar producto"             << endl;
        cout << "  " << CIAN << "3." << RST << " Editar producto"             << endl;
        cout << "  " << CIAN << "4." << RST << " Ajustar stock manualmente"  << endl;
        cout << "  " << CIAN << "5." << RST << " Eliminar producto"           << endl;
        cout << "  " << CIAN << "6." << RST << " Listar todos los productos" << endl;
        cout << "  " << ROJO  << "0." << RST << " Volver al menu principal"   << endl;

        cout << endl;
        opP = Formatos::leerEntero("  Seleccione: ", 0, 6);
        cout << endl;

        // Delegamos la acción al módulo correspondiente
        if      (opP == 1) registrarProducto(tienda);
        else if (opP == 2) buscarProducto(tienda);
        else if (opP == 3) actualizarProducto(tienda);
        else if (opP == 4) ajusteStockProducto(tienda);
        else if (opP == 5) eliminarProducto(tienda);
        else if (opP == 6) listarProductos(tienda);

        if (opP != 0) Formatos::pausar();

    } while (opP != 0);
}

void Interfaz::menuProveedores(Tienda& tienda) {
    int opProv = -1;
    do {
        system("cls");
        cout << endl << AZUL << NEG;
        cout << "  =========================================" << endl;
        cout << "      GESTION DE PROVEEDORES             " << endl;
        cout << "  =========================================" << endl;
        cout << RST << endl;

        cout << "  " << CIAN << "1." << RST << " Registrar nuevo proveedor" << endl;
        cout << "  " << CIAN << "2." << RST << " Buscar proveedor"          << endl;
        cout << "  " << CIAN << "3." << RST << " Actualizar proveedor"      << endl;
        cout << "  " << CIAN << "4." << RST << " Eliminar proveedor"        << endl;
        cout << "  " << CIAN << "5." << RST << " Listar proveedores"        << endl;
        cout << "  " << ROJO  << "0." << RST << " Volver al menu principal"  << endl;

        cout << endl;
        opProv = Formatos::leerEntero("  Seleccione: ", 0, 5);
        cout << endl;

        if      (opProv == 1) crearProveedor(tienda);
        else if (opProv == 2) buscarProveedor(tienda);
        else if (opProv == 3) actualizarProveedor(tienda);
        else if (opProv == 4) eliminarProveedor(tienda);
        else if (opProv == 5) listarProveedores(tienda);

        if (opProv != 0) Formatos::pausar();

    } while (opProv != 0);
}

void Interfaz::menuClientes(Tienda& tienda) {
    int opCli = -1;
    do {
        system("cls");
        cout << endl << AZUL << NEG;
        cout << "  =========================================" << endl;
        cout << "      GESTION DE CLIENTES               " << endl;
        cout << "  =========================================" << endl;
        cout << RST << endl;

        cout << "  " << CIAN << "1." << RST << " Registrar nuevo cliente"   << endl;
        cout << "  " << CIAN << "2." << RST << " Buscar cliente"             << endl;
        cout << "  " << CIAN << "3." << RST << " Actualizar datos"           << endl;
        cout << "  " << CIAN << "4." << RST << " Listar todos los clientes" << endl;
        cout << "  " << CIAN << "5." << RST << " Eliminar cliente"           << endl;
        cout << "  " << ROJO  << "0." << RST << " Volver al menu principal"  << endl;

        cout << endl;
        opCli = Formatos::leerEntero("  Seleccione: ", 0, 5);
        cout << endl;

        if      (opCli == 1) crearCliente(tienda);
        else if (opCli == 2) buscarCliente(tienda);
        else if (opCli == 3) actualizarCliente(tienda);
        else if (opCli == 4) listarClientes(tienda);
        else if (opCli == 5) eliminarCliente(tienda);

        if (opCli != 0) Formatos::pausar();

    } while (opCli != 0);
}

void Interfaz::menuTransacciones(Tienda& tienda) {
    int opT = -1;
    do {
        system("cls");
        cout << endl << AZUL << NEG;
        cout << "  =========================================" << endl;
        cout << "      GESTION DE TRANSACCIONES           " << endl;
        cout << "  =========================================" << endl;
        cout << RST << endl;

        cout << "  " << CIAN  << "1." << RST << " Registrar Compra (a Proveedor)" << endl;
        cout << "  " << CIAN  << "2." << RST << " Registrar Venta (a Cliente)"    << endl;
        cout << "  " << CIAN  << "3." << RST << " Buscar Transacciones"           << endl;
        cout << "  " << CIAN  << "4." << RST << " Listar Transacciones"           << endl;
        cout << "  " << ROJO  << "5." << RST << " Cancelar Transaccion"           << endl;
        cout << "  " << ROJO  << "0." << RST << " Volver al menu principal"       << endl;

        cout << endl;
        opT = Formatos::leerEntero("  Seleccione: ", 0, 5);
        cout << endl;

        if      (opT == 1) registrarCompraBinaria(tienda);
        else if (opT == 2) registrarVentaBinaria(tienda);
        else if (opT == 3) buscarTransacciones(tienda);
        else if (opT == 4) listarTransacciones(tienda);
        else if (opT == 5) cancelarTransaccion(tienda);

        if (opT != 0) Formatos::pausar();

    } while (opT != 0);
}

void Interfaz::menuMantenimiento(Tienda& tienda) {
    int opM = -1;
    do {
        system("cls");
        cout << endl << MAGEN << NEG;
        cout << "  =========================================" << endl;
        cout << "      MANTENIMIENTO Y REPORTES           " << endl;
        cout << "  =========================================" << endl;
        cout << RST << endl;

        cout << "  " << MAGEN << "1." << RST << " Verificar Integridad Referencial" << endl;
        cout << "  " << MAGEN << "2." << RST << " Crear Respaldo (Backup)"          << endl;
        cout << "  " << MAGEN << "3." << RST << " Reporte de Stock Critico"         << endl;
        cout << "  " << MAGEN << "4." << RST << " Historial Detallado de Cliente"   << endl;
        cout << "  " << ROJO   << "0." << RST << " Volver al menu principal"        << endl;

        cout << endl;
        opM = Formatos::leerEntero("  Seleccione: ", 0, 4);
        cout << endl;

        if      (opM == 1) verificarIntegridadReferencial();
        else if (opM == 2) crearBackup();
        else if (opM == 3) reporteStockCritico(tienda);
        else if (opM == 4) historialDetalladoCliente(tienda);

        if (opM != 0) Formatos::pausar();

    } while (opM != 0);
}