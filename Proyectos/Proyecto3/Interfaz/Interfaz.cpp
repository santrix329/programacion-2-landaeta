#include "Interfaz.hpp"
#include <iostream>
#include <iomanip>

/* ========================================================================
    INCLUSIONES DE MÓDULOS DE OPERACIONES
    Cada archivo .cpp se encarga de la lógica de su dominio.
   ======================================================================== */
#include "../productos/operacionesProductos.hpp"
#include "../proveedores/operacionesProveedores.hpp"
#include "../clientes/operacionesClientes.hpp"
#include "../transacciones/operacionesTransacciones.hpp"

/* UTILIDADES Y PERSISTENCIA */
#include "../utilidades/Formatos.hpp" 
#include "../persistencia/GestorArchivos.hpp"
#include "../tienda/Constantes.hpp"

using namespace std;
using namespace Constantes; // Uso de constantes globales para colores y rutas

/* ========================================================================
    MÉTODOS DE APOYO VISUAL (Privados)
   ======================================================================== */

void Interfaz::mostrarEncabezado(const char* titulo) {
    Formatos::limpiarPantalla();
    cout << COLOR_CIAN << COLOR_NEG;
    cout << "  ==============================================================" << endl;
    cout << "  " << setfill(' ') << setw(30 + strlen(titulo)/2) << titulo << endl;
    cout << "  ==============================================================" << endl;
    cout << COLOR_RST << endl;
}

/* ========================================================================
    FLUJO PRINCIPAL DEL SISTEMA
   ======================================================================== */

void Interfaz::ejecutar(Tienda& tienda) {
    char opcionPrincipal = ' ';
    
    do {
        mostrarEncabezado("SISTEMA DE GESTION INTEGRAL v3.0");
        
        cout << "  " << COLOR_AMARI << "DATOS DE LA TIENDA:" << COLOR_RST << endl;
        cout << "  Nombre: " << tienda.getNombre() << endl;
        cout << "  Ubicacion: " << tienda.getDireccion() << endl;
        cout << "  --------------------------------------------------------------" << endl;

        cout << "  " << COLOR_CIAN << "1." << COLOR_RST << " Gestion de Inventario (Productos)" << endl;
        cout << "  " << COLOR_CIAN << "2." << COLOR_RST << " Gestion de Proveedores"           << endl;
        cout << "  " << COLOR_CIAN << "3." << COLOR_RST << " Cartera de Clientes"              << endl;
        cout << "  " << COLOR_CIAN << "4." << COLOR_RST << " Ventas y Movimientos"              << endl;
        cout << "  " << COLOR_CIAN << "5." << COLOR_RST << " Mantenimiento y Reportes"          << endl;
        cout << "  " << COLOR_ROJO << "6." << COLOR_RST << " Cerrar Sistema y Guardar"         << endl;

        cout << endl << COLOR_NEG << "  Seleccione una categoria: " << COLOR_RST;
        cin >> opcionPrincipal;
        Formatos::limpiarBuffer();

        switch (opcionPrincipal) {
            case '1': menuProductos(tienda); break;
            case '2': menuProveedores(tienda); break;
            case '3': menuClientes(tienda); break;
            case '4': menuTransacciones(tienda); break;
            case '5': menuMantenimiento(tienda); break;
            case '6':
                // Sincronizacion final con el archivo tienda.bin
                cout << endl << COLOR_AMARI << "  [>] Guardando estado maestro..." << COLOR_RST << endl;
                if (GestorArchivos::actualizarRegistro<Tienda>(ARCHIVO_TIENDA, 0, tienda)) {
                    cout << COLOR_VERDE << "  [OK] Sesion cerrada con exito." << COLOR_RST << endl;
                } else {
                    cout << COLOR_ROJO << "  [!] Error al guardar metadatos." << COLOR_RST << endl;
                }
                break;
            default:
                cout << endl << COLOR_ROJO << "  [!] Opcion invalida. Reintente." << COLOR_RST << endl;
                Formatos::pausar();
        }
    } while (opcionPrincipal != '6');
}

/* ========================================================================
    SUBMENÚS POR DOMINIO (Delegación de Responsabilidades)
   ======================================================================== */

