#include <iostream>
#include "Tienda/Tienda.hpp"
#include "Persistencia/Constantes.hpp"
#include "Persistencia/GestorArchivos.hpp"
#include "Utilidades/Formatos.hpp"
#include "Productos/operacionesProductos.hpp"
#include "Proveedores/operacionesProveedores.hpp"
#include "Transacciones/operacionesTransacciones.hpp"

using namespace std;

void guardarTienda(Tienda& t) {
    if (!GestorArchivos::actualizarRegistro<Tienda>(Constantes::ARCHIVO_TIENDA, 0, t)) {
        GestorArchivos::guardarRegistro<Tienda>(Constantes::ARCHIVO_TIENDA, t);
    }
}

int main() {
    Tienda miTienda;
    // Intentar cargar la configuracion inicial
    GestorArchivos::leerRegistroPorIndice<Tienda>(Constantes::ARCHIVO_TIENDA, 0, miTienda);

    int opcion = -1;
    do {
        Formatos::limpiarPantalla();
        cout << "\n  ==========================================" << endl;
        cout << "  >>>      SISTEMA DE GESTION POS        <<<" << endl;
        cout << "  " << miTienda.getNombre() << endl;
        cout << "  ==========================================" << endl;
        cout << "  1. Modulo de Clientes\n  2. Modulo de Productos\n  3. Modulo de Proveedores" << endl;
        cout << "  4. Realizar Venta\n  5. Realizar Compra\n  6. Reportes\n  0. Salir" << endl;
        
        opcion = Formatos::leerEntero("  Seleccione: ", 0, 6);

        switch (opcion) {
            case 1: /* llamar ops clientes */ break;
            case 2: listarProductos(miTienda); break;
            case 3: listarProveedores(miTienda); break;
            case 4: realizarVenta(miTienda); break;
            case 5: /*realizarCompra(miTienda);*/ break;
            case 6: consultarHistorial(miTienda); break;
            case 0: guardarTienda(miTienda); break;
        }
    } while (opcion != 0);

    return 0;
}