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
void actualizarStockManual(Tienda* tienda) { 
if (tienda->numProductos == 0) { // Verifica si el arreglo dinámico tiene elementos antes de procesar
cout << "\n[!] No hay productos registrados." << endl;
return;
}
int idB;
cout << "Ingrese ID del producto: "; cin >> idB;
int idx = -1; // Usamos idx igual a -1 para saber si encontramos el producto o no
for (int i = 0; i < tienda->numProductos; i++) { // Este ciclo recorre el arreglo buscando el ID que el usuario escribió
if (tienda->productos[i].id == idB) { idx = i; break; }
}
if (idx == -1) { cout << "No encontrado." << endl; return; }
cout << "Producto: " << tienda->productos[idx].nombre << endl;
cout << "Stock actual: " << tienda->productos[idx].stock << endl;
int aj;
cout << "Cantidad a sumar (ej: 10) o restar (ej: -5): "; cin >> aj;
if (tienda->productos[idx].stock + aj < 0) { // Esta es una validacion de seguridad para no tener stock negativo
cout << "Error: El stock no puede ser menor a cero." << endl;
} else {
tienda->productos[idx].stock += aj;
cout << "Stock actualizado con exito." << endl;
}
}
void listarProductos(Tienda* tienda) {
    if (tienda->numProductos == 0) {
        cout << "\n[!] No hay productos registrados." << endl;
        return;
    }

    cout << "\n========================================================================================" << endl;
    cout << "                         LISTADO DE PRODUCTOS                             " << endl;
    cout << "========================================================================================" << endl;
    cout << " ID |   Codigo   |       Nombre       |   Proveedor  |  Precio  |  Stock |    Fecha    " << endl;
    cout << "----------------------------------------------------------------------------------------" << endl;

    for (int i = 0; i < tienda->numProductos; i++) {
        // Buscamos el nombre del proveedor usando su ID
        const char* nombreProv = "Desconocido";
        for(int j = 0; j < tienda->numProveedores; j++) {
            if(tienda->proveedores[j].id == tienda->productos[i].idProveedor) {
                nombreProv = tienda->proveedores[j].nombre;
                break;
            }
        } // Usamos la estructura de datos para imprimir cada campo del producto actual
        // tienda->productos[i] accede al producto en la posición i del arreglo dinámico
        cout << setw(3) << tienda->productos[i].id << " | "
             << setw(10) << tienda->productos[i].codigo << " | "
             << setw(18) << tienda->productos[i].nombre << " | "
             << setw(12) << nombreProv << " | "
             << setw(8) << fixed << setprecision(2) << tienda->productos[i].precio << " | "
             << setw(6) << tienda->productos[i].stock << " | "
             << tienda->productos[i].fechaRegistro << endl;
    } // setw(num) se usa para reservar un espacio fijo de caracteres para el siguiente dato que se va a imprimir
    cout << "========================================================================================" << endl;
    cout << "Total de productos: " << tienda->numProductos << endl;
}

void eliminarProducto(Tienda* tienda) {
if (tienda->numProductos == 0) {
cout << "\n[!] No hay productos para eliminar." << endl;
return;
}
int idB;
cout << "ID del producto a eliminar: "; cin >> idB;
int idx = -1;
for (int i = 0; i < tienda->numProductos; i++) { // Buscamos en qué posición del arreglo se encuentra el ID solicitado
if (tienda->productos[i].id == idB) { idx = i; break; }
}
if (idx == -1) { cout << "No encontrado." << endl; return; }
for (int i = idx; i < tienda->numProductos - 1; i++) { // Movemos un elemento a la izquierda para sobreescribir el producto que queremos eliminar
tienda->productos[i] = tienda->productos[i+1];
}
tienda->numProductos--; // Al reducir numProductos, el programa ignorará la última casilla duplicada, dejando el arreglo limpio y con un espacio libre al final.
cout << "Producto eliminado de la base de datos." << endl;
}

