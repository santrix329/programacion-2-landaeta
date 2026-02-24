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

/*
 * Verificando que el email contenga un @ y al menos un '.' 
 */
bool validarEmail(const char* email) {
    const char* arroba = strchr(email, '@');
    if (arroba == nullptr) return false;
    const char* punto = strchr(arroba, '.');
    if (punto == nullptr || punto < arroba) return false;
    return true;

}

/*
 * aqui se verifica que la cadena tenga formato YYYY-MM-DD 
 */
bool validarFecha(const char* fecha) {
    if (strlen(fecha) != 10) return false;
    if (fecha[4] != '-' || fecha[7] != '-') return false;
    return true;
}

/*
 * Retorna true si el ID del producto existe en el arreglo
 */
bool existeProducto(Tienda* tienda, int id) {
    for (int i = 0; i < tienda->numProductos; i++) {
        if (tienda->productos[i].id == id) return true;
    }
    return false;
}

/*
 * Retorna true si el ID del proveedor existe en el arreglo
 */
bool existeProveedor(Tienda* tienda, int id) {
    for (int i = 0; i < tienda->numProveedores; i++) {
        if (tienda->proveedores[i].id == id) return true;
    }
    return false;
}

/*
 * Retorna true si el ID del cliente existe en el arreglo
 */
bool existeCliente(Tienda* tienda, int id) {
    for (int i = 0; i < tienda->numClientes; i++) {
        if (tienda->clientes[i].id == id) return true;
    }
    return false;
}

/*
 * Verifica si un codigo de producto ya existe para evitar duplicados.
 */
bool codigoDuplicado(Tienda* tienda, const char* codigo) {
    for (int i = 0; i < tienda->numProductos; i++) {
        if (strcmp(tienda->productos[i].codigo, codigo) == 0) return true;
    }
    return false;
}

/* 
 * Verifica exclusivamente en la lista de proveedores si el RIF ingresado 
 * ya se encuentra registrado para evitar duplicados 
 */
bool rifDuplicado(Tienda* tienda, const char* rif) {
    for (int i = 0; i < tienda->numProveedores; i++) {
        // Solo buscamos en proveedores, que sí tienen el campo .rif
        if (strcmp(tienda->proveedores[i].rif, rif) == 0) {
            return true;
        }
    }
    return false;
}

/*
 * Duplica la capacidad del arreglo dinamico de productos cuando este se 
 * encuentra lleno
 */
void redimensionarProductos(Tienda* tienda) {
    int nuevaCapacidad = tienda->capacidadProductos * 2;
    Producto* nuevoArreglo = new Producto[nuevaCapacidad];

    // Copiamos los elementos actuales al nuevo espacio
    for (int i = 0; i < tienda->numProductos; i++) {
        nuevoArreglo[i] = tienda->productos[i];
    }

    // Liberamos la memoria vieja
    delete[] tienda->productos;

    // Actualizamos la tienda con el nuevo arreglo y la nueva capacidad
    tienda->productos = nuevoArreglo;
    tienda->capacidadProductos = nuevaCapacidad;
    
    cout << " Capacidad de productos aumentada a " << nuevaCapacidad << endl;
}

/*
 * Solicita los datos de un nuevo producto, valida su existencia y
 * lo registra en el sistema, Llama a redimensionarProductos si el 
 * arreglo alcanza su capacidad maxima.
 */
