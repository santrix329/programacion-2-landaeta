#include "Producto.hpp" 
#include "operacionesProductos.hpp"
#include "../Persistencia/Constantes.hpp"
#include "../Persistencia/GestorArchivos.hpp"
#include "../Utilidades/Formatos.hpp"

#include <iostream>
#include <iomanip>
#include <cstring>

using namespace std;

/* ========================================================================
    CONFIGURACION DE INTERFAZ LOCAL (Colores ANSI para profesionalismo)
   ======================================================================== */
#define RST     "\033[0m"
#define NEG     "\033[1m"
#define ROJO    "\033[31m"
#define VERDE   "\033[32m"
#define AMARI   "\033[33m"
#define CIAN    "\033[36m"

/* ========================================================================
    IMPLEMENTACION DE FUNCIONES DE NEGOCIO
   ======================================================================== */

/*
 * registrarProducto
 * Flujo de captura con asignacion de ID desde la Tienda.
 */
void registrarProducto(Tienda& tienda) {
    Formatos::limpiarPantalla();
    cout << CIAN << NEG << "=== [ SISTEMA DE INVENTARIO : REGISTRO ] ===" << RST << endl;

    char nombre[Constantes::TAM_NOMBRE];
    char codigo[Constantes::TAM_CODIGO];
    char desc[Constantes::TAM_DESCRIPCION];
    float precio;
    int stock, stockMin, idProv;

    // 1. CAPTURA DE DATOS USANDO UTILIDADES
    cout << "\n > Ingrese Nombre del Producto: ";
    Formatos::limpiarBuffer();
    cin.getline(nombre, Constantes::TAM_NOMBRE);

    cout << " > Codigo de Barras/Referencia: ";
    cin.getline(codigo, Constantes::TAM_CODIGO);

    idProv = Formatos::leerEntero(" > ID del Proveedor asociado: ", 1, 999);
    precio = Formatos::leerFloat(" > Precio de Venta (unitario): ", 0.01, 100000.0);
    stock = Formatos::leerEntero(" > Stock Inicial en almacen: ", 0, 10000);
    stockMin = Formatos::leerEntero(" > Nivel de Stock Minimo (Alerta): ", 1, 500);

    cout << " > Descripcion breve: ";
    Formatos::limpiarBuffer();
    cin.getline(desc, Constantes::TAM_DESCRIPCION);

    // 2. CREACION Y CONFIGURACION DE LA ENTIDAD
    Producto nuevo(nombre, codigo, desc, idProv, precio, stock, stockMin);
    
    // Asignamos el ID actual de la tienda y aumentamos el contador
    nuevo.setId(tienda.generarIdProducto());

    // 3. PERSISTENCIA USANDO TEMPLATES
    if (nuevo.esDatosCompletos()) {
        if (GestorArchivos::guardarRegistro<Producto>(Constantes::ARCHIVO_PRODUCTOS, nuevo)) {
            cout << "\n" << VERDE << "[EXITO] Producto ID " << nuevo.getId() 
                 << " guardado correctamente." << RST << endl;
            
            // IMPORTANTE: Guardamos el estado de la tienda para no perder el contador de ID
            GestorArchivos::actualizarRegistro<Tienda>(Constantes::ARCHIVO_TIENDA, 0, tienda);
        } else {
            cout << "\n" << ROJO << "[ERROR] Fallo al escribir en productos.bin" << RST << endl;
        }
    } else {
        cout << "\n" << ROJO << "[ERROR] Datos invalidos segun reglas de negocio." << RST << endl;
    }

    Formatos::pausar();
}

/*
 * listarProductos
 * Muestra el inventario usando el template genérico de listado.
 */
void listarProductos(Tienda& tienda) {
    Formatos::limpiarPantalla();
    cout << CIAN << NEG << "=== [ INVENTARIO GENERAL DE PRODUCTOS ] ===" << RST << endl << endl;

    // Encabezado con formato profesional
    cout << NEG << left 
         << setw(6)  << "ID" 
         << setw(15) << "CODIGO" 
         << setw(30) << "NOMBRE" 
         << setw(12) << "PRECIO" 
         << setw(10) << "STOCK" 
         << "ESTADO" << RST << endl;
    
    for(int i=0; i<85; i++) cout << "-"; 
    cout << endl;

    // Delegamos la lectura y filtrado al GestorArchivos
    GestorArchivos::listarTodo<Producto>(Constantes::ARCHIVO_PRODUCTOS);

    for(int i=0; i<85; i++) cout << "-";
    cout << "\n" << AMARI << "Nota: Los productos marcados con [!] estan bajo el stock minimo." << RST << endl;
    
    Formatos::pausar();
}

/*
 * eliminarProducto
 * Aplica el borrado logico (encapsulamiento de estado).
 */
void eliminarProducto(Tienda& tienda) {
    Formatos::limpiarPantalla();
    cout << ROJO << NEG << "=== [ ELIMINAR PRODUCTO (BORRADO LOGICO) ] ===" << RST << endl;

    int idTarget = Formatos::leerEntero("\n > Ingrese el ID del producto a dar de baja: ", 1, 9999);

    // Buscamos el registro en el archivo binario
    int indice = GestorArchivos::buscarIndicePorID<Producto>(Constantes::ARCHIVO_PRODUCTOS, idTarget);

    if (indice != -1) {
        Producto temp;
        GestorArchivos::leerRegistroPorIndice<Producto>(Constantes::ARCHIVO_PRODUCTOS, indice, temp);

        // Verificamos si ya estaba eliminado
        if(temp.isEliminado()) {
            cout << AMARI << "\n[INFO] El producto ya se encuentra inactivo." << RST << endl;
        } else {
            cout << "\n Confirmar eliminacion de: " << NEG << temp.getNombre() << RST << " (s/n): ";
            char confirm;
            cin >> confirm;

            if (confirm == 's' || confirm == 'S') {
                temp.setEliminado(true); // Cambiamos estado interno
                
                if (GestorArchivos::actualizarRegistro<Producto>(Constantes::ARCHIVO_PRODUCTOS, indice, temp)) {
                    cout << VERDE << "\n[OK] Producto desactivado exitosamente." << RST << endl;
                }
            }
        }
    } else {
        cout << "\n" << ROJO << "[ERROR] ID " << idTarget << " no encontrado." << RST << endl;
    }

    Formatos::pausar();
}

/*
 * reporteStockCritico
 * Filtra el archivo buscando productos donde stock <= stockMinimo.
 */
void reporteStockCritico(Tienda& tienda) {
    Formatos::limpiarPantalla();
    cout << ROJO << NEG << "=== [ REPORTE: STOCK CRITICO ] ===" << RST << endl;
    
    int total = GestorArchivos::contarRegistros<Producto>(Constantes::ARCHIVO_PRODUCTOS);
    bool huboAlertas = false;

    for (int i = 0; i < total; i++) {
        Producto p;
        if (GestorArchivos::leerRegistroPorIndice<Producto>(Constantes::ARCHIVO_PRODUCTOS, i, p)) {
            // Regla de negocio: Stock actual menor o igual al minimo y no eliminado
            if (!p.isEliminado() && p.getStock() <= p.getStockMinimo()) {
                p.mostrarInformacionCompleta();
                cout << "------------------------------------------" << endl;
                huboAlertas = true;
            }
        }
    }

    if (!huboAlertas) {
        cout << VERDE << "\n  [OK] No hay productos con stock critico actualmente." << RST << endl;
    }

    Formatos::pausar();
}