#include "Producto.hpp"
#include "../Proveedores/Proveedores.hpp" // Súbelo aquí
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
/*
 * registrarProducto
 * Valida la existencia del proveedor, solicita datos y persiste el producto
 * en el archivo binario.
 */
void registrarProducto(Tienda& tienda) {
    Formatos::limpiarPantalla();
    cout << "=== [ SISTEMA DE INVENTARIO : REGISTRO ] ===" << endl;

    // 1. Validar Proveedor
    int idProv = Formatos::leerEntero("\n > ID del Proveedor asociado: ", 1, 999);
    ArchivoHeader headerProv = GestorArchivos::leerHeader(Constantes::ARCHIVO_PROVEEDORES);
    bool proveedorExiste = false;

    // Solo entramos al bucle si hay registros
    for (int i = 0; i < headerProv.cantidadRegistros; i++) {
    Proveedor provAux;
    if (GestorArchivos::leerRegistroPorIndice<Proveedor>(Constantes::ARCHIVO_PROVEEDORES, i, provAux)) {
        if (provAux.getId() == idProv && !provAux.isEliminado()) {
            proveedorExiste = true;
            break;
        }
    }
}

    if (!proveedorExiste) {
        cout << "\n [!] ERROR: El Proveedor con ID " << idProv << " no existe." << endl;
        cout << " [!] Debe registrar el proveedor primero en el Modulo de Proveedores." << endl;
        Formatos::pausar();
        return; 
    }

    // 2. Recolectar datos del Producto
    string nombreStr, codigoStr, descStr;
    cout << " > Ingrese Nombre del Producto: "; cin >> nombreStr;
    cout << " > Codigo de Barras/Referencia: "; cin >> codigoStr;
    float precio = Formatos::leerFloat(" > Precio de Venta (unitario): ", 0.01, 999999);
    int stock = Formatos::leerEntero(" > Stock Inicial en almacen: ", 0, 10000);
    int stockMin = Formatos::leerEntero(" > Nivel de Stock Minimo (Alerta): ", 1, 500);
    cout << " > Descripcion breve: "; cin >> descStr;

    // 3. Crear el objeto con el ID automatico
    int nuevoId = tienda.generarIdProducto();
    Producto nuevo(
    nombreStr.c_str(), 
    codigoStr.c_str(), 
    descStr.c_str(), 
    nuevoId, 
    precio, 
    stock, 
    stockMin
);

    // 4. Intentar guardar
    if (GestorArchivos::guardarRegistro<Producto>(Constantes::ARCHIVO_PRODUCTOS, nuevo)) {
        cout << "\n [ OK ] Producto registrado exitosamente con ID: " << nuevoId << endl;
    } else {
        // Si entra aqui, es porque la ruta en Constantes::ARCHIVO_PRODUCTOS es invalida
        cout << "\n [ERROR] Fallo al escribir en " << Constantes::ARCHIVO_PRODUCTOS << endl;
        cout << " Verifique que la carpeta de datos exista." << endl;
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
 * actualizarProducto
 * Busca un producto por ID y permite modificar su precio o stock.
 * Guarda los cambios directamente en el archivo.
 */
void actualizarProducto(Tienda& tienda) {
    Formatos::limpiarPantalla();
    cout << "=== [ EDITAR / ACTUALIZAR PRODUCTO ] ===" << endl;
    
    int idBusca = Formatos::leerEntero(" Ingrese el ID del producto a modificar: ", 1, 9999);
    
    Producto p;
    int i = 0;
    bool encontrado = false;

    // Buscamos el registro en el archivo
    while (GestorArchivos::leerRegistroPorIndice<Producto>(Constantes::ARCHIVO_PRODUCTOS, i, p)) {
        if (p.getId() == idBusca && !p.isEliminado()) {
            encontrado = true;
            
            Formatos::limpiarPantalla();
            cout << "=== [ PRODUCTO ENCONTRADO ] ===" << endl;
            p.mostrarInformacionCompleta();
            cout << "-------------------------------------------" << endl;
            
            cout << " ¿Que desea modificar?" << endl;
            cout << " 1. Precio" << endl;
            cout << " 2. Stock" << endl;
            cout << " 3. Ambos" << endl;
            cout << " 0. Cancelar" << endl;
            int subOp = Formatos::leerEntero(" Seleccione: ", 0, 3);

            if (subOp == 0) return;

            if (subOp == 1 || subOp == 3) {
                float nuevoPrecio = Formatos::leerFloat(" Nuevo Precio: ", 0.01, 1000000);
                p.setPrecio(nuevoPrecio);
            }

            if (subOp == 2 || subOp == 3) {
                int nuevoStock = Formatos::leerEntero(" Nuevo Stock: ", 0, 10000);
                p.setStock(nuevoStock);
            }

            // Guardamos los cambios en la misma posicion 'i'
            if (GestorArchivos::actualizarRegistro<Producto>(Constantes::ARCHIVO_PRODUCTOS, i, p)) {
                cout << "\n[ OK ] Producto actualizado exitosamente." << endl;
            } else {
                cout << "\n[ ERROR ] No se pudo actualizar el archivo." << endl;
            }
            break;
        }
        i++;
    }

    if (!encontrado) {
        cout << "\n[ ! ] El ID ingresado no existe o el producto esta eliminado." << endl;
    }
    
    Formatos::pausar();
}

/*
 * menuProductos
 * Interfaz de usuario para el módulo de inventario.
 */
void menuProductos(Tienda& tienda) {
    int op = -1;
    do {
        Formatos::limpiarPantalla();
        cout << "==========================================================" << endl;
        cout << " >>>           GESTION DE PRODUCTOS           <<< " << endl;
        cout << "==========================================================" << endl;
        cout << "\n 1. Registrar nuevo producto" << endl;
        cout << " 2. Buscar producto" << endl;
        cout << " 3. Editar producto (Actualizar)" << endl;
        cout << " 4. Reporte de Stock Critico" << endl; // Cambié el orden para que coincida con tus funciones
        cout << " 5. Eliminar producto" << endl;
        cout << " 6. Listar todos los productos" << endl;
        cout << " 0. Volver al menu principal" << endl;
        
        op = Formatos::leerEntero("\n Seleccione: ", 0, 6);

        switch (op) {
            case 1: registrarProducto(tienda); break;
            case 2: buscarProducto(tienda); break;
            case 3: actualizarProducto(tienda); break;
            case 4: reporteStockCritico(tienda); break;
            case 5: eliminarProducto(tienda); break;
            case 6: listarProductos(tienda); break;
            case 0: return; // Sale del bucle y vuelve al main
        }
    } while (op != 0);
}
/*
 * eliminarProducto
 * Cambia el estado del producto a eliminado (borrado logico).
 */
void eliminarProducto(Tienda& tienda) {
    Formatos::limpiarPantalla();
    cout << "=== [ ELIMINAR PRODUCTO ] ===" << endl;
    int idElim = Formatos::leerEntero(" ID del producto a eliminar: ", 1, 9999);

    Producto p;
    int i = 0;
    bool exito = false;

    while (GestorArchivos::leerRegistroPorIndice<Producto>(Constantes::ARCHIVO_PRODUCTOS, i, p)) {
        if (p.getId() == idElim && !p.isEliminado()) {
            p.setEliminado(true);
            if (GestorArchivos::actualizarRegistro<Producto>(Constantes::ARCHIVO_PRODUCTOS, i, p)) {
                cout << "\n[ OK ] Producto eliminado correctamente." << endl;
                exito = true;
            }
            break;
        }
        i++;
    }

    if (!exito) cout << "\n[ ! ] No se pudo encontrar el producto." << endl;
    Formatos::pausar();
}

/*
 * buscarProducto
 * Solicita un ID al usuario, recorre el archivo y muestra la ficha 
 * detallada si el producto existe y no esta eliminado.
 */
void buscarProducto(Tienda& tienda) {
    Formatos::limpiarPantalla();
    cout << "==========================================================" << endl;
    cout << " >>>              BUSCAR PRODUCTO POR ID              <<< " << endl;
    cout << "==========================================================" << endl;

    int idBusca = Formatos::leerEntero("\n Ingrese el ID del producto: ", 1, 9999);
    
    Producto p;
    int i = 0;
    bool encontrado = false;

    // Recorremos el archivo registro por registro
    while (GestorArchivos::leerRegistroPorIndice<Producto>(Constantes::ARCHIVO_PRODUCTOS, i, p)) {
        // Verificamos si es el ID que buscamos y que no este borrado logicamente
        if (p.getId() == idBusca && !p.isEliminado()) {
            encontrado = true;
            Formatos::limpiarPantalla();
            cout << ">>> PRODUCTO ENCONTRADO <<<" << endl;
            p.mostrarInformacionCompleta(); // Usamos el metodo de la clase Producto
            break;
        }
        i++;
    }

    if (!encontrado) {
        cout << "\n [!] El producto con ID " << idBusca << " no existe o fue eliminado." << endl;
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