void crearProducto(Tienda* tienda) {
    char respuestaConfirmacion;
    cout <<"Desea registrar un nuevo producto? (S/N): ";
    cin >> respuestaConfirmacion;
    if(respuestaConfirmacion == 'N' || respuestaConfirmacion == 'n') {
        cout << "Registro de producto cancelado" << endl;
        return;
    }

    char nombreIngresado[100];
    cout << " Ingrese el nombre del producto o escriba(cancelar) para cancelar "<< endl;
    cin.ignore();
    cin.getline(nombreIngresado, 100);
    if(strcmp(nombreIngresado, "cancelar") == 0) {
        cout << "Registro de producto cancelado" << endl;
        return;
    }

    char codigoValidar[20];
    cout << " Ingrese el codigo del producto o escriba (cancelar): ";
    cin >> codigoValidar;
    if(strcmp(codigoValidar, "cancelar") == 0) {
        cout << "Registro de producto cancelado" << endl;
        return;
    }

    if (codigoDuplicado(tienda, codigoValidar)) {
        cout << "Error, El codigo '" << codigoValidar << "' ya existe en otro producto" << endl;
        return;
    }

    float precioIngresado;
    cout << " Ingrese el precio del producto o escriba (0) para cancelar "<< endl;
    cin >> precioIngresado;
    if(precioIngresado == 0) {
        cout << "Registro de producto cancelado" << endl;
        return;
    }
    if(precioIngresado < 0) {
        cout << "El precio del producto no puede ser negativo, se cancela el registro" << endl;
        return;
    }

    int stockInicial;
    cout << " Ingrese el stock del producto o escriba (0) para cancelar "<< endl;
    cin >> stockInicial;
    if(stockInicial == 0) {
        cout << "Registro de producto cancelado" << endl;
        return;
    }
    if(stockInicial < 0) {
        cout << "El stock del producto no puede ser negativo, se cancela el registro" << endl;
        return;
    }

    int idProveedorAsociado;
    cout <<" Ingrese el ID del proveedor o escribe (0) para cancelar "<< endl;
    cin >> idProveedorAsociado;
    if(idProveedorAsociado == 0) {
        cout << "Registro de producto cancelado" << endl;
        return;
    }

    
    if(!existeProveedor(tienda, idProveedorAsociado)) {
        cout << "No existe un proveedor con ese ID, debe crearlo primero" << endl;
        return;
    }

    // --- RESUMEN  ---
    cout << "\n========================================" << endl;
    cout << "       RESUMEN DEL NUEVO PRODUCTO       " << endl;
    cout << "========================================" << endl;
    cout << "Nombre:       " << nombreIngresado << endl;
    cout << "Codigo:       " << codigoValidar << endl; 
    cout << "Precio:       " << precioIngresado << endl;
    cout << "Stock:        " << stockInicial << endl;
    cout << "ID Proveedor: " << idProveedorAsociado << endl;
    cout << "========================================" << endl;

    char guardarPermanente;
    cout << "\n Desea guardar este producto permanentemente? (S/N): ";
    cin >> guardarPermanente;

    if (guardarPermanente == 'S' || guardarPermanente == 's') {

        //  REDIMENSIONAMIENTO 
        if (tienda->numProductos == tienda->capacidadProductos) {
            redimensionarProductos(tienda);
        }

        int indiceNuevo = tienda->numProductos; 

        // Asignación de datos con nombres claros
        tienda->productos[indiceNuevo].id = tienda->siguienteIdProducto++;
        strcpy(tienda->productos[indiceNuevo].codigo, codigoValidar);
        strcpy(tienda->productos[indiceNuevo].nombre, nombreIngresado);
        tienda->productos[indiceNuevo].precio = precioIngresado;
        tienda->productos[indiceNuevo].stock = stockInicial;
        tienda->productos[indiceNuevo].idProveedor = idProveedorAsociado;
        
        // Fecha automática 
        time_t tiempoActual = time(0);
        tm* fechaEstructura = localtime(&tiempoActual);
        strftime(tienda->productos[indiceNuevo].fechaRegistro, 11, "%Y/%m/%d", fechaEstructura);

        tienda->numProductos++;

        cout << "Producto guardado satisfactoriamente con ID: " << tienda->productos[indiceNuevo].id << endl;
        cout << "Fecha de Registro: " << tienda->productos[indiceNuevo].fechaRegistro << endl;

    } else {
        cout << "Operacion cancelada, Los datos han sido descartados." << endl;
    }
}


     void buscarProducto(Tienda* tienda) {
    if (tienda->numProductos == 0) {
        cout << "\n No hay productos registrados para buscar." << endl;
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
        case 1: { // Buscar por ID exacto
            int idBusca;
            cout << "Ingrese el ID del producto: ";
            cin >> idBusca;
            
            bool encontrado = false;
            for (int i = 0; i < tienda->numProductos; i++) {
                if (tienda->productos[i].id == idBusca) {
                    cout << "\n Producto encontrado:" << endl;
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
            puestoEncontrado = i; // Guardamos el numero de la cajita
            break;
        }
    }

    if (puestoEncontrado == -1) {
        cout << " Error, No existe un producto con ese ID." << endl;
        return;
    }

    // cramos un borrador, esto nos permite editar mucho y solo guardar al final 
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
cout << "\n No hay productos registrados." << endl;

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
cout << "Error, El stock no puede ser menor a cero." << endl;
} else {
tienda->productos[idx].stock += aj;
cout << "Stock actualizado con exito." << endl;

}

}
void listarProductos(Tienda* tienda) {
    if (tienda->numProductos == 0) {
        cout << "\n No hay productos registrados." << endl;
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
cout << "\n No hay productos para eliminar." << endl;
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

    // Mudas los datos del arreglo viejo al nuevo 
    for (int i = 0; i < tienda->numProveedores; i++) {
        nuevoArreglo[i] = tienda->proveedores[i];
    }
    
     // Liberamos la memoria del arreglo viejo para evitar fugas
    delete[] tienda->proveedores;

    //  Decirle a la tienda dónde está su nueva casa
    tienda->proveedores = nuevoArreglo;
    tienda->capacidadProveedores = nuevaCapacidad;
    
    cout << " Capacidad de proveedores aumentada a " << nuevaCapacidad << " ---" << endl;

}

/*
 * Registro un nuevo proveedor validando que el RIF no este duplicado,
 * y que el email sea valido 
 */
void crearProveedor(Tienda* tienda) {
    char respuestaConfirmar;
    cout << "Desea registrar un nuevo proveedor? (S/N): ";
    cin >> respuestaConfirmar;
    if (respuestaConfirmar == 'N' || respuestaConfirmar == 'n') {
        cout << "Registro de proveedor cancelado." << endl;
        return;
    }

    char rifIngresado[20];
    cout << "Ingrese el RIF del proveedor o escriba (cancelar): ";
    cin >> rifIngresado;
    if (strcmp(rifIngresado, "cancelar") == 0) {
        cout << "Registro cancelado." << endl;
        return;
    }

    if (rifDuplicado(tienda, rifIngresado)) {
        cout << "Error, El RIF '" << rifIngresado << "' ya pertenece a otro proveedor registrado" << endl;
        return;
    }

    char nombreEmpresa[100];
    cout << "Ingrese el nombre de la empresa/proveedor: ";
    cin.ignore();
    cin.getline(nombreEmpresa, 100);

    char emailContacto[100];
    cout << "Ingrese el email del proveedor: ";
    cin >> emailContacto;

    if (!validarEmail(emailContacto)) {
        cout << "Error, El formato del email es invalido (ejemplo@dominio.com)." << endl;
        return;
    }

    // --- RESUMEN DE DATOS DEL PROVEEDOR ---
    cout << "\n========================================" << endl;
    cout << "      RESUMEN DEL NUEVO PROVEEDOR       " << endl;
    cout << "========================================" << endl;
    cout << "Nombre/Empresa: " << nombreEmpresa << endl;
    cout << "RIF:            " << rifIngresado << endl;
    cout << "Email:          " << emailContacto << endl;
    cout << "========================================" << endl;

    char guardarPermanente;
    cout << "\n¿Desea guardar este proveedor permanentemente? (S/N): ";
    cin >> guardarPermanente;

    if (guardarPermanente == 'S' || guardarPermanente == 's') {
        
        if (tienda->numProveedores == tienda->capacidadProveedores) {
            redimensionarProveedores(tienda);
        }

        int indiceNuevo = tienda->numProveedores;

        // Asignacion de datos a la estructura en la memoria dinamica
        tienda->proveedores[indiceNuevo].id = tienda->siguienteIdProveedor++;
        strcpy(tienda->proveedores[indiceNuevo].rif, rifIngresado);
        strcpy(tienda->proveedores[indiceNuevo].nombre, nombreEmpresa);
        strcpy(tienda->proveedores[indiceNuevo].email, emailContacto);
        
        tienda->numProveedores++;

        cout << "Proveedor guardado con exito., ID asignado: " << tienda->proveedores[indiceNuevo].id << endl;
    } else {
        cout << "Operacion cancelada, Los datos han sido descartados." << endl;
    }

}
void buscarProveedor(Tienda* tienda) {

if (tienda->numProveedores == 0) {
cout << "\n No hay proveedores registrados." << endl;
return;
}

char nombreB[100];
cout << "Ingrese nombre del proveedor a buscar: ";
cin.ignore(); 
cin.getline(nombreB, 100); // Limpiamos el buffer y usamos getline para permitir nombres con espacios

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

void listarProveedores(Tienda* tienda) {
    if (tienda->numProveedores == 0) {
        cout << "\n No hay proveedores registrados." << endl;
        return;
    }

    cout << "\n================================================================================" << endl;
    cout << "                         LISTADO DE PROVEEDORES                                 " << endl;
    cout << "================================================================================" << endl;
    cout << " ID |       Nombre       |       RIF       |           Email                    " << endl;
    cout << "--------------------------------------------------------------------------------" << endl;

    for (int i = 0; i < tienda->numProveedores; i++) {
        cout << setw(3) << tienda->proveedores[i].id << " | "
             << setw(18) << tienda->proveedores[i].nombre << " | "
             << setw(15) << tienda->proveedores[i].rif << " | "
             << tienda->proveedores[i].email << endl;
    }
    
    cout << "================================================================================" << endl;
    cout << "Total de proveedores: " << tienda->numProveedores << endl;
}

void eliminarProveedor(Tienda* tienda) {
    if (tienda->numProveedores == 0) {
        cout << "\n No hay proveedores registrados para eliminar." << endl;
        return;
    }

    int idB;
    cout << "ID del proveedor a eliminar: "; 
    cin >> idB;

    int idx = -1;
    // Buscamos la posición
    for (int i = 0; i < tienda->numProveedores; i++) {
        if (tienda->proveedores[i].id == idB) {
            idx = i;
            break;
        }
    }

    if (idx == -1) {
        cout << " Proveedor no encontrado." << endl;
        return;
    }

    // Confirmación de seguridad
    char confirmar;
    cout << "ESTA SEGURO? Si elimina al proveedor, los productos asociados quedaran huerfanos (s/n): ";
    cin >> confirmar;

    if (confirmar == 's' || confirmar == 'S') {
        // Corremos los asientos hacia la izquierda
        for (int i = idx; i < tienda->numProveedores - 1; i++) {
            tienda->proveedores[i] = tienda->proveedores[i+1];
        }
        tienda->numProveedores--; 
        cout << " Proveedor eliminado exitosamente." << endl;
    } else {
        cout << "Operacion cancelada." << endl;
    }
}
void menuProveedores(Tienda* tienda) {
int op;
do {

cout << "\n--- GESTION DE PROVEEDORES ---" << endl;
cout << "1. Registrar Proveedor\n2. Buscar Proveedor\n3. Actualizar Proveedor\n4. Eliminar Proveedor\n5. Listar Proveedores\n0. Volver\nSeleccione: ";
cin >> op;
if (op == 1) crearProveedor(tienda);
if (op == 2) buscarProveedor(tienda);
if (op == 3) actualizarProveedor(tienda);
if (op == 4) eliminarProveedor(tienda);
if (op == 5) listarProveedores(tienda);
} while (op != 0);

}

void redimensionarClientes(Tienda* tienda) {
    // 1. Aquí declaro la variable redimension y le asigno el nuevo espacio en memoria (el doble del actual)
    int nuevaCap = tienda->capacidadClientes * 2;
    Cliente* redimension = new Cliente[nuevaCap]; 

    
    // el programa olvida que nArr existe)
    for (int i = 0; i < tienda->numClientes; i++) {
        redimension[i] = tienda->clientes[i]; 
    }

    delete[] tienda->clientes;

    // 3. Aquí le asignas la nueva dirección
    tienda->clientes = redimension; 
    tienda->capacidadClientes = nuevaCap;

    cout << "\n Capacidad de clientes aumentada a " << nuevaCap << endl;
}

/*
 * Esto lo que hace es registrar un nuevo cliente validando el formato del email y gestionando
 * el redimensionamiento del arreglo dinamico.
 */
void crearCliente(Tienda* tienda) {
    char respuestaConfirmar;
    cout << "Desea registrar un nuevo cliente? (S/N): ";
    cin >> respuestaConfirmar;
    if (respuestaConfirmar == 'N' || respuestaConfirmar == 'n') {
        cout << "Registro de cliente cancelado." << endl;
        return;
    }

    char nombreCompleto[100];
    cout << "Ingrese el nombre completo del cliente: ";
    cin.ignore();
    cin.getline(nombreCompleto, 100);

    char cedulaIngresada[20];
    cout << "Ingrese la cedula de identidad: ";
    cin >> cedulaIngresada;

    char correoElectronico[100];
    cout << "Ingrese el email del cliente: ";
    cin >> correoElectronico;

    if (!validarEmail(correoElectronico)) {
        cout << "Error: El email no tiene un formato valido (ejemplo@correo.com)." << endl;
        return;
    }

    char numeroTelefono[20];
    cout << "Ingrese el numero de telefono: ";
    cin >> numeroTelefono;

    char direccionHogar[200];
    cout << "Ingrese la direccion de habitacion: ";
    cin.ignore();
    cin.getline(direccionHogar, 200);

    // --- RESUMEN DE DATOS DEL CLIENTE ---
    cout << "\n========================================" << endl;
    cout << "        RESUMEN DEL NUEVO CLIENTE       " << endl;
    cout << "========================================" << endl;
    cout << "Nombre:    " << nombreCompleto << endl;
    cout << "Cedula:    " << cedulaIngresada << endl;
    cout << "Email:     " << correoElectronico << endl;
    cout << "Telefono:  " << numeroTelefono << endl;
    cout << "Direccion: " << direccionHogar << endl;
    cout << "========================================" << endl;

    char guardarPermanente;
    cout << "\n¿Desea guardar los datos del cliente? (S/N): ";
    cin >> guardarPermanente;

    if (guardarPermanente == 'S' || guardarPermanente == 's') {
        
        // redimensionamiento
        if (tienda->numClientes == tienda->capacidadClientes) {
            redimensionarClientes(tienda);
        }

        int indiceNuevo = tienda->numClientes;

        // Asignación de datos a la memoria dinámica
        tienda->clientes[indiceNuevo].id = tienda->siguienteIdCliente++;
        strcpy(tienda->clientes[indiceNuevo].nombre, nombreCompleto);
        strcpy(tienda->clientes[indiceNuevo].cedula, cedulaIngresada);
        strcpy(tienda->clientes[indiceNuevo].email, correoElectronico);
        strcpy(tienda->clientes[indiceNuevo].telefono, numeroTelefono);
        strcpy(tienda->clientes[indiceNuevo].direccion, direccionHogar);
        
        // Fecha de registro automática
        time_t t = time(0);
        tm* now = localtime(&t);
        strftime(tienda->clientes[indiceNuevo].fechaRegistro, 11, "%Y/%m/%d", now);

        tienda->numClientes++;

        cout << "Cliente guardado exitosamente con ID: " << tienda->clientes[indiceNuevo].id << endl;
    } else {
        cout << "Operacion cancelada, Los datos han sido descartados." << endl;
    }
}

void buscarCliente(Tienda* tienda) {
if (tienda->numClientes == 0) {
cout << "\n No hay clientes registrados." << endl;
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
        cout << "\nNo hay clientes registrados." << endl;
        system("pause"); // Pausa para que el usuario lea el aviso
        return;
    }

    cout << "\n================================================================================" << endl;
    cout << "                         LISTADO DE CLIENTES                                    " << endl;
    cout << "================================================================================" << endl;
    cout << " ID |       Nombre       |      Cedula      |           Email                   " << endl;
    cout << "--------------------------------------------------------------------------------" << endl;

    for (int i = 0; i < tienda->numClientes; i++) {
        cout << setw(3) << tienda->clientes[i].id << " | "
             << setw(18) << tienda->clientes[i].nombre << " | "
             << setw(16) << tienda->clientes[i].cedula << " | "
             << tienda->clientes[i].email << endl;
    }
    
    cout << "================================================================================" << endl;
    cout << "Total de clientes: " << tienda->numClientes << endl;

    system("pause"); 
}

void eliminarCliente(Tienda* tienda) {
    if (tienda->numClientes == 0) {
        cout << "\n No hay clientes registrados para eliminar" << endl;
        system("pause");
        return;
    }
    char cedulaB[20];
    cout << "Ingrese la Cedula del cliente a eliminar: "; 
    cin >> cedulaB;
    int idx = -1;
    // Buscamos la posición del cliente por su cédula
    for (int i = 0; i < tienda->numClientes; i++) {
        if (strcmp(tienda->clientes[i].cedula, cedulaB) == 0) {
            idx = i;
            break;
        }
    }
    if (idx == -1) {
        cout << "Cliente con cedula " << cedulaB << " no encontrado." << endl;
        system("pause");
        return;
    }
    // Confirmación de seguridad
    char confirmar;
    cout << "Se eliminara a: " << tienda->clientes[idx].nombre << endl;
    cout << "Esta seguro de eliminar este registro? (s/n): ";
    cin >> confirmar;

    if (confirmar == 's' || confirmar == 'S') {
        // Desplazamos los elementos para cerrar el hueco
        for (int i = idx; i < tienda->numClientes - 1; i++) {
            tienda->clientes[i] = tienda->clientes[i+1];
        }
        tienda->numClientes--; 
        cout << " Cliente eliminado satisfactoriamente." << endl;
    } else {
        cout << "Operacion cancelada." << endl;
    }
    system("pause");
}

void menuClientes(Tienda* tienda) {

int opcion = -1;
do {
cout << endl << "--- MENU GESTION DE CLIENTES ---" << endl;
cout << "1. Registrar Nuevo Cliente" << endl;
cout << "2. Buscar Cliente (Cedula)" << endl;
cout << "3. Actualizar Datos" << endl;
cout << "4. Listar Todos los Clientes" << endl;
cout << "5. Eliminar Cliente" << endl;
cout << "0. Volver al Menu Princpal" << endl;
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
case 5:
eliminarCliente(tienda);
break;
case 0:
cout << "Regresando......." << endl;
break;
default:
cout << "Opcion no valida." << endl;
break;
}
} while (opcion != 0);
}

/*
 * Se duplica la capacidad del arreglo dinamico de transacciones cuando este
 * llega a su limite
 */
void redimensionarTransacciones(Tienda* tienda) {
    int nuevaCapacidad = tienda->capacidadTransacciones * 2;
    Transaccion* nuevoArreglo = new Transaccion[nuevaCapacidad];

    // Mudar los datos existentes
    for (int i = 0; i < tienda->numTransacciones; i++) {
        nuevoArreglo[i] = tienda->transacciones[i];
    }

    // Liberar la memoria vieja
    delete[] tienda->transacciones;

    // Actualizar los punteros en la estructura tienda
    tienda->transacciones = nuevoArreglo;
    tienda->capacidadTransacciones = nuevaCapacidad;

    cout << "Capacidad de transacciones aumentada a " << nuevaCapacidad << endl;
    system("pause");
}

/*
 * Registra una entrada de mercancia al inventario. Valida la existencia 
 * del producto y del proveedor usando funciones auxiliares, calcula el 
 * monto total y actualiza el stock tras la confirmacion del usuario.
 */
void registrarCompra(Tienda* tienda) {
    int idProductoBuscado, idProveedorBuscado, cantidadComprada;
    float precioUnitarioCompra;

    cout << "\n--- REGISTRAR COMPRA A PROVEEDOR ---" << endl;

    cout << "Ingrese ID del producto: ";
    cin >> idProductoBuscado;
    
    if (!existeProducto(tienda, idProductoBuscado)) {
        cout << "Error, El producto no existe." << endl;
        system("pause");
        return;
    }

    cout << "Ingrese ID del proveedor: ";
    cin >> idProveedorBuscado;
    
    
    if (!existeProveedor(tienda, idProveedorBuscado)) {
        cout << "Error, El proveedor no existe." << endl;
        system("pause");
        return;
    }

    // aqui capturamos los indices para mostrar el resumen de compra con nombres en lugar de IDs
    int indiceProd = -1;
    int indiceProv = -1;

    for (int i = 0; i < tienda->numProductos; i++) {
        if (tienda->productos[i].id == idProductoBuscado) {
            indiceProd = i;
            break;
        }
    }

    for (int i = 0; i < tienda->numProveedores; i++) {
        if (tienda->proveedores[i].id == idProveedorBuscado) {
            indiceProv = i;
            break;
        }
    }

    cout << "Cantidad comprada: ";
    cin >> cantidadComprada;
    
    if (cantidadComprada <= 0) { 
        cout << "La cantidad debe ser mayor a cero." << endl;
        system("pause");
        return;
    }

    cout << "Precio unitario de compra: ";
    cin >> precioUnitarioCompra;

    float montoTotalCalculado = (float)cantidadComprada * precioUnitarioCompra;

    // --- RESUMEN DE COMPRA ---
    cout << "\n========================================" << endl;
    cout << "           RESUMEN DE COMPRA            " << endl;
    cout << "========================================" << endl;
    cout << "Producto:  " << tienda->productos[indiceProd].nombre << endl;
    cout << "Proveedor: " << tienda->proveedores[indiceProv].nombre << endl;
    cout << "Cantidad:  " << cantidadComprada << endl;
    cout << "Total:     " << montoTotalCalculado << " $" << endl;
    cout << "========================================" << endl;

    char respuestaConfirmacion;
    cout << "\n¿Desea confirmar la compra? (S/N): ";
    cin >> respuestaConfirmacion;

    if (respuestaConfirmacion == 's' || respuestaConfirmacion == 'S') {
        
        // --- REDIMENSIONAMIENTO AUTOMATICO 2.6.1 ---
        if (tienda->numTransacciones == tienda->capacidadTransacciones) {
            redimensionarTransacciones(tienda);
        }

        int pos = tienda->numTransacciones;

        // Guardado en el struct Transaccion
        tienda->transacciones[pos].id = tienda->siguienteIdTransaccion++;
        strcpy(tienda->transacciones[pos].tipo, "COMPRA");
        tienda->transacciones[pos].idProducto = idProductoBuscado;
        tienda->transacciones[pos].idRelacionado = idProveedorBuscado;
        tienda->transacciones[pos].cantidad = cantidadComprada;
        tienda->transacciones[pos].precioUnitario = precioUnitarioCompra;
        tienda->transacciones[pos].total = montoTotalCalculado;

        // Fecha automatica
        time_t t = time(0);
        tm* now = localtime(&t);
        strftime(tienda->transacciones[pos].fecha, 11, "%d/%m/%Y", now);

        // Actualizacion de stock 
        tienda->productos[indiceProd].stock += cantidadComprada;

        tienda->numTransacciones++;
        cout << "Compra aprobada, El inventario ha sido actualizado." << endl;
    } else {
        cout << "Operacion cancelada." << endl;
    }

    system("pause");
}

/*
 * registrarVenta
 * Procesa la salida de productos para un cliente. Verifica la existencia
 * de las entidades, valida la disponibilidad de stock y gestiona el
 * almacenamiento dinamico de la transaccion resultante.
 */
void registrarVenta(Tienda* tienda) {
    int idProductoBuscado, idClienteBuscado, cantidadVenta;

    cout << "\n--- REGISTRAR VENTA A CLIENTE ---" << endl;

    cout << "Ingrese ID del Cliente: ";
    cin >> idClienteBuscado;
    
    if (!existeCliente(tienda, idClienteBuscado)) {
        cout << "Error, El cliente no existe." << endl;
        system("pause");
        return;
    }

    cout << "Ingrese ID del Producto: ";
    cin >> idProductoBuscado;
    
    if (!existeProducto(tienda, idProductoBuscado)) {
        cout << "Error, El producto no existe." << endl;
        system("pause");
        return;
    }

    // captura de indices para el resumen de venta
    int indiceProd = -1;
    int indiceCli = -1;

    for (int i = 0; i < tienda->numProductos; i++) {
        if (tienda->productos[i].id == idProductoBuscado) {
            indiceProd = i;
            break;
        }
    }

    for (int i = 0; i < tienda->numClientes; i++) {
        if (tienda->clientes[i].id == idClienteBuscado) {
            indiceCli = i;
            break;
        }
    }

    cout << "Cantidad a vender: ";
    cin >> cantidadVenta;

    
    if (cantidadVenta <= 0) {
        cout << "Error, La cantidad debe ser mayor a cero," << endl;
        system("pause");
        return;
    }

    if (cantidadVenta > tienda->productos[indiceProd].stock) {
        cout << "Error, Stock insuficiente. Solo quedan " << tienda->productos[indiceProd].stock << " unidades." << endl;
        system("pause");
        return;
    }

    // Usamos el precio que ya tiene el producto en el inventario
    float precioVenta = tienda->productos[indiceProd].precio;
    float montoTotalCalculado = (float)cantidadVenta * precioVenta;

    // RESUMEN DE VENTA 
    cout << "\n========================================" << endl;
    cout << "            RESUMEN DE VENTA            " << endl;
    cout << "========================================" << endl;
    cout << "Cliente:  " << tienda->clientes[indiceCli].nombre << endl;
    cout << "Producto: " << tienda->productos[indiceProd].nombre << endl;
    cout << "Cantidad: " << cantidadVenta << endl;
    cout << "Precio U: " << precioVenta << " $" << endl;
    cout << "Total:    " << montoTotalCalculado << " $" << endl;
    cout << "========================================" << endl;

    char respuestaConfirmacion;
    cout << "\n¿Desea confirmar la venta? (S/N): ";
    cin >> respuestaConfirmacion;

    if (respuestaConfirmacion == 's' || respuestaConfirmacion == 'S') {
        
    
        if (tienda->numTransacciones == tienda->capacidadTransacciones) {
            redimensionarTransacciones(tienda);
        }

        int pos = tienda->numTransacciones;

        tienda->transacciones[pos].id = tienda->siguienteIdTransaccion++;
        strcpy(tienda->transacciones[pos].tipo, "VENTA");
        tienda->transacciones[pos].idProducto = idProductoBuscado;
        tienda->transacciones[pos].idRelacionado = idClienteBuscado; // ID del Cliente
        tienda->transacciones[pos].cantidad = cantidadVenta;
        tienda->transacciones[pos].precioUnitario = precioVenta;
        tienda->transacciones[pos].total = montoTotalCalculado;

        // Fecha automatica
        time_t t = time(0);
        tm* now = localtime(&t);
        strftime(tienda->transacciones[pos].fecha, 11, "%d/%m/%Y", now);

        // ACTUALIZACION DE STOCK (Resta de inventario)
        tienda->productos[indiceProd].stock -= cantidadVenta;

        tienda->numTransacciones++;
        cout << "Venta realizada con exito, Stock actualizado." << endl;
    } else {
        cout << "Venta cancelada" << endl;
    }

    system("pause");
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
cout << "4. Modulo de Transacciones" << endl;
cout << "0. Salir del Sistema" << endl;
cout << "Seleccione una opcion: ";
cin >> opcionPrincipal;

system("cls"); 
switch (opcionPrincipal) {
case 1: {
int opP = -1;
do {

cout << "\n--- GESTION DE PRODUCTOS ---" << endl;
cout << "1. Registrar" << endl;
cout << "2. Buscar" << endl;
cout << "3. Editar" << endl;
cout << "4. Stock" << endl;
cout << "5. Eliminar" << endl;
cout << "6. Listar" << endl;
cout << "0. Volver" << endl;
cout << "Seleccione una opcion: ";
cin >> opP;
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
case 4: { // submenu de transacciones
                int opT = -1;
                do {
                   
                    cout << "--- GESTION DE TRANSACCIONES ---" << endl;
                    cout << "1. Registrar Compra (A Proveedor)" << endl;
                    cout << "2. Registrar Venta (A Cliente)" << endl;
                    cout << "3. Listar Historial de Transacciones" << endl;
                    cout << "0. Volver" << endl;
                    cout << "Seleccione: ";
                    cin >> opT;

                    if (opT == 1) registrarCompra(&miTienda);
                    else if (opT == 2) registrarVenta(&miTienda);
                    else if (opT == 3) cout << "error" << endl; // Aqui iria la función para listar transacciones, que no se ha implementado
                } while (opT != 0);
                break;
            }
case 0:
cout << "Cerrando sistema........" << endl;
break;
default:
cout << "Opcion no valida." << endl;
break;
}
} while (opcionPrincipal != 0);
liberarTienda(&miTienda);
return 0;
}
