#include <iostream>
#include <cstring>
#include <string>
#include <iomanip>
#include <ctime>
#include <cstdlib>
#include <cctype>

using namespace std;

struct Producto {
    int id;
    char codigo[20];
    char nombre[100];
    char descripcion[200];
    int idProveedor;
    float precio;
    int stock;
    char fechaRegistro[11];
};

struct Proveedor {
    int id;
    char nombre[100];
    char rif[20];
    char telefono[20];
    char email[100];
    char direccion[200];
    char fechaRegistro[11];
};

struct Cliente {
    int id;
    char nombre[100];
    char cedula[20];
    char telefono[20];
    char email[100];
    char direccion[200];
    char fechaRegistro[11];
};

struct Transaccion {
    int id;                    
    char tipo[10];            
    int idProducto;           
    int idRelacionado;        
    int cantidad;              
    float precioUnitario;      
    float total;               
    char fecha[11];            
    char descripcion[200];     
};


struct Tienda {
    char nombre[100];          // Nombre de la tienda
    char rif[20];              // RIF de la tienda
    
    // === Arrays dinámicos de entidades (Punteros) ===
    // Se inicializan con capacidad 5 según el punto 2.1
    Producto* productos;
    int numProductos;          // Cuántos hay actualmente
    int capacidadProductos;    // Tamaño máximo actual del array
    
    Proveedor* proveedores;
    int numProveedores;
    int capacidadProveedores;
    
    Cliente* clientes;
    int numClientes;
    int capacidadClientes;
    
    Transaccion* transacciones;
    int numTransacciones;
    int capacidadTransacciones;
    
    //  Contadores para IDs autoincrementales 
    // Deben empezar en 1 
    int siguienteIdProducto;
    int siguienteIdProveedor;
    int siguienteIdCliente;
    int siguienteIdTransaccion;
};

void inicializarTienda(Tienda* tienda, const char* nombre, const char* rif) {
    strcpy(tienda->nombre, nombre); 
    strcpy(tienda->rif, rif);
    
    // Inicializar arrays dinamicos con capacidad 5
    tienda->productos = new Producto[5];
    tienda->numProductos = 0;
    tienda->capacidadProductos = 5;
    
    tienda->proveedores = new Proveedor[5];
    tienda->numProveedores = 0;
    tienda->capacidadProveedores = 5;
    
    tienda->clientes = new Cliente[5];
    tienda->numClientes = 0;
    tienda->capacidadClientes = 5;
    
    tienda->transacciones = new Transaccion[5];
    tienda->numTransacciones = 0;
    tienda->capacidadTransacciones = 5;
    
    // Inicializar contadores de IDs
    tienda->siguienteIdProducto = 1;
    tienda->siguienteIdProveedor = 1;
    tienda->siguienteIdCliente = 1;
    tienda->siguienteIdTransaccion = 1;
}


void liberarTienda(Tienda* tienda) {
    delete[] tienda->productos;
    delete[] tienda->proveedores;
    delete[] tienda->clientes;
    delete[] tienda->transacciones;

    tienda->productos = NULL;
    tienda->proveedores = NULL;
    tienda->clientes = NULL;
    tienda->transacciones = NULL;
}

