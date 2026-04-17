#include "operacionesTransacciones.hpp"
#include "Transaccion.hpp"
#include "../Productos/Producto.hpp"
//#include "../Clientes/Clientes.hpp"
#include "../Proveedores/Proveedores.hpp"
#include "../persistencia/GestorArchivos.hpp"
#include "../Utilidades/Formatos.hpp"
#include "../Persistencia/Constantes.hpp"
#include <iostream>
#include <iomanip>

using namespace std;

/*
 * realizarVenta
 * Proceso complejo que vincula Cliente y Producto. Verifica existencia de 
 * ambos, valida stock suficiente, actualiza inventario y genera el registro.
 */
void realizarVenta(Tienda& tienda) {
    Formatos::limpiarPantalla();
    cout << "\n  ==========================================" << endl;
    cout << "  >>>        MODULO DE VENTAS POS        <<<" << endl;
    cout << "  ==========================================" << endl;

     // Validacion de Cliente
    int idCli = Formatos::leerEntero("  ID Cliente: ", 1, 9999);
  
    bool clienteOk = true;
    /* //
    while (GestorArchivos::leerRegistroPorIndice<Cliente>(Constantes::ARCHIVO_CLIENTES, i++, cli)) {
        if (cli.getId() == idCli && !cli.isEliminado()) {
            clienteOk = true;
            cout << "  [Cliente: " << cli.getNombre() << "]" << endl;
            break;
        } 
    
    }
    if (!clienteOk) {
        cout << "\n  [!] Error: El cliente no existe o esta inactivo." << endl;
        Formatos::pausar();
        return;
    }
    */
    // 2. Validacion de Producto y Stock
    int idProd = Formatos::leerEntero("  ID Producto: ", 1, 9999);
    Producto prod;
    int indiceProd = -1;
    int i = 0;
    while (GestorArchivos::leerRegistroPorIndice<Producto>(Constantes::ARCHIVO_PRODUCTOS, i, prod)) {
        if (prod.getId() == idProd && !prod.isEliminado()) {
            indiceProd = i;
            cout << "  [Producto: " << prod.getNombre() << " | Stock: " << prod.getStock() << "]" << endl;
            break;
        }
        i++;
    }

    if (indiceProd == -1) {
        cout << "\n  [!] Error: Producto no encontrado." << endl;
        Formatos::pausar();
        return;
    }

    int cant = Formatos::leerEntero("  Cantidad a vender: ", 1, prod.getStock());
    if (cant > prod.getStock()) {
        cout << "\n  [!] Error: No hay suficiente stock disponible." << endl;
        Formatos::pausar();
        return;
    }

    // 3. Confirmacion y Ejecucion
    cout << "\n  Total a cobrar: " << (prod.getPrecio() * cant) << " $" << endl;
    char confirm;
    cout << "  ¿Confirmar venta? (s/n): "; cin >> confirm;
    cin.ignore();

    if (tolower(confirm) == 's') {
        // Actualizar Stock
        prod.setStock(prod.getStock() - cant);
        GestorArchivos::actualizarRegistro<Producto>(Constantes::ARCHIVO_PRODUCTOS, indiceProd, prod);

        // Registrar Transaccion
        Transaccion t(tienda.generarIdTransaccion(), idProd, idCli, cant, prod.getPrecio(), 'V');
        GestorArchivos::guardarRegistro<Transaccion>(Constantes::ARCHIVO_TRANSACCIONES, t);

        cout << "\n  [OK] Venta # " << t.getId() << " procesada exitosamente." << endl;
        
        // Alerta de Stock Minimo (Punto 9.1 - Logica adicional)
        if (prod.getStock() <= prod.getStockMinimo()) {
            cout << "  [ADVERTENCIA] El producto ha alcanzado su stock minimo." << endl;
        }
    } else {
        cout << "\n  [X] Venta cancelada por el usuario." << endl;
    }
    Formatos::pausar();
}


/*
 * consultarHistorial
 * Muestra el libro diario de movimientos. Convierte el timestamp a 
 * formato legible y calcula subtotales.
 */
void consultarHistorial(const Tienda& tienda) {
    Formatos::limpiarPantalla();
    cout << "\n  " << string(85, '=') << endl;
    cout << "  " << left << setw(6) << "ID" << setw(10) << "FECHA" << setw(10) << "TIPO" 
         << setw(10) << "PROD" << setw(8) << "CANT" << setw(12) << "P. UNIT" << "TOTAL" << endl;
    cout << "  " << string(85, '-') << endl;

    Transaccion t;
    int i = 0;
    while (GestorArchivos::leerRegistroPorIndice<Transaccion>(Constantes::ARCHIVO_TRANSACCIONES, i++, t)) {
        if (!t.isEliminado()) {
            // Convertir time_t a string legible
            char bufferFecha[11];
            time_t fechaAux = t.getFecha();
            struct tm* timeinfo = localtime(&fechaAux);
            strftime(bufferFecha, 11, "%d/%m/%y", timeinfo);

            cout << "  " << left << setw(6) << t.getId() 
                 << setw(10) << bufferFecha
                 << setw(10) << (t.getTipo() == 'V' ? "VENTA" : "COMPRA")
                 << setw(10) << t.getIdProducto()
                 << setw(8) << t.getCantidad()
                 << setw(12) << fixed << setprecision(2) << t.getPrecioUnitario()
                 << t.getMontoTotal() << " $" << endl;
        }
    }
    cout << "  " << string(85, '=') << endl;
    Formatos::pausar();
}

/*
 * generarReporteDiario
 * Balance de caja. Filtra movimientos y muestra estadisticas basicas.
 */
void generarReporteDiario(const Tienda& tienda) {
    Formatos::limpiarPantalla();
    float sumVentas = 0, sumCompras = 0;
    int cntV = 0, cntC = 0;
    Transaccion t;
    int i = 0;

    while (GestorArchivos::leerRegistroPorIndice<Transaccion>(Constantes::ARCHIVO_TRANSACCIONES, i++, t)) {
        if (!t.isEliminado()) {
            if (t.getTipo() == 'V') {
                sumVentas += t.getMontoTotal();
                cntV++;
            } else {
                sumCompras += t.getMontoTotal();
                cntC++;
            }
        }
    }

    cout << "\n  ==========================================" << endl;
    cout << "  >>>       REPORTE FINANCIERO POS       <<<" << endl;
    cout << "  ==========================================" << endl;
    cout << "  Operaciones de Venta:    " << cntV << " (+" << sumVentas << " $)" << endl;
    cout << "  Operaciones de Compra:   " << cntC << " (-" << sumCompras << " $)" << endl;
    cout << "  ------------------------------------------" << endl;
    cout << "  BALANCE NETO EN CAJA:    " << (sumVentas - sumCompras) << " $" << endl;
    cout << "  ==========================================" << endl;
    Formatos::pausar();
}