void Interfaz::menuProductos(Tienda& tienda) {
    int op = -1;
    do {
        mostrarEncabezado("MODULO: GESTION DE PRODUCTOS");
        cout << "  1. Registrar entrada de producto" << endl;
        cout << "  2. Buscar por ID especifico"       << endl;
        cout << "  3. Actualizar precios o stock"     << endl;
        cout << "  4. Dar de baja (Eliminacion logica)" << endl;
        cout << "  5. Ver inventario general"         << endl;
        cout << "  0. Volver al menu anterior"        << endl;

        op = Formatos::leerEntero("\n  Seleccione operacion: ", 0, 5);

        switch (op) {
            case 1: registrarProducto(tienda); break;
            case 2: buscarProducto(tienda); break;
            case 3: actualizarProducto(tienda); break;
            case 4: eliminarProducto(tienda); break;
            case 5: listarProductos(tienda); break;
        }
        if (op != 0) Formatos::pausar();
    } while (op != 0);
}

void Interfaz::menuProveedores(Tienda& tienda) {
    int op = -1;
    do {
        mostrarEncabezado("MODULO: GESTION DE PROVEEDORES");
        cout << "  1. Registrar nuevo proveedor" << endl;
        cout << "  2. Buscar por ID"             << endl;
        cout << "  3. Modificar informacion"     << endl;
        cout << "  4. Eliminar proveedor"        << endl;
        cout << "  5. Listado de contactos"      << endl;
        cout << "  0. Volver al menu anterior"   << endl;

        op = Formatos::leerEntero("\n  Seleccione operacion: ", 0, 5);

        switch (op) {
            case 1: crearProveedor(tienda); break;
            case 2: buscarProveedor(tienda); break;
            case 3: actualizarProveedor(tienda); break;
            case 4: eliminarProveedor(tienda); break;
            case 5: listarProveedores(tienda); break;
        }
        if (op != 0) Formatos::pausar();
    } while (op != 0);
}

void Interfaz::menuClientes(Tienda& tienda) {
    int op = -1;
    do {
        mostrarEncabezado("MODULO: CARTERA DE CLIENTES");
        cout << "  1. Afiliar nuevo cliente"     << endl;
        cout << "  2. Consultar por ID"          << endl;
        cout << "  3. Editar perfil de cliente"  << endl;
        cout << "  4. Eliminar del sistema"      << endl;
        cout << "  5. Ver base de datos"         << endl;
        cout << "  0. Volver al menu anterior"   << endl;

        op = Formatos::leerEntero("\n  Seleccione operacion: ", 0, 5);

        switch (op) {
            case 1: crearCliente(tienda); break;
            case 2: buscarCliente(tienda); break;
            case 3: actualizarCliente(tienda); break;
            case 4: eliminarCliente(tienda); break;
            case 5: listarClientes(tienda); break;
        }
        if (op != 0) Formatos::pausar();
    } while (op != 0);
}

void Interfaz::menuTransacciones(Tienda& tienda) {
    int op = -1;
    do {
        mostrarEncabezado("MODULO: MOVIMIENTOS Y VENTAS");
        cout << "  1. Nueva Venta a Cliente"      << endl;
        cout << "  2. Nueva Compra a Proveedor"   << endl;
        cout << "  3. Ver historial de ventas"    << endl;
        cout << "  4. Ver historial de compras"   << endl;
        cout << "  0. Volver al menu anterior"    << endl;

        op = Formatos::leerEntero("\n  Seleccione operacion: ", 0, 4);

        switch (op) {
            case 1: registrarVenta(tienda); break;
            case 2: registrarCompra(tienda); break;
            case 3: listarTransaccionesVenta(tienda); break;
            case 4: listarTransaccionesCompra(tienda); break;
        }
        if (op != 0) Formatos::pausar();
    } while (op != 0);
}

void Interfaz::menuMantenimiento(Tienda& tienda) {
    int op = -1;
    do {
        mostrarEncabezado("MODULO: REPORTES Y SISTEMA");
        cout << "  1. Reporte de Stock Critico"     << endl;
        cout << "  2. Balance de Transacciones"      << endl;
        cout << "  3. Verificar Archivos Binarios"   << endl;
        cout << "  0. Volver al menu anterior"       << endl;

        op = Formatos::leerEntero("\n  Seleccione reporte: ", 0, 3);

        switch (op) {
            case 1: productosStockCritico(tienda); break;
            case 2: balanceGeneral(tienda); break;
            case 3: GestorArchivos::inicializarSistemaArchivos(); break;
        }
        if (op != 0) Formatos::pausar();
    } while (op != 0);
}