void redimensionarProveedores(Tienda* tienda) {
int nuevaCapacidad = tienda->capacidadProveedores * 2;
Proveedor* nuevoArreglo = new Proveedor[nuevaCapacidad];

}
void crearProveedor(Tienda* tienda) {
if (tienda->numProveedores == tienda->capacidadProveedores) { // Verificamos si el numero de proveedores llegó al límite de la capacidad actual
int nCap = tienda->capacidadProveedores * 2;
Proveedor* nArr = new Proveedor[nCap]; // Creamos un nuevo arreglo temporal en la memoria heap con el doble de tamaño
for (int i = 0; i < tienda->numProveedores; i++) { nArr[i] = tienda->proveedores[i]; }
delete[] tienda->proveedores; // Liberamos la memoria del arreglo viejo para evitar fugas
tienda->proveedores = nArr;
tienda->capacidadProveedores = nCap;
}
int p = tienda->numProveedores; // Usamos 'p' como un índice auxiliar para escribir en la primera casilla libre
cout << "Nombre del Proveedor: "; cin.ignore(); cin.getline(tienda->proveedores[p].nombre, 100); // Limpiamos el buffer para que getline no se salte la entrada
cout << "RIF: "; cin >> tienda->proveedores[p].rif;
cout << "Email: "; cin >> tienda->proveedores[p].email;
tienda->proveedores[p].id = tienda->siguienteIdProveedor++; // Asignamos el ID autoincremental y luego lo aumentamos para el siguiente registro
tienda->numProveedores++; // Incrementamos el contador de proveedores activos en la tienda
cout << "Proveedor registrado con ID: " << tienda->proveedores[p].id << endl;

}
void buscarProveedor(Tienda* tienda) {
if (tienda->numProveedores == 0) {
cout << "\n[!] No hay proveedores registrados." << endl;
return;
}
char nombreB[100];
cout << "Ingrese nombre del proveedor a buscar: ";
cin.ignore(); cin.getline(nombreB, 100); // Limpiamos el buffer y usamos getline para permitir nombres con espacios
bool hallado = false; // Usamos una variable booleana como señal para saber si encontramos algo
for (int i = 0; i < tienda->numProveedores; i++) {
if (strstr(tienda->proveedores[i].nombre, nombreB) != NULL) { // Función strstr: Busca la cadena 'nombreB' dentro del nombre del proveedor actual. Si devuelve algo distinto a NULL, significa que hubo una coincidencia
cout << "ID: " << tienda->proveedores[i].id << " | Nombre: " << tienda->proveedores[i].nombre << " | RIF: " << tienda->proveedores[i].rif << endl;
hallado = true;
}
}
if (!hallado) cout << "No se encontraron coincidencias." << endl;
}

void actualizarProveedor(Tienda* tienda) {
int idB;
cout << "ID del proveedor a editar: "; cin >> idB;
int idx = -1;
for (int i = 0; i < tienda->numProveedores; i++) { // Comparamos el ID ingresado con los IDs guardados en el arreglo dinámico
if (tienda->proveedores[i].id == idB) { idx = i; break; } // Guardamos la ubicación exacta en la memoria
}
if (idx == -1) { cout << "Proveedor no existe." << endl; return; }
cout << "Nuevo nombre: "; cin.ignore(); // Al tener el índice (idx), podemos acceder directamente a los campos de ese proveedor
cin.getline(tienda->proveedores[idx].nombre, 100); // Limpiamos el salto de línea anterior para poder usar getline
cout << "Nuevo RIF: "; cin >> tienda->proveedores[idx].rif;
cout << "Datos actualizados." << endl;
}

void menuProveedores(Tienda* tienda) {
int op;
do {
cout << "\n--- GESTION DE PROVEEDORES ---" << endl;
cout << "1. Registrar Proveedor\n2. Buscar Proveedor\n3. Actualizar Proveedor\n0. Volver\nSeleccione: ";
cin >> op;
if (op == 1) crearProveedor(tienda);
if (op == 2) buscarProveedor(tienda);
if (op == 3) actualizarProveedor(tienda);
} while (op != 0);
}

void eliminarProveedor(Tienda* tienda) {
if (tienda->numProveedores == 0) {
cout << "\n[!] No hay proveedores registrados para eliminar." << endl;
return;
}

}
void redimensionarClientes(Tienda* tienda) {
int nuevaCap = tienda->capacidadClientes * 2;
Cliente* nuevoArreglo = new Cliente[nuevaCap];

}
void crearCliente(Tienda* tienda) {
if (tienda->numClientes == tienda->capacidadClientes) {
int nCap = tienda->capacidadClientes * 2;
Cliente* nArr = new Cliente[nCap];
for (int i = 0; i < tienda->numClientes; i++) { nArr[i] = tienda->clientes[i]; } // Copiamos uno a uno los clientes existentes al nuevo espacio de memoria
delete[] tienda->clientes;
tienda->clientes = nArr;
tienda->capacidadClientes = nCap; // Reasignamos el puntero de la tienda hacia la nueva dirección de memoria
}
int p = tienda->numClientes; // Usamos el valor actual de numClientes como índice para la nueva inserción
cout << "Nombre del Cliente: "; cin.ignore(); cin.getline(tienda->clientes[p].nombre, 100);
cout << "Cedula: "; cin >> tienda->clientes[p].cedula;
cout << "Email: "; cin >> tienda->clientes[p].email;
tienda->clientes[p].id = tienda->siguienteIdCliente++; // Asignamos el ID autoincremental que lleva el control global de la tienda
tienda->numClientes++; // Incrementamos el contador total de clientes registrados
cout << "Cliente registrado con ID: " << tienda->clientes[p].id << endl;
}

