#include <iostream>
#include <cstring>
#include <string>
#include <iomanip>
#include <ctime>
#include <cstdlib>
#include <cctype>
#include <fstream>  

using namespace std;

struct Producto {
    int id;
    char codigo[20];
    char nombre[100];
    char descripcion[200];
    int idProveedor;
    float precio;
    int stock;
               
    
    // 4. Estadísticas del registro
    int stockMinimo;                 
    int totalVendido;                
    
    // 5. Metadata de Control Obligatoria
    bool eliminado;                  // Para BORRADO LÓGICO
    time_t fechaCreacion;
    time_t fechaUltimaModificacion;

};

struct Proveedor {
    int id;
    char nombre[100];
    char rif[20];
    char telefono[20];
    char email[100];
    char direccion[200];
    
    bool eliminado;               
    time_t fechaRegistro;
};

struct Cliente {
    int id;
    char nombre[100];
    char cedula[20];
    char telefono[20];
    char email[100];
    char direccion[200];
    bool eliminado;          // Para el borrado logico
    time_t fechaRegistro;    // Formato profesional de fecha
    float totalGastado;
};

struct Transaccion {
    int id;                    
    char tipo[10];            
    int idRelacionado;        
    int idsProductos[15];      
    int cantidades[15];
    int numItems;              // Cuántos de los 15 espacios estamos usando
    
    float totalFactura; 
    time_t fecha; 
    char observaciones[200]; 
    bool eliminado;
};


struct Tienda {
    char nombre[100];          // Nombre de la tienda
    char rif[20];              // RIF de la tienda
    
    // Ya no guardamos los datos, solo totales para reportes rapidos
    float ventasTotales;       
    float comprasTotales;
    int totalTransaccionesRealizadas;
    // Los siguienteId se eliminan (ahora van en el Header de cada archivo).
};

// 1. Estructura obligatoria para el control de archivos
struct ArchivoHeader {
    int cantidadRegistros;      // Total histórico
    int proximoID;              // Siguiente ID a asignar
    int registrosActivos;       // No eliminados
    int version;                // Control de versión
};

/*
 * Esta funcion se asegura de que el archivo exista. 
 * Si no existe, lo construye con su respectivo Header
 */
bool inicializarArchivo(const char* nombreArchivo) {
    
    ifstream archivoExistente(nombreArchivo, ios::binary);

    if (archivoExistente.good()) {
        // Elarchivo ya existe, no hacemos nada y retornamos true
        archivoExistente.close(); 
        return true; 
    } 
    else {
        //  El archivo no existe, intentamos crearlo con su header inicial
        archivoExistente.close(); 
        
        // Creamos el archivo nuevo (modo escritura binaria)
        ofstream nuevoArchivo(nombreArchivo, ios::binary);

        if (nuevoArchivo.is_open()) {
            ArchivoHeader header;
            header.cantidadRegistros = 0;
            header.proximoID = 1;
            header.registrosActivos = 0;
            header.version = 1;

            // Escribimos los 16 bytes del Header al principio
            nuevoArchivo.write(reinterpret_cast<char*>(&header), sizeof(ArchivoHeader));
            
            nuevoArchivo.close();
            return true; // Archivo creado con éxito
        } 
        else {
            // Si por alguna razón (permisos, carpeta llena) no se pudo crear
            return false; 
        }
    }
}

ArchivoHeader leerHeader(const char* nombreArchivo) {
    ArchivoHeader header = {0, 0}; // Valores por defecto
    ifstream archivo(nombreArchivo, ios::binary);

    if (!archivo) {
        // Si el archivo no existe, lo creamos de una vez
        ofstream nuevo(nombreArchivo, ios::binary);
        nuevo.write(reinterpret_cast<char*>(&header), sizeof(ArchivoHeader));
        nuevo.close();
        return header; 
    }

    archivo.read(reinterpret_cast<char*>(&header), sizeof(ArchivoHeader));
    archivo.close();
    return header;
}

/*
 * actualizarHeader
 * Sobrescribe unicamente la seccion del header al principio del archivo
 */
bool actualizarHeader(const char* nombreArchivo, ArchivoHeader header) {
    // Abrimos en modo binario y entrada/salida (in|out) 
    // para no borrar lo que ya existe (ios::trunc)
    fstream archivo(nombreArchivo, ios::binary | ios::in | ios::out);

    if (!archivo) return false;

    // Saltamos al inicio del archivo
    archivo.seekp(0, ios::beg);
    
    // Sobrescribimos los 16 bytes del header
    archivo.write(reinterpret_cast<char*>(&header), sizeof(ArchivoHeader));
    
    archivo.close();
    return true;
}

/*
 * Calcula la posicion exacta en bytes de un registro segun su indice.
 * El indice 0 es el primer registro que guardaste.
 */
long calcularOffset(int indice, size_t tamanoEstructura) {
    // Formula: Saltamos los 16 bytes del Header + (el lugar del registro * su peso)
    return sizeof(ArchivoHeader) + (indice * tamanoEstructura);
}

/*
 * Recibe un ID (ej: 105) y nos dice en que indice fisico esta (ej: 4).
 * Retorna -1 si el ID no existe en el archivo.
 */
int buscarIndicePorID(const char* nombreArchivo, int idBuscado) {
    ArchivoHeader header = leerHeader(nombreArchivo);
    Producto p; // Usamos producto como ejemplo
    ifstream archivo(nombreArchivo, ios::binary);

    if (!archivo) return -1;

    // Recorremos todos los registros que dice el Header que existen
    for (int i = 0; i < header.cantidadRegistros; i++) {
        // Saltamos a la posicion del registro 'i'
        archivo.seekg(calcularOffset(i, sizeof(Producto)), ios::beg);
        
        // Leemos el registro
        archivo.read(reinterpret_cast<char*>(&p), sizeof(Producto));

        // ¿Es este el que buscamos y no esta borrado?
        if (p.id == idBuscado && !p.eliminado) {
            archivo.close();
            return i; // ¡Encontrado! Retornamos el indice 0, 1, 2...
        }
    }

    archivo.close();
    return -1; // No se encontro el ID
}
/*
 * convertir a Minusculas
 * Recorre la cadena y transforma cada caracter a su equivalente en minuscula.
 */