void crearProducto(Tienda* tienda) {
    char confirmar;
    cout <<"Desea registrar un nuevo producto? (S/N): ";
    cin >> confirmar;
    if(confirmar == 'N' || confirmar == 'n') {
        cout << "Registro de producto cancelado." << endl;
        return;
    }

    char entradaProducto[100];
    cout << " Ingrese el nombre del producto o escriba(cancelar) para cancelar "<< endl;
    cin >> entradaProducto;
    if(strcmp(entradaProducto, "cancelar") == 0) {
        cout << "Registro de producto cancelado." << endl;
        return;
    }

    //Solicitar y validar código de producto 
    char codigoTemp[20];
    cout << " Ingrese el codigo del producto o escriba (cancelar): ";
    cin >> codigoTemp;
    if(strcmp(codigoTemp, "cancelar") == 0) {
        cout << "Registro de producto cancelado." << endl;
        return;
    }

    // Validar que el codigo no este duplicado recorriendo los productos actuales
    for (int i = 0; i < tienda->numProductos; i++) {
        if (strcmp(tienda->productos[i].codigo, codigoTemp) == 0) {
            cout << "Error: El codigo '" << codigoTemp << "' ya existe en otro producto." << endl;
            return;
        }
    }

    float precioProducto;
    cout << " Ingrese el precio ddel producto o escriba (0) para cancelr "<< endl;
    cin >> precioProducto;
    if(precioProducto == 0) {
        cout << "Registro de producto cancelado." << endl;
        return;
    }

    if(precioProducto < 0) {
        cout << "El precio del producto no puede ser negativo, se cancela el registro" << endl;
        return;
    }


    int stockProducto;
    cout << " Ingrese el stock del producto o escriba (0) para cancelar "<< endl;
    cin >> stockProducto;
    if(stockProducto == 0) {
        cout << "Registro de producto cancelado." << endl;
        return;
    }

    if(stockProducto < 0) {
        cout << "El stock del producto no puede ser negativo, se cancela el registro" << endl;
        return;
    }

    //  Solicitar y validar ID del Proveedor
     int idProv;
     cout <<" Ingrese el ID del proveedor o escribe (0) para cancelar "<< endl;
    cin >> idProv;
    if(idProv == 0) {
        cout << "Registro de producto cancelado." << endl;
        return;
    }

    bool proveedorExiste = false;
    for(int i = 0; i < tienda->numProveedores; i++) {
        if(tienda->proveedores[i].id == idProv) {
            proveedorExiste = true;
            break;
        }
    }
    if(!proveedorExiste) {
        cout << "No existe un proveedor con ese ID, debe crearlo primero" << endl;
        return;
    }

    //  Mostrando resumen de datos ingresados
    cout << "\n========================================" << endl;
    cout << "       RESUMEN DEL NUEVO PRODUCTO       " << endl;
    cout << "========================================" << endl;
    cout << "Nombre:       " << entradaProducto << endl;
    cout << "Codigo:       " << codigoTemp << endl; 
    cout << "Precio:       " << precioProducto << endl;
    cout << "Stock:        " << stockProducto << endl;
    cout << "ID Proveedor: " << idProv << endl;
    cout << "========================================" << endl;


    //  Solicitar confirmacion antes de guardar
    char confirmarFinal;
    cout << "\n¿Desea guardar este producto permanentemente? (S/N): ";
    cin >> confirmarFinal;

    if (confirmarFinal == 'S' || confirmarFinal == 's') {

        // Verificamos si el arreglo está lleno antes de guardar
        if (tienda->numProductos == tienda->capacidadProductos) {
            // 1. Calculamos la nueva capacidad, que seria el doble
            int nuevaCapacidad = tienda->capacidadProductos * 2;
            
            // 2 Creamos un nuevo arreglo temporal con mas espacio
            Producto* nuevoArreglo = new Producto[nuevaCapacidad];
            
            // 3 Copiamos los productos existentes al nuevo arreglo
            for (int i = 0; i < tienda->numProductos; i++) {
                nuevoArreglo[i] = tienda->productos[i];
            }
            
            // 4. Liberamos la memoria del arreglo viejo para evitar fugas
            delete[] tienda->productos;
            
            // 5. Apuntamos al nuevo arreglo y actualizamos la capacidad
            tienda->productos = nuevoArreglo;
            tienda->capacidadProductos = nuevaCapacidad;
            
            cout << "  Capacidad aumentada a " << nuevaCapacidad  << endl;
        }

        int posicion = tienda->numProductos; // Usamos el contador actual

        // 1. Asignar ID automático
        tienda->productos[posicion].id = tienda->siguienteIdProducto;

        // 2. Copiamos los datos de tus variables a la estructura real
        strcpy(tienda->productos[posicion].codigo, codigoTemp);
        strcpy(tienda->productos[posicion].nombre, entradaProducto);
        tienda->productos[posicion].precio = precioProducto;
        tienda->productos[posicion].stock = stockProducto;
        tienda->productos[posicion].idProveedor = idProv; // Tu variable corta
        
    
        // 3. Obtener la fecha real del sistema usando <ctime>
        time_t t = time(0); // Obtiene el tiempo actual
        tm* now = localtime(&t); // Lo convierte a estructura de fecha local

        // Formateamos la fecha como DD/MM/AAAA y la guardamos en el producto
        strftime(tienda->productos[posicion].fechaRegistro, 11, "%d/%m/%Y", now);

        // 4. Aumentamos los contadores globales
        tienda->numProductos++;
        tienda->siguienteIdProducto++;

        cout << "Producto guardado exitosamente con ID: " << tienda->productos[posicion].id  << endl;


    } else {
        cout << "Operacion cancelada. Los datos han sido descartados." << endl;
    }


}
    void buscarProducto(Tienda* tienda) {
    if (tienda->numProductos == 0) {
        cout << "\n[!] No hay productos registrados para buscar." << endl;
        return;
    }

    int opcionBusqueda;
    cout << "\n========== MENU DE BUSQUEDA ==========" << endl;
    cout << "1. Buscar por ID (exacto)" << endl;
    cout << "2. Buscar por nombre (coincidencia parcial)" << endl;
    cout << "3. Buscar por codigo (coincidencia parcial)" << endl;
    cout << "4. Listar por proveedor" << endl;
    cout << "0. Cancelar" << endl;
    cout << "Seleccione una opcion: ";
    cin >> opcionBusqueda;

    if (opcionBusqueda == 0) return;

    switch (opcionBusqueda) {
        case 1: { // Buscar por ID (exacto)
            int idBusca;
            cout << "Ingrese el ID del producto: ";
            cin >> idBusca;
            
            bool encontrado = false;
            for (int i = 0; i < tienda->numProductos; i++) {
                if (tienda->productos[i].id == idBusca) {
                    cout << "\n PRODUCTO ENCONTRADO:" << endl;
                    cout << "Nombre: " << tienda->productos[i].nombre << " | Codigo: " << tienda->productos[i].codigo << endl;
                    encontrado = true;
                    break; 
                }
            }
            if (!encontrado) cout << "No se encontro ningun producto con ese ID." << endl;
            break;
        }

        case 2: { // Buscar por Nombre (coincidencia parcial)
            char busqueda[100];
            cout << "Ingrese el nombre a buscar: ";
            cin.ignore(); 
            cin.getline(busqueda, 100);
            
            // Pasamos la busqueda a minusculas manualmente para no complicarnos
            for(int i = 0; busqueda[i]; i++) busqueda[i] = tolower(busqueda[i]);

            bool algunResultado = false;
            for (int i = 0; i < tienda->numProductos; i++) {
                // Creamos una copia del nombre del producto en minusculas para comparar
                char nombreProd[100];
                strcpy(nombreProd, tienda->productos[i].nombre);
                for(int j = 0; nombreProd[j]; j++) nombreProd[j] = tolower(nombreProd[j]);

                // usams strstr  para la "coincidencia parcial"
                if (strstr(nombreProd, busqueda) != NULL) {
                    cout << "- ID: " << tienda->productos[i].id << " | " << tienda->productos[i].nombre << endl;
                    algunResultado = true;
                }
            }
            if (!algunResultado) cout << "No hay coincidencias para ese nombre." << endl;
            break;
        }
        case 3: { // Buscar por codigo (coincidencia parcial)
            char busqueda[20];
            cout << "Ingrese el codigo (o parte de el) a buscar: ";
            cin.ignore();
            cin.getline(busqueda, 20);
            
            for(int i = 0; busqueda[i]; i++) busqueda[i] = tolower(busqueda[i]);

            bool algunResultado = false;
            for (int i = 0; i < tienda->numProductos; i++) {
                char codigoProd[20];
                strcpy(codigoProd, tienda->productos[i].codigo);
                for(int j = 0; codigoProd[j]; j++) codigoProd[j] = tolower(codigoProd[j]);

                if (strstr(codigoProd, busqueda) != NULL) {
                    cout << "- ID: " << tienda->productos[i].id << " | Nombre: " << tienda->productos[i].nombre << " | Codigo: " << tienda->productos[i].codigo << endl;
                    algunResultado = true;
                }
            }
            if (!algunResultado) cout << "No hay coincidencias para ese codigo." << endl;
            break;
        }
        case 4: { // Listar por proveedor
            int idProvBusca;
            cout << "Ingrese el ID del proveedor para filtrar productos: ";
            cin >> idProvBusca;

            bool encontrado = false;
            cout << "\n--- Productos del Proveedor ID " << idProvBusca << " ---" << endl;
            for (int i = 0; i < tienda->numProductos; i++) {
                if (tienda->productos[i].idProveedor == idProvBusca) {
                    cout << "- ID: " << tienda->productos[i].id << " | " << tienda->productos[i].nombre << endl;
                    encontrado = true;
                }
            }
            if (!encontrado) cout << "No se encontraron productos asociados a este proveedor." << endl;
            break;
        }

        default:
            cout << "Opcion no valida." << endl;
            break;
    } 
}

          void actualizarProducto(Tienda* tienda) {
    if (tienda->numProductos == 0) {
        cout << "\n No hay productos para editar." << endl;
        return;
    }

    int idABuscar;
    cout << "\nIngrese el ID del producto que desea modificar: ";
    cin >> idABuscar;

    // Buscamos en que pueto de la fila está el producto
    int puestoEncontrado = -1; 
    for (int i = 0; i < tienda->numProductos; i++) {
        if (tienda->productos[i].id == idABuscar) {
            puestoEncontrado = i; // Guardamos el número de la "cajita"
            break;
        }
    }

    if (puestoEncontrado == -1) {
        cout << " Error, No existe un producto con ese ID." << endl;
        return;
    }

    // CREAMOS UN BORRADOR 
    // Esto nos permite editar mucho y solo guardar al final
    Producto borrador = tienda->productos[puestoEncontrado];
    int opcion;

    do {
        cout << "\n  EDITANDO PRODUCTO (BORRADOR) " << endl;
        cout << "Nombre actual: " << borrador.nombre << " | Precio: " << borrador.precio << endl;
        
        cout << "1. Codigo\n2. Nombre\n3. Descripcion\n4. ID Proveedor\n5. Precio\n6. Stock\n7. GUARDAR CAMBIOS\n0. CANCELAR" << endl;
        cout << "Seleccione una opcion: ";
        cin >> opcion;

        if (opcion == 1) { 
            cout << "Nuevo Codigo: "; cin >> borrador.codigo; 
        } else if (opcion == 2) { 
            cout << "Nuevo Nombre: "; cin.ignore(); cin.getline(borrador.nombre, 100); 
        } else if (opcion == 3) { 
            cout << "Nueva Descripcion: "; cin.ignore(); cin.getline(borrador.descripcion, 200); 
        } else if (opcion == 4) { 
            cout << "Nuevo ID Proveedor: "; cin >> borrador.idProveedor; 
        } else if (opcion == 5) { 
            cout << "Nuevo Precio: "; cin >> borrador.precio; 
        } else if (opcion == 6) { 
            cout << "Nuevo Stock: "; cin >> borrador.stock; 
        } else if (opcion == 7) { 
            //  Confirmamos antes de guardar
            char confirmar;
            cout << "¿Desea aplicar todos los cambios permanentemente? (s/n): ";
            cin >> confirmar;
            
            if (confirmar == 's' || confirmar == 'S') {
                tienda->productos[puestoEncontrado] = borrador; // Pasamos el borrador a la tienda real
                cout << " Cambios guardados con exito." << endl;
                return;
                 // Salimos de la funcion
            }
        }
    } while (opcion != 0);

    cout << "Edicion cancelada. No se modifico nada." << endl;
}

int main() {
    
    Tienda miTienda;
    // Inicializamos con datos de prueba 

    inicializarTienda(&miTienda, "La Bodeguita 2.0", "J-12345678-9");

    int opcionPrincipal;
    do {
        cout << "\n========== GESTION DE PRODUCTOS ==========" << endl;
        cout << "1. Registrar Producto" << endl;
        cout << "2. Buscar Producto" << endl;
        cout << "3. Actualizar Producto" << endl;
        cout << "4. Eliminar Producto" << endl;
        cout << "0. Salir del Sistema" << endl;
        cout << "Seleccione una opcion: ";
        cin >> opcionPrincipal;

        switch (opcionPrincipal) {
            case 1:
                crearProducto(&miTienda); 
                break;
            case 2:
                buscarProducto(&miTienda); 
                break;
            case 3:
                actualizarProducto(&miTienda); 
                break;
            case 4:
                
                cout << "Proximamente: Eliminar Producto..." << endl; 
                break;
            case 0:
                cout << "Cerrando sistema... ¡Hasta luego!" << endl;
                break;
            default:
                cout << "Opcion no valida, intente de nuevo." << endl;
        }
    } while (opcionPrincipal != 0);

    liberarTienda(&miTienda);
    
    return 0;
    
}