void buscarCliente(Tienda* tienda) {
if (tienda->numClientes == 0) {
cout << "\n[!] No hay clientes registrados." << endl;
return;
}
char ced[20];
cout << "Ingrese Cedula del cliente: "; cin >> ced;
for (int i = 0; i < tienda->numClientes; i++) {
if (strcmp(tienda->clientes[i].cedula, ced) == 0) { // Función strcmp: compara la cédula ingresada con la guardada en el arreglo
cout << "Cliente: " << tienda->clientes[i].nombre << " | ID: " << tienda->clientes[i].id << endl; // Si lo encontramos, mostramos la información y usamos 'return'
return;
}
}
cout << "Cliente no encontrado." << endl;
}

void actualizarCliente(Tienda* tienda) {
int idB;
cout << "ID del cliente a editar: "; cin >> idB;
for (int i = 0; i < tienda->numClientes; i++) {
if (tienda->clientes[i].id == idB) { // Verificamos si el ID de la posición actual coincide con el buscado
cout << "Nuevo nombre: "; cin.ignore(); // Limpiamos el buffer para que getline lea el nombre completo
cin.getline(tienda->clientes[i].nombre, 100);
cout << "Nueva Cedula: "; cin >> tienda->clientes[i].cedula;
cout << "Actualizado correctamente." << endl; // Una vez actualizados los campos, informamos y salimos de la función
return;
}
}
cout << "ID no encontrado." << endl;
}

void listarClientes(Tienda* tienda) {
if (tienda->numClientes == 0) {
cout << "No hay clientes para mostrar." << endl;
return;
}
cout << endl << "ID | NOMBRE | CEDULA | EMAIL" << endl; // Imprimimos una línea de títulos para identificar cada columna de la información
for (int i = 0; i < tienda->numClientes; i++) {
cout << tienda->clientes[i].id << " | " << tienda->clientes[i].nombre << " | " << tienda->clientes[i].cedula << " | " << tienda->clientes[i].email << endl;
} // tienda->clientes[i] representa el registro específico en cada vuelta del ciclo
}

void menuClientes(Tienda* tienda) {
int opcion = -1;
do {
cout << endl << "--- MENU GESTION DE CLIENTES ---" << endl;
cout << "1. Registrar Nuevo Cliente" << endl;
cout << "2. Buscar Cliente (Cedula)" << endl;
cout << "3. Actualizar Datos" << endl;
cout << "4. Listar Todos los Clientes" << endl;
cout << "0. Volver al Menu Principal" << endl;
cout << "Seleccione: ";
cin >> opcion;

switch (opcion) {
case 1:
crearCliente(tienda);
break;
case 2:
buscarCliente(tienda);
break;
case 3:
actualizarCliente(tienda);
break;
case 4:
listarClientes(tienda);
break;
case 0:
cout << "Regresando..." << endl;
break;
default:
cout << "Opcion no valida." << endl;
break;
}
} while (opcion != 0);
}

int main() {
    
    Tienda miTienda;
    // Inicializamos con datos de prueba 

    inicializarTienda(&miTienda, "La Bodeguita 2.0", "J-12345678-9");

    int opcionPrincipal=-1;
    
     do {
cout << endl << "========== MENU PRINCIPAL ==========" << endl;
cout << "1. Modulo de Productos" << endl;
cout << "2. Modulo de Proveedores" << endl;
cout << "3. Modulo de Clientes" << endl;
cout << "0. Salir del Sistema" << endl;
cout << "Seleccione una opcion: ";
cin >> opcionPrincipal;
switch (opcionPrincipal) {
case 1: {
int opP = -1;
do {
cout << endl << "--- GESTION DE PRODUCTOS ---" << endl;
cout << "1. Registrar 2. Buscar 3. Editar" << endl;
cout << "4. Stock 5. Eliminar 6. Listar 0. Volver" << endl;
cout << "Seleccione: "; cin >> opP;
if(opP == 1) crearProducto(&miTienda);
else if(opP == 2) buscarProducto(&miTienda);
else if(opP == 3) actualizarProducto(&miTienda);
else if(opP == 4) actualizarStockManual(&miTienda);
else if(opP == 5) eliminarProducto(&miTienda);
else if(opP == 6) listarProductos(&miTienda);
} while (opP != 0);
break;
}
case 2:
menuProveedores(&miTienda);
break;
case 3:
menuClientes(&miTienda);
break;
case 0:
cout << "Cerrando sistema..." << endl;
break;
default:
cout << "Opcion no valida." << endl;
break;
}
} while (opcionPrincipal != 0);
liberarTienda(&miTienda);
return 0;
}