void convertirAMinusculas(char* cadena) {
    for (int i = 0; cadena[i]; i++) {
        cadena[i] = tolower(cadena[i]);
    }
}

/*
 * contieneSubstring
 * Retorna true si la cadena 'busqueda' se encuentra dentro de 'texto',
 * ignorando mayusculas/minusculas si se usa junto con convertirAMinusculas.
 */
bool contieneSubstring(const char* texto, const char* busqueda) {
    return strstr(texto, busqueda) != nullptr;
}

/*
 * obtenerFechaActual
 * Obtiene la fecha del sistema y la guarda en el buffer en formato YYYY-MM-DD.
 */
void obtenerFechaActual(char* buffer) {
    time_t t = time(0);
    struct tm* now = localtime(&t);
    // strftime formatea la fecha y la guarda en el char*
    strftime(buffer, 11, "%Y-%m-%d", now);
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
 * Verifica que la cadena tenga el formato YYYY/MM/DD.
 */
bool validarFecha(const char* fecha) {
    if (strlen(fecha) != 10) return false;
    // Validamos que tenga las barras en el lugar correcto
    if (fecha[4] != '/' || fecha[7] != '/') return false; 
    return true;
}

/*
 * codigoDuplicado (Versión Proyecto 2)
 * Busca en productos.bin si el código ya existe.
 */
bool codigoDuplicado(const char* codigo) {
    ArchivoHeader h = leerHeader("productos.bin");
    Producto p;
    ifstream archivo("productos.bin", ios::binary);
    if (!archivo) return false;

    for (int i = 0; i < h.cantidadRegistros; i++) {
        archivo.seekg(calcularOffset(i, sizeof(Producto)), ios::beg);
        archivo.read(reinterpret_cast<char*>(&p), sizeof(Producto));
        // Solo comparamos si no está eliminado lógicamente
        if (!p.eliminado && strcmp(p.codigo, codigo) == 0) {
            archivo.close();
            return true;
        }
    }
    archivo.close();
    return false;
}

/*
 * existeProveedor (Versión Proyecto 2)
 * Verifica si el ID del proveedor existe en proveedores.bin
 */
bool existeProveedor(int id) {
    
    return buscarIndicePorID("proveedores.bin", id) != -1;
}

/*
 * registrarProducto
 * Guarda un producto al final del archivo y actualiza el conteo global.
 */
bool registrarProducto(const char* nombreArchivo, Producto nuevo) {
    
    ArchivoHeader header = leerHeader(nombreArchivo);

    
    nuevo.id = header.proximoID;
    nuevo.eliminado = false;

    fstream archivo(nombreArchivo, ios::binary | ios::in | ios::out);
    
    if (!archivo.is_open()) {
        return false; 
    }
    long posicionFinal = calcularOffset(header.cantidadRegistros, sizeof(Producto));
    archivo.seekp(posicionFinal, ios::beg);
    
    
    archivo.write(reinterpret_cast<char*>(&nuevo), sizeof(Producto));
    archivo.close();

    // Actualizamos los numeros del Header
    header.cantidadRegistros++; // Sumamos 1 al total
    header.proximoID++;          // Preparamos el ID para el siguiente
    
    // Guardamos el Header actualizado al principio del archivo
    return actualizarHeader(nombreArchivo, header);
}

/*Busca el ID, calcula el offset,
 * se posiciona con seekp y marca eliminado = true
 */
bool eliminarProductoLogico(const char* nombreArchivo, int idABuscar) {
    // 1. Buscar el indice del registro mediante su ID (Usa la funcion del 3.2)
    int indice = buscarIndicePorID(nombreArchivo, idABuscar);
    
    if (indice == -1) {
        cout << "Error: No existe un producto con el ID " << idABuscar << endl;
        return false;
    }

    // 2. Calcular su posicion exacta en bytes
    long offset = calcularOffset(indice, sizeof(Producto));

    // 3. Abrir archivo y posicionar el cursor de escritura (seekp)
    fstream archivo(nombreArchivo, ios::binary | ios::in | ios::out);
    if (!archivo) return false;

    // Leemos el registro actual primero para modificar solo el campo necesario
    Producto p;
    archivo.seekg(offset, ios::beg);
    archivo.read(reinterpret_cast<char*>(&p), sizeof(Producto));

    // 4. Escribir la estructura modificada (eliminado = true)
    p.eliminado = true; // El cambio clave
    
    archivo.seekp(offset, ios::beg); // Nos aseguramos de estar en el byte correcto
    archivo.write(reinterpret_cast<char*>(&p), sizeof(Producto));
    
    archivo.close();

    // Actualizamos el Header (Punto 3.1)
    ArchivoHeader h = leerHeader(nombreArchivo);
    h.registrosActivos--; 
    actualizarHeader(nombreArchivo, h);

    return true;
}

/*
 * actualizarPrecioProducto: Ubica, salta y sobrescribe con un nuevo precio.
 */
bool actualizarPrecioProducto(const char* nombreArchivo, int idABuscar, float nuevoPrecio) {
    int indice = buscarIndicePorID(nombreArchivo, idABuscar);
    if (indice == -1) return false;

    long offset = calcularOffset(indice, sizeof(Producto));
    fstream archivo(nombreArchivo, ios::binary | ios::in | ios::out);

    Producto p;
    archivo.seekg(offset, ios::beg);
    archivo.read(reinterpret_cast<char*>(&p), sizeof(Producto));

    // Modificamos el dato que el usuario quiere
    p.precio = nuevoPrecio;

    // Sobrescribimos
    archivo.seekp(offset, ios::beg);
    archivo.write(reinterpret_cast<char*>(&p), sizeof(Producto));
    
    archivo.close();
    return true;
}

/*
 * Recorre el archivo de principio a fin usando el conteo del Header.
 */
void mostrarInventario(const char* nombreArchivo) {
    ArchivoHeader h = leerHeader(nombreArchivo);
    Producto p;
    ifstream archivo(nombreArchivo, ios::binary);

    if (!archivo) {
        cout << "No se pudo abrir el archivo o esta vacio." << endl;
        return;
    }

    cout << "\n--- LISTA DE PRODUCTOS REGISTRADOS ---" << endl;
    
    // Recorremos segun la cantidad de registros que dice el Header
    for (int i = 0; i < h.cantidadRegistros; i++) {
        
        archivo.seekg(calcularOffset(i, sizeof(Producto)), ios::beg);
        archivo.read(reinterpret_cast<char*>(&p), sizeof(Producto));

        // Mostramos solo los que no están eliminados logicamente
        if (!p.eliminado) {
            cout << "ID: " << p.id 
                 << " | Codigo: " << p.codigo 
                 << " | Nombre: " << p.nombre 
                 << " | Stock: " << p.stock 
                 << " | Precio: $" << p.precio << endl;
        }
    }
    archivo.close();
}

void crearProducto() { // Ya no recibe Tienda* porque no usamos la RAM
    char respuestaConfirmacion;
    
    while (true) {
        cout << "Desea registrar un nuevo producto? (S/N): ";
        cin >> respuestaConfirmacion;
        respuestaConfirmacion = tolower(respuestaConfirmacion);
        if (respuestaConfirmacion == 's' || respuestaConfirmacion == 'n') break;
        cout << "Opcion no valida, ingresa 'S' para si o 'N' para no." << endl;
    }

    if (respuestaConfirmacion == 'n') {
        cout << "Registro de producto cancelado" << endl;
        return;
    }

    char nombreIngresado[100];
    cout << " Ingrese el nombre del producto o escriba (cancelar) para cancelar: "<< endl;
    cin.ignore();
    cin.getline(nombreIngresado, 100);

    if(strcmp(nombreIngresado, "cancelar") == 0) return;

    char codigoValidar[20];
    cout << " Ingrese el codigo del producto: ";
    cin >> codigoValidar;

    // VALIDACIÓN USANDO EL ARCHIVO
    if (codigoDuplicado(codigoValidar)) {
        cout << "Error: El codigo '" << codigoValidar << "' ya existe en el archivo." << endl;
        return;
    }

    float precioIngresado;
    cout << " Ingrese el precio: ";
    cin >> precioIngresado;
    if(precioIngresado <= 0) {
        cout << "Precio no valido. Cancelado." << endl;
        return;
    }

    int stockInicial;
    cout << " Ingrese el stock: ";
    cin >> stockInicial;

    int idProveedorAsociado;
    cout <<" Ingrese el ID del proveedor: "<< endl;
    cin >> idProveedorAsociado;
    
    // VALIDACIÓN USANDO EL ARCHIVO
    if(!existeProveedor(idProveedorAsociado)) {
        cout << "No existe un proveedor con ese ID en el archivo." << endl;
        return;
    }

    // --- RESUMEN ---
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
    cout << "\n Desea guardar en productos.bin? (S/N): ";
    cin >> guardarPermanente;

    if (tolower(guardarPermanente) == 's') {
        // Creamos la estructura temporal para enviar al archivo
        Producto nuevo;
        strcpy(nuevo.codigo, codigoValidar);
        strcpy(nuevo.nombre, nombreIngresado);
        nuevo.precio = precioIngresado;
        nuevo.stock = stockInicial;
        nuevo.idProveedor = idProveedorAsociado;
        
        if (registrarProducto("productos.bin", nuevo)) {
            cout << "Exito, Producto guardado mediante Acceso Aleatorio." << endl;
        } else {
            cout << "Error al escribir en el disco" << endl;
        }
    }
}
/*
 * buscarProductosPorNombre
 * Retorna un arreglo dinámico con los índices de los productos que coinciden.
 */
/*
int* buscarProductosPorNombre(Tienda* tienda, const char* nombre, int* numResultados) {
    *numResultados = 0;
    char busquedaMin[100];
    strcpy(busquedaMin, nombre);
    convertirAMinusculas(busquedaMin);

    // Primer conteo
    for (int i = 0; i < tienda->numProductos; i++) {
        char nombreProdMin[100];
        strcpy(nombreProdMin, tienda->productos[i].nombre);
        convertirAMinusculas(nombreProdMin);

        if (contieneSubstring(nombreProdMin, busquedaMin)) {
            (*numResultados)++;
        }
    }

    if (*numResultados == 0) return nullptr;

    // Reserva y llenado
    int* indices = new int[*numResultados];
    int k = 0;
    for (int i = 0; i < tienda->numProductos; i++) {
        char nombreProdMin[100];
        strcpy(nombreProdMin, tienda->productos[i].nombre);
        convertirAMinusculas(nombreProdMin);

        if (contieneSubstring(nombreProdMin, busquedaMin)) {
            indices[k] = i;
            k++;
        }
    }
    return indices;
}

/*
 * buscarProducto
 * Menu principal de busquedas que delega la logica a funciones 
 * especializadas (por ID y por Nombre).

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
        case 1: { // Reutilizando buscarProductoPorId
            int idBusca;
            cout << "Ingrese el ID del producto: ";
            cin >> idBusca;
            
            int i = buscarProductoPorId(tienda, idBusca);

            if (i != -1) {
                cout << "\n Producto encontrado:" << endl;
                cout << "Nombre: " << tienda->productos[i].nombre 
                     << " | Codigo: " << tienda->productos[i].codigo << endl;
            } else {
                cout << "No se encontro ningun producto con ese ID." << endl;
            }
            break;
        }

        case 2: { // Reutilizando buscarProductosPorNombre
            char busqueda[100];
            cout << "Ingrese el nombre a buscar: ";
            cin.ignore(); 
            cin.getline(busqueda, 100);
            
            int numEncontrados = 0;
            int* resultados = buscarProductosPorNombre(tienda, busqueda, &numEncontrados);

            if (resultados != nullptr) {
                cout << "\n--- Resultados encontrados (" << numEncontrados << ") ---" << endl;
                for (int j = 0; j < numEncontrados; j++) {
                    int idx = resultados[j];
                    cout << "- ID: " << tienda->productos[idx].id 
                         << " | " << tienda->productos[idx].nombre << endl;
                }
                delete[] resultados; 
            } else {
                cout << "No hay coincidencias para ese nombre." << endl;
            }
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
    system("pause");
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

/*
 * actualizarStockProducto
 * Permite ajustar el inventario de un producto sumando o restando 
 * unidades. Valida que el stock final no sea negativo.
 
void actualizarStockProducto(Tienda* tienda) {
    if (tienda->numProductos == 0) {
        cout << "\n No hay productos registrados para ajustar stock" << endl;
        return;
    }

    int idB;
    cout << "\n--- AJUSTE MANUAL DE STOCK ---" << endl;
    cout << "Ingrese ID del producto: "; 
    cin >> idB;

    //  Buscar producto por ID
    int i = buscarProductoPorId(tienda, idB);

    if (i == -1) {
        cout << " Error, Producto no encontrado." << endl;
        return;
    }

    // Mostrar stock actual
    cout << "Producto: " << tienda->productos[i].nombre << endl;
    cout << "Stock actual: " << tienda->productos[i].stock << endl;

    //  Permitir ajuste manual (+/-)
    int ajuste;
    cout << "Cantidad a sumar (ej: 10) o restar (ej: -5): "; 
    cin >> ajuste;

    // Validar que stock final >= 0
    if (tienda->productos[i].stock + ajuste < 0) {
        cout << "  Error, El stock resultante no puede ser menor a cero." << endl;
    
    } else {
        //  Confirmar cambio
        tienda->productos[i].stock += ajuste;
        cout << "  Stock actualizado con exito. Nuevo total: " 
             << tienda->productos[i].stock << endl;
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

/*
 * eliminarProducto
 * Localiza un producto por ID y solicita confirmacion. Si la entrada
 * no es exactamente '1', la operacion se cancela por seguridad.
 
void eliminarProducto(Tienda* tienda) {
    if (tienda->numProductos == 0) {
        cout << "\n No hay productos para eliminar." << endl;
        return;
    }

    int idB;
    cout << "\n--- ELIMINAR PRODUCTO ---" << endl;
    cout << "ID del producto a eliminar: "; 
    cin >> idB;

    int idx = buscarProductoPorId(tienda, idB);

    if (idx == -1) { 
        cout << " Error, Producto no encontrado." << endl; 
        return; 
    }

    cout << "\n Producto: " << tienda->productos[idx].nombre << endl;
    cout << " Esta accion no se puede deshacer." << endl;
    
    int confirmar;
    cout << " ¿Confirmar eliminacion? (1: SI / 0: NO): ";
    cin >> confirmar;

    if (confirmar == 1) {
        // Lógica de mover elementos
        for (int i = idx; i < tienda->numProductos - 1; i++) {
            tienda->productos[i] = tienda->productos[i + 1];
        }
        tienda->numProductos--;
        cout << "  Producto eliminado exitosamente" << endl;
    } 
    else if (confirmar == 0) {
        cout << " Operacion cancelada por el usuario" << endl;
    } 
    else {
        // Si mete cualquier otro numero o letras, entra aqui
        cout << "  Opcion no valida, Operacion cancelada por seguridad" << endl;
    }
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
 
void crearProveedor(Tienda* tienda) {
    char respuestaConfirmar;
    bool opcionValida = false;

    // Ciclo de validacion: No deja pasar al usuario hasta que responda bien
    while (!opcionValida) {
        cout << "\n Desea registrar un nuevo proveedor? (S/N): ";
        cin >> respuestaConfirmar;

        // Convertimos a minuscula para que sea mas facil comparar
        respuestaConfirmar = tolower(respuestaConfirmar);

        if (respuestaConfirmar == 's' || respuestaConfirmar == 'n') {
            opcionValida = true;
        } else {
            cout << " Opcion no valida,  ingresa 'S' para si o 'N' para no." << endl;
        }
    }

    if (respuestaConfirmar == 'n') {
        cout << " Registro de proveedor cancelado." << endl;
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

/*
 * buscarProveedor
 * Menu interactivo para localizar proveedores por ID, nombre o RIF.
 * Utiliza busqueda exacta para IDs y coincidencia parcial para texto.
 
void buscarProveedor(Tienda* tienda) {
    if (tienda->numProveedores == 0) {
        cout << "\n No hay proveedores registrados para buscar." << endl;
        return;
    }

    int opcionBusqueda;
    cout << "\n========== MENU DE BUSQUEDA DE PROVEEDORES ==========" << endl;
    cout << "1. Buscar por ID (exacto)" << endl;
    cout << "2. Buscar por nombre (coincidencia parcial)" << endl;
    cout << "3. Buscar por RIF (exacto)" << endl;
    cout << "0. Cancelar" << endl;
    cout << "Seleccione una opcion: ";
    cin >> opcionBusqueda;

    if (opcionBusqueda == 0) return;

    switch (opcionBusqueda) {
        case 1: { // Buscar por ID
            int idBusca;
            cout << "Ingrese el ID del proveedor: ";
            cin >> idBusca;
            
            int i = buscarProveedorPorId(tienda, idBusca);
            if (i != -1) {
                cout << "\n Proveedor encontrado:" << endl;
                cout << "ID: " << tienda->proveedores[i].id 
                     << " | Nombre: " << tienda->proveedores[i].nombre 
                     << " | RIF: " << tienda->proveedores[i].rif << endl;
            } else {
                cout << "No se encontro ningun proveedor con ese ID." << endl;
            }
            break;
        }

        case 2: { // Buscar por nombre parcial e insensible a mayusculas
    char nombreB[100];
    cout << "Ingrese el nombre (o parte del nombre): ";
    cin.ignore();
    cin.getline(nombreB, 100);
    
    // Normalizamos la búsqueda del usuario
    convertirAMinusculas(nombreB); 

    bool hallado = false;
    for (int i = 0; i < tienda->numProveedores; i++) {
        // Creamos una copia temporal del nombre del proveedor para no alterar el original
        char nombreProvMin[100];
        strcpy(nombreProvMin, tienda->proveedores[i].nombre);
        convertirAMinusculas(nombreProvMin);

        // Usamos la nueva función booleana
        if (contieneSubstring(nombreProvMin, nombreB)) {
            cout << "ID: " << tienda->proveedores[i].id 
                 << " | Nombre: " << tienda->proveedores[i].nombre 
                 << " | RIF: " << tienda->proveedores[i].rif << endl;
            hallado = true;
        }
    }
    if (!hallado) cout << "No se encontraron coincidencias por nombre." << endl;
    break;
}

      
  case 3: { // Buscar por RIF
            char rifB[20];
            cout << "Ingrese el RIF a buscar: ";
            cin >> rifB;
            
            bool hallado = false;
            for (int i = 0; i < tienda->numProveedores; i++) {
                if (strcmp(tienda->proveedores[i].rif, rifB) == 0) {
                    cout << "\n Proveedor encontrado:" << endl;
                    cout << "ID: " << tienda->proveedores[i].id 
                         << " | Nombre: " << tienda->proveedores[i].nombre 
                         << " | RIF: " << tienda->proveedores[i].rif << endl;
                    hallado = true;
                    break;
                }
            }
            if (!hallado) cout << "No se encontro ningun proveedor con ese RIF." << endl;
            break;
        }

        default:
            cout << "Opcion no valida." << endl;
            break;
    }
}

/*
 * actualizarProveedor
 * Localiza un proveedor por ID y permite modificar sus datos basicos
 * de forma directa y sencilla.
 
void actualizarProveedor(Tienda* tienda) {
    int idB;
    cout << "ID del proveedor a editar: "; 
    cin >> idB;

    // Usamos la funcion de busqueda 
    int idx = buscarProveedorPorId(tienda, idB);

    // Si no lo encuentra, avisamos y salimos
    if (idx == -1) { 
        cout << "Proveedor no existe." << endl; 
        return; 
    }

    // Al tener el indice idx, accedemos directamente a los campos
    cout << "Nuevo nombre: "; 
    cin.ignore(); 
    cin.getline(tienda->proveedores[idx].nombre, 100);

    cout << "Nuevo RIF: "; 
    cin >> tienda->proveedores[idx].rif;

    cout << "Nuevo Telefono: ";
    cin >> tienda->proveedores[idx].telefono;

    cout << "Datos actualizados con exito." << endl;
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
    //  Aquí declaro la variable redimension y le asigno el nuevo espacio en memoria (el doble del actual)
    int nuevaCap = tienda->capacidadClientes * 2;
    Cliente* redimension = new Cliente[nuevaCap]; 

    
    // el programa olvida que nArr existe)
    for (int i = 0; i < tienda->numClientes; i++) {
        redimension[i] = tienda->clientes[i]; 
    }

    delete[] tienda->clientes;

    //  Aquí le asignas la nueva dirección
    tienda->clientes = redimension; 
    tienda->capacidadClientes = nuevaCap;

    cout << "\n Capacidad de clientes aumentada a " << nuevaCap << endl;
}

/*
 * Esto lo que hace es registrar un nuevo cliente validando el formato del email y gestionando
 * el redimensionamiento del arreglo dinamico.
 
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

        // Asignacion de datos a la memoria dinámica
        tienda->clientes[indiceNuevo].id = tienda->siguienteIdCliente++;
        strcpy(tienda->clientes[indiceNuevo].nombre, nombreCompleto);
        strcpy(tienda->clientes[indiceNuevo].cedula, cedulaIngresada);
        strcpy(tienda->clientes[indiceNuevo].email, correoElectronico);
        strcpy(tienda->clientes[indiceNuevo].telefono, numeroTelefono);
        strcpy(tienda->clientes[indiceNuevo].direccion, direccionHogar);
        
        // Fecha de registro automatica
        time_t t = time(0);
        tm* now = localtime(&t);
        strftime(tienda->clientes[indiceNuevo].fechaRegistro, 11, "%Y/%m/%d", now);

        tienda->numClientes++;

        cout << "Cliente guardado exitosamente con ID: " << tienda->clientes[indiceNuevo].id << endl;
    } else {
        cout << "Operacion cancelada, Los datos han sido descartados." << endl;
    }
}

/*
 * buscarCliente
 * Menu interactivo que permite localizar clientes por su ID, 
 * por su cedula o por coincidencia parcial en el nombre.
 
void buscarCliente(Tienda* tienda) {
    if (tienda->numClientes == 0) {
        cout << "\n No hay clientes registrados para buscar." << endl;
        return;
    }

    int opcion;
    cout << "\n--- MENU BUSQUEDA DE CLIENTES ---" << endl;
    cout << "1. Buscar por ID (exacto)" << endl;
    cout << "2. Buscar por Cedula (exacto)" << endl;
    cout << "3. Buscar por Nombre (coincidencia parcial)" << endl;
    cout << "0. Cancelar" << endl;
    cout << "Seleccione una opcion: "; 
    cin >> opcion;

    if (opcion == 0) return;

    switch (opcion) {
        case 1: { // Busqueda por ID usando la funcion del requisito 
            int idB;
            cout << "Ingrese ID del cliente: "; cin >> idB;
            int idx = buscarClientePorId(tienda, idB);
            
            if (idx != -1) {
                cout << "\n [Resultado] Cliente: " << tienda->clientes[idx].nombre 
                     << " | Cedula: " << tienda->clientes[idx].cedula << endl;
            } else {
                cout << "ID no encontrado." << endl;
            }
            break;
        }

        case 2: { // Busqueda por cedula (exacta)
            char ced[20];
            cout << "Ingrese Cedula del cliente: "; cin >> ced;
            bool hallado = false;
            for (int i = 0; i < tienda->numClientes; i++) {
                if (strcmp(tienda->clientes[i].cedula, ced) == 0) {
                    cout << "\n [Resultado] Cliente: " << tienda->clientes[i].nombre 
                         << " | ID: " << tienda->clientes[i].id << endl;
                    hallado = true;
                    break;
                }
            }
            if (!hallado) cout << "Cedula no encontrada." << endl;
            break;
        }

        case 3: { // Busqueda por nombre (parcial)
            char nombreB[100];
            cout << "Ingrese nombre o parte del nombre: ";
            cin.ignore();
            cin.getline(nombreB, 100);
            
            bool hallado = false;
            cout << "\n--- Coincidencias encontradas ---" << endl;
            for (int i = 0; i < tienda->numClientes; i++) {
                // strstr busca "nombreB" dentro del nombre del cliente i
                if (strstr(tienda->clientes[i].nombre, nombreB) != NULL) {
                    cout << "ID: " << tienda->clientes[i].id 
                         << " | Nombre: " << tienda->clientes[i].nombre 
                         << " | Cedula: " << tienda->clientes[i].cedula << endl;
                    hallado = true;
                }
            }
            if (!hallado) cout << "No se encontraron clientes con ese nombre." << endl;
            break;
        }

        default:
            cout << "Opcion no valida." << endl;
            break;
    }
}

/*
 * actualizarCliente
 * Busca un cliente por su identificador y permite modificar sus datos.
 * Reutiliza la funcion de busqueda para evitar repetir ciclos for.
 
void actualizarCliente(Tienda* tienda) {
    if (tienda->numClientes == 0) {
        cout << "\n No hay clientes registrados para editar." << endl;
        return;
    }

    int idB;
    cout << "\n--- ACTUALIZAR DATOS DE CLIENTE ---" << endl;
    cout << "ID del cliente a editar: "; 
    cin >> idB;

    // Buscamos la posicion del cliente usando la funcion de busqueda
    int i = buscarClientePorId(tienda, idB);

    // Si el indice es valido (distinto de -1), editamos
    if (i != -1) {
        cout << "Editando a: " << tienda->clientes[i].nombre << endl;
        cout << "-------------------------------------------" << endl;

        cout << "Nuevo nombre: "; 
        cin.ignore();
        cin.getline(tienda->clientes[i].nombre, 100);
        
        cout << "Nueva Cedula: "; 
        cin >> tienda->clientes[i].cedula;

        cout << "Nuevo Telefono: ";
        cin >> tienda->clientes[i].telefono;

        cout << "Nueva Direccion: "; 
        cin.ignore();
        cin.getline(tienda->clientes[i].direccion, 200);

        cout << "\n [OK] Cliente actualizado correctamente." << endl;
    } else {
        cout << " [!] Error: ID no encontrado." << endl;
    }
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
        strftime(tienda->transacciones[pos].fecha, 11, "%Y/%m/%d", now);

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
        strftime(tienda->transacciones[pos].fecha, 11, "%Y/%m/%d", now);

        // ACTUALIZACION DE STOCK (Resta de inventario)
        tienda->productos[indiceProd].stock -= cantidadVenta;

        tienda->numTransacciones++;
        cout << "Venta realizada con exito, Stock actualizado." << endl;
    } else {
        cout << "Venta cancelada" << endl;
    }

    system("pause");
}

/*
 * buscarTransacciones
 * Localiza registros en el historial utilizando criterios de busqueda.
 * Se utiliza una estructura expandida para facilitar la comprension 
 * del flujo de datos y la aplicacion de filtros individuales.
 
void buscarTransacciones(Tienda* tienda) {
    // 1. VALIDACION INICIAL
    if (tienda->numTransacciones == 0) {
        cout << "\n No hay transacciones registradas actualmente." << endl;
        system("pause");
        return;
    }

    int opcion;
    cout << "\n--- MENU DE BUSQUEDA DE TRANSACCIONES ---" << endl;
    cout << "1. Por ID de Transaccion" << endl;
    cout << "2. Por ID de Producto" << endl;
    cout << "3. Por ID de Cliente" << endl;
    cout << "4. Por ID de Proveedor" << endl;
    cout << "5. Por Fecha (YYYY/MM/DD)" << endl;
    cout << "6. Por Tipo (COMPRA/VENTA)" << endl;
    cout << "0. Regresar" << endl;
    cout << "Seleccione una opcion: ";
    cin >> opcion;

    if (opcion == 0) return;

    int idBuscado;
    char filtroTexto[20];
    bool encontrado = false;

    switch (opcion) {
        
        case 1: // BUSCAR POR ID DE TRANSACCION
            cout << "Ingrese ID de la transaccion: ";
            cin >> idBuscado;
            cout << "\nID   | TIPO   | PROD | RELAC. | CANT | TOTAL | FECHA" << endl;
            cout << "------------------------------------------------------" << endl;
            for (int i = 0; i < tienda->numTransacciones; i++) {
                if (tienda->transacciones[i].id == idBuscado) {
                    // Imprimimos la linea directamente
                    cout << tienda->transacciones[i].id << " | " 
                         << tienda->transacciones[i].tipo << " | "
                         << tienda->transacciones[i].idProducto << " | "
                         << tienda->transacciones[i].idRelacionado << " | "
                         << tienda->transacciones[i].cantidad << " | "
                         << tienda->transacciones[i].total << " | "
                         << tienda->transacciones[i].fecha << endl;
                    encontrado = true;
                    break; 
                }
            }
            break;

        case 2: // BUSCAR POR ID DE PRODUCTO
            cout << "Ingrese ID del Producto: ";
            cin >> idBuscado;
            if (!existeProducto(tienda, idBuscado)) {
                cout << "Error, El producto no existe en el sistema." << endl;
            } else {
                cout << "\nID   | TIPO   | PROD | RELAC. | CANT | TOTAL | FECHA" << endl;
                cout << "------------------------------------------------------" << endl;
                for (int i = 0; i < tienda->numTransacciones; i++) {
                    if (tienda->transacciones[i].idProducto == idBuscado) {
                        cout << tienda->transacciones[i].id << " | " 
                             << tienda->transacciones[i].tipo << " | "
                             << tienda->transacciones[i].idProducto << " | "
                             << tienda->transacciones[i].idRelacionado << " | "
                             << tienda->transacciones[i].cantidad << " | "
                             << tienda->transacciones[i].total << " | "
                             << tienda->transacciones[i].fecha << endl;
                        encontrado = true;
                    }
                }
            }
            break;

        case 3: // BUSCAR POR ID DE CLIENTE
            cout << "Ingrese ID del Cliente: ";
            cin >> idBuscado;
            if (!existeCliente(tienda, idBuscado)) {
                cout << "Error, El cliente no existe" << endl;
            } else {
                cout << "\nID   | TIPO   | PROD | RELAC. | CANT | TOTAL | FECHA" << endl;
                cout << "------------------------------------------------------" << endl;
                for (int i = 0; i < tienda->numTransacciones; i++) {
                    if (strcmp(tienda->transacciones[i].tipo, "VENTA") == 0 && 
                        tienda->transacciones[i].idRelacionado == idBuscado) {
                        cout << tienda->transacciones[i].id << " | " 
                             << tienda->transacciones[i].tipo << " | "
                             << tienda->transacciones[i].idProducto << " | "
                             << tienda->transacciones[i].idRelacionado << " | "
                             << tienda->transacciones[i].cantidad << " | "
                             << tienda->transacciones[i].total << " | "
                             << tienda->transacciones[i].fecha << endl;
                        encontrado = true;
                    }
                }
            }
            break;

        case 4: // BUSCAR POR ID DE PROVEEDOR
            cout << "Ingrese ID del Proveedor: ";
            cin >> idBuscado;
            if (!existeProveedor(tienda, idBuscado)) {
                cout << "Error: El proveedor no existe." << endl;
            } else {
                cout << "\nID   | TIPO   | PROD | RELAC. | CANT | TOTAL | FECHA" << endl;
                cout << "------------------------------------------------------" << endl;
                for (int i = 0; i < tienda->numTransacciones; i++) {
                    if (strcmp(tienda->transacciones[i].tipo, "COMPRA") == 0 && 
                        tienda->transacciones[i].idRelacionado == idBuscado) {
                        cout << tienda->transacciones[i].id << " | " 
                             << tienda->transacciones[i].tipo << " | "
                             << tienda->transacciones[i].idProducto << " | "
                             << tienda->transacciones[i].idRelacionado << " | "
                             << tienda->transacciones[i].cantidad << " | "
                             << tienda->transacciones[i].total << " | "
                             << tienda->transacciones[i].fecha << endl;
                        encontrado = true;
                    }
                }
            }
            break;

        case 5: { // BUSCAR POR FECHA
    char fechaHoy[11];
    obtenerFechaActual(fechaHoy); 

    cout << "\n--- BUSQUEDA POR FECHA ---" << endl;
    cout << "Ingrese la fecha (YYYY/MM/DD): ";
    cin >> filtroTexto;

    // AQUÍ LA METEMOS: Validación de seguridad
    if (!validarFecha(filtroTexto)) {
        cout << " Error, El formato de fecha es incorrecto (Debe ser YYYY/MM/DD)." << endl;
        break; // Sale del caso y vuelve al menú
    }

    cout << "\nID   | TIPO   | TOTAL | FECHA" << endl;
    cout << "--------------------------------------" << endl;
    for (int i = 0; i < tienda->numTransacciones; i++) {
        if (contieneSubstring(tienda->transacciones[i].fecha, filtroTexto)) {
            cout << tienda->transacciones[i].id << " | " 
                 << tienda->transacciones[i].tipo << " | "
                 << tienda->transacciones[i].total << " | "
                 << tienda->transacciones[i].fecha << endl;
            encontrado = true;
        }
    }
    break;
}


        case 6: // BUSCAR POR TIPO
            cout << "Ingrese el tipo (COMPRA o VENTA): ";
            cin >> filtroTexto;
            cout << "\nID   | TIPO   | PROD | RELAC. | CANT | TOTAL | FECHA" << endl;
            cout << "------------------------------------------------------" << endl;
            for (int i = 0; i < tienda->numTransacciones; i++) {
                if (strcmp(tienda->transacciones[i].tipo, filtroTexto) == 0) {
                    cout << tienda->transacciones[i].id << " | " 
                         << tienda->transacciones[i].tipo << " | "
                         << tienda->transacciones[i].idProducto << " | "
                         << tienda->transacciones[i].idRelacionado << " | "
                         << tienda->transacciones[i].cantidad << " | "
                         << tienda->transacciones[i].total << " | "
                         << tienda->transacciones[i].fecha << endl;
                    encontrado = true;
                }
            }
            break;

        default:
            cout << "Opcion no valida." << endl;
            break;
    }

    if (!encontrado && opcion != 0) {
        cout << "\n No se encontraron transacciones para esta busqueda." << endl;
    }

    system("pause");
}

/*
 * listarTransacciones
 * Genera un reporte detallado de todas las transacciones.
 * Utiliza setw para el formateo de columnas, manteniendo la 
 * consistencia visual con el resto de los listados del sistema.
 
void listarTransacciones(Tienda* tienda) {
    if (tienda->numTransacciones == 0) {
        cout << "\nNo hay transacciones registradas." << endl;
        system("pause");
        return;
    }

    cout << "\n================================================================================" << endl;
    cout << "                         LISTADO DE TRANSACCIONES                               " << endl;
    cout << "================================================================================" << endl;
    cout << " ID |  Tipo  | Prod | Relac | Cant |   Total    |    Fecha    " << endl;
    cout << "--------------------------------------------------------------------------------" << endl;

    for (int i = 0; i < tienda->numTransacciones; i++) {
        cout << setw(3) << tienda->transacciones[i].id << " | "
             << setw(6) << tienda->transacciones[i].tipo << " | "
             << setw(4) << tienda->transacciones[i].idProducto << " | "
             << setw(5) << tienda->transacciones[i].idRelacionado << " | "
             << setw(4) << tienda->transacciones[i].cantidad << " | "
             << setw(10) << fixed << tienda->transacciones[i].total << " | " 
             << tienda->transacciones[i].fecha << endl;
    }
    
    cout << "================================================================================" << endl;
    cout << "Total de registros: " << tienda->numTransacciones << endl;

    system("pause");
}

/*
 * cancelarTransaccion
 * Localiza una transaccion por su ID, muestra sus datos para confirmar,
 * revierte el impacto en el stock del producto y elimina el registro 
 * del historial mediante el desplazamiento de elementos.
 
void cancelarTransaccion(Tienda* tienda) {
    // 1. Validacion: Si no hay transacciones, no hay nada que anular
    if (tienda->numTransacciones == 0) {
        cout << "\n El historial de transacciones esta vacio." << endl;
        system("pause");
        return;
    }

    int idBuscado;
    cout << "\n--- ANULAR TRANSACCION ---" << endl;
    cout << "Ingrese el ID de la transaccion: ";
    cin >> idBuscado;

    // Encontrar la posicion (indice) de la transaccion
    int pos = -1;
    for (int i = 0; i < tienda->numTransacciones; i++) {
        if (tienda->transacciones[i].id == idBuscado) {
            pos = i;
            break;
        }
    }

    if (pos == -1) {
        cout << " Error:, El ID ingresado no existe." << endl;
        system("pause");
        return;
    }

    // Para estar seguro que es la correcta
    Transaccion t = tienda->transacciones[pos];
    cout << "\nSe encontro la siguiente transaccion:" << endl;
    cout << "Tipo: " << t.tipo << " | Producto ID: " << t.idProducto 
         << " | Cantidad: " << t.cantidad << endl;
    cout << "Total: " << fixed << t.total << " | Fecha: " << t.fecha << endl;
    
    char confirmacion;
    cout << "\n¿Desea anular esta operacion y revertir el stock? (s/n): ";
    cin >> confirmacion;

    if (confirmacion == 's' || confirmacion == 'S') {
        
        // revertimos stock buscamos el producto en el inventario
        for (int j = 0; j < tienda->numProductos; j++) {
            if (tienda->productos[j].id == t.idProducto) {
                // Si la transaccion fue una COMPRA, el stock subio -> ahora debe bajar
                if (strcmp(t.tipo, "COMPRA") == 0) {
                    tienda->productos[j].stock -= t.cantidad;
                    cout << "-> Stock ajustado: Se restaron " << t.cantidad << " unidades." << endl;
                } 
                // Si fue una VENTA, el stock bajo, ahora debe subir (devolucion)
                else {
                    tienda->productos[j].stock += t.cantidad;
                    cout << "-> Stock ajustado: Se sumaron " << t.cantidad << " unidades." << endl;
                }
                break;
            }
        }

        // Desplazamos los elementos para tapar el hueco
        // Todos los que estan despues de pos se mueven un lugar a la izquierda
        for (int i = pos; i < tienda->numTransacciones - 1; i++) {
            tienda->transacciones[i] = tienda->transacciones[i + 1];
        }
        
        // Reducimos el contador de transacciones
        tienda->numTransacciones--;

        cout << "\n La transaccion ha sido eliminada del historial." << endl;
    } else {
        cout << "\n Operacion cancelada. No se realizaron cambios." << endl;
    }

    system("pause");
}*/


int main() {
    // 1. Ya no usamos Tienda miTienda ni inicializarTienda
    // porque ahora todo vive en "productos.bin"
    
    char opcionPrincipal = ' '; 
    const char* archivo = "productos.bin"; // Nombre de tu base de datos

    do {
        system("cls"); // Limpiar pantalla al inicio de cada ciclo
        cout << "\n########################################################" << endl;
        cout << "#          SISTEMA DE GESTION DE INVENTARIO            #" << endl;
        cout << "#               (VERSION BINARIA 2.0)                  #" << endl;
        cout << "########################################################" << endl;

        cout << "\n1. Gestion de Productos" << endl;
        cout << "2. Gestion de Proveedores (PROXIMAMENTE)" << endl;
        cout << "3. Gestion de Clientes (PROXIMAMENTE)" << endl;
        cout << "4. Gestion de Transacciones (PROXIMAMENTE)" << endl;
        cout << "5. Salir" << endl;
        cout << "\nSeleccione una opcion: ";
        cin >> opcionPrincipal;

        switch (opcionPrincipal) {
            case '1': {
                int opP = -1;
                do {
                    cout << "\n--- GESTION DE PRODUCTOS (ACCESO ALEATORIO) ---" << endl;
                    cout << "1. Registrar (Append)" << endl;
                    cout << "2. Listar Todo" << endl;
                    cout << "3. Editar Precio (Update)" << endl;
                    cout << "4. Eliminar (Borrado Logico)" << endl;
                    cout << "0. Volver" << endl;
                    cout << "Seleccione: ";
                    cin >> opP;

                    if(opP == 1) {
                        crearProducto(); // Ya no pasas &miTienda
                    } 
                    else if(opP == 2) {
                        mostrarInventario(archivo); // Usa la que hicimos hoy
                    }
                    else if(opP == 3) {
                        // Llamas a tu interfaz de actualizar
                        int id; float precio;
                        cout << "ID a editar: "; cin >> id;
                        cout << "Nuevo precio: "; cin >> precio;
                        actualizarPrecioProducto(archivo, id, precio);
                    }
                    else if(opP == 4) {
                        // Llamas a tu interfaz de eliminar
                        int id;
                        cout << "ID a eliminar: "; cin >> id;
                        eliminarProductoLogico(archivo, id);
                    }
                    
                    if(opP != 0) {
                        cout << "\nPresione una tecla para continuar...";
                        cin.ignore(); cin.get();
                        system("cls");
                    }

                } while (opP != 0);
                break;
            }
            case '5':
                cout << "Cerrando sistema y guardando archivos binarios..." << endl;
                break;
            default:
                cout << "Opcion no valida." << endl;
                break;
        }
    } while (opcionPrincipal != '5');

    // Ya no necesitas liberarTienda(&miTienda);
    return 0;
}