#include <iostream>
#include "Tienda/Tienda.hpp"
#include "Persistencia/Constantes.hpp"
#include "Persistencia/GestorArchivos.hpp"
#include "Utilidades/Formatos.hpp"
#include "Productos/operacionesProductos.hpp"
#include "Proveedores/operacionesProveedores.hpp"
#include "Transacciones/operacionesTransacciones.hpp"

using namespace std;

/*
 * guardarTienda
 * Actualiza o guarda los datos de la tienda en el archivo de configuración.
 */
void guardarTienda(Tienda& t) {
    if (!GestorArchivos::actualizarRegistro<Tienda>(Constantes::ARCHIVO_TIENDA, 0, t)) {
        GestorArchivos::guardarRegistro<Tienda>(Constantes::ARCHIVO_TIENDA, t);
    }
}

int main() {
    // 1. INICIALIZACIÓN: Creamos los archivos .bin si no existen
    // Esto ejecutará el ofstream que está dentro de inicializarArchivo
    if (!GestorArchivos::inicializarSistemaArchivos()) {
        cout << "\n [!] ERROR CRITICO: No se pudo preparar el repositorio de datos." << endl;
        cout << " [!] Verifique que la carpeta 'datos' exista en el directorio del proyecto." << endl;
        Formatos::pausar();
        return 1; // Cerramos el programa porque sin archivos no funcionará nada
    }

    Tienda miTienda;
    
    // 2. CARGA: Intentar cargar la configuracion de la tienda
    GestorArchivos::leerRegistroPorIndice<Tienda>(Constantes::ARCHIVO_TIENDA, 0, miTienda);

    int opcion = -1;
    do {
        Formatos::limpiarPantalla();
        cout << "\n  ==========================================" << endl;
        cout << "  >>>      SISTEMA DE GESTION POS      <<<" << endl;
        cout << "  " << miTienda.getNombre() << endl;
        cout << "  ==========================================" << endl;
        cout << "  1. Modulo de Clientes" << endl;
        cout << "  2. Modulo de Productos" << endl;
        cout << "  3. Modulo de Proveedores" << endl;
        cout << "  4. Realizar Venta" << endl;
        cout << "  5. Realizar Compra" << endl;
        cout << "  6. Reportes" << endl;
        cout << "  0. Salir" << endl;
        
        opcion = Formatos::leerEntero("  Seleccione: ", 0, 6);

        switch (opcion) {
            case 1: 
                Formatos::limpiarPantalla();
                cout << "\n [!] Modulo de clientes no disponible todavia.\n";
                Formatos::pausar();
                break;

            case 2: 
                menuProductos(miTienda); 
                break;

            case 3: 
                listarProveedores(miTienda); 
                break;

            case 4: 
                realizarVenta(miTienda); 
                break;

            case 5: 
                Formatos::limpiarPantalla();
                cout << "\n [!] Modulo de compras no disponible todavia.\n";
                Formatos::pausar();
                break;

            case 6: 
                consultarHistorial(miTienda); 
                break;

            case 0: 
                guardarTienda(miTienda);
                cout << "\n Saliendo del sistema..." << endl;
                break;
        }
    } while (opcion != 0);

    return 0;
}