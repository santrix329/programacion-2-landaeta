#include <iostream>
#include <cstring>
#include <string>
#include <iomanip>
#include <ctime>
#include <cstdlib>
#include <cctype>
#include <fstream>
#include <cstddef>    // For offsetof macro
#include <climits>    // INT_MAX, INT_MIN

using namespace std;

// Colores para la consola (ANSI escape codes)

#define RST    "\033[0m"    // Reset / Normal
#define NEG    "\033[1m"    // Negrita
#define ROJO   "\033[31m"   // Errores, alertas criticas
#define VERDE  "\033[32m"   // Stock saludable, mensajes de exito
#define AMARI  "\033[33m"   // Advertencias, alertas de stock bajo, mensajes informativos
#define AZUL   "\033[34m"   // Títulos, menus principales
#define MAGEN  "\033[35m"   // Destacado, resaltar informacion importante
#define CIAN   "\033[36m"   // Detalles, informacion secundaria
#define FVRD   "\033[42m"   // Fondo verde - exito, operacion realizada, nuevo registro creado
#define FROJ   "\033[41m"   // Fondo rojo - error critico, fallo al guardar, datos invalidos

// Estructuras de datos
struct ArchivoHeader {
    int cantidadRegistros;   // Total historico (incluye borrados)  
    int proximoID;           // El ID que se asignara al proximo registro creado
    int registrosActivos;    // Cuantos registros no eliminados hay actualmente
    int version;             // Para futuras actualizaciones del formato del archivo
};

// Producto - mercancia que vendemos. Se guarda en productos.bin
struct Producto {
    int    id;
    char   codigo[20];
    char   nombre[100];
    char   descripcion[200];
    int    idProveedor;              // Llave foranea -> proveedores.bin
    float  precio;
    int    stock;
    int    stockMinimo;              // Umbral de alerta de stock critico
    int    totalVendido;             // Acumulado historico de ventas
    bool   eliminado;                // Flag de borrado logico
    time_t fechaCreacion;
    time_t fechaUltimaModificacion;
    int    historialVentas[50];      // IDs de transacciones del producto
    int    cantidadVentas;           // Cuantos slots del historial usados
};

// Proveedor - empresa que suministra productos. Se guarda en proveedores.bin
struct Proveedor {
    int    id;
    char   nombre[100];
    char   rif[20];
    char   telefono[20];
    char   email[100];
    char   direccion[200];
    bool   eliminado;
    time_t fechaRegistro;
};

// Cliente - persona que nos compra mercancia
struct Cliente {
    int    id;
    char   nombre[100];
    char   cedula[20];
    char   telefono[20];
    char   email[100];
    char   direccion[200];
    bool   eliminado;
    time_t fechaRegistro;
    float  totalGastado;             // Suma acumulada de todas sus compras   
    int    historialTransacciones[50]; // IDs de ventas vinculadas al cliente 
    int    numTransacciones;           // Cuantos slots del historial usados   
};

// Transaccion - representa una compra o venta. Se guarda en transacciones.bin
struct Transaccion {
    int    id;
    char   tipo[10];         // "COMPRA" o "VENTA"                       
    int    idRelacionado;    // ID del cliente (VENTA) o proveedor (COMPRA)
    int    idsProductos[15]; // IDs de los productos involucrados         
    int    cantidades[15];   // Cantidad de cada producto                 
    int    numItems;         // Cuantos de los 15 slots se usan           
    float  totalFactura;
    time_t fecha;
    char   observaciones[200];
    bool   eliminado;
};

// Tienda - informacion general de la tienda, se guarda en tienda.bin. Solo hay un registro.
struct Tienda {
    char  nombre[100];
    char  rif[20];
    float ventasTotales;
    float comprasTotales;
    int   totalTransaccionesRealizadas;
};

// Prototipos de funciones

int       buscarIndiceProductoPorID(int id);
int       buscarIndiceProveedorPorID(int id);
int       buscarIndiceClientePorID(int id);
int       buscarIndiceTransaccionPorID(int id);
Producto  leerProductoPorIndice(int indice);
Proveedor leerProveedorPorIndice(int indice);
Cliente   leerClientePorIndice(int indice);
bool      existeProveedor(int id);
bool      existeCliente(int id);
bool      codigoDuplicado(const char* codigo);
bool      rifDuplicado(const char* rif);
bool      cedulaDuplicada(const char* cedula);
void      imprimirProductoDetallado(Producto p);
void      imprimirProveedorDetallado(Proveedor p);
void      imprimirClienteDetallado(Cliente c);
void      imprimirLinea(char c, int ancho);
void      pausar();
int       leerEntero(const char* mensaje, int minimo, int maximo);
float     leerFlotante(const char* mensaje, float minimo);
void      limpiarBuffer();
void      actualizarProductoEnDisco(int indice, Producto p);
void      actualizarProveedorEnDisco(int indice, Proveedor p);
void      actualizarClienteEnDisco(int indice, Cliente c);

// Lee el header del archivo binario. Si el archivo no existe, lo crea con un header en cero y devuelve ese header inicial.
bool inicializarArchivo(const char* nombreArchivo) {

    // Intentamos abrirlo en lectura para ver si existe
    ifstream prueba(nombreArchivo, ios::binary);

    if (prueba.good()) {
        // Ya existe, cerramos y salimos sin hacer nada
        prueba.close();
        return true;
    }
    prueba.close();

    // El archivo no existe, lo creamos con su header inicial
    ofstream nuevo(nombreArchivo, ios::binary);

    if (!nuevo.is_open()) {
        cout << ROJO << "  [ERROR CRITICO] No se pudo crear: "
             << nombreArchivo << RST << endl;
        return false;
    }

    // Header inicial con todos los contadores en cero
    ArchivoHeader h;
    h.cantidadRegistros = 0;
    h.proximoID         = 1;   // IDs arrancan desde 1
    h.registrosActivos  = 0;
    h.version           = 1;

    nuevo.write(reinterpret_cast<char*>(&h), sizeof(ArchivoHeader));
    nuevo.close();

    return true;
}

 //Abre el archivo, lee el header y lo devuelve. Si el archivo no existe, lo crea con un header en cero y devuelve ese header inicializado.
ArchivoHeader leerHeader(const char* nombreArchivo) {
    // Valores seguros por defecto
    ArchivoHeader h;
    h.cantidadRegistros = 0;
    h.proximoID         = 1;
    h.registrosActivos  = 0;
    h.version           = 1;

    ifstream archivo(nombreArchivo, ios::binary);

    if (!archivo.is_open()) {
        inicializarArchivo(nombreArchivo);
    return h;
    }
    archivo.read(reinterpret_cast<char*>(&h), sizeof(ArchivoHeader));
    archivo.close();

    return h;
 }

// Abre el archivo, se posiciona al inicio y reescribe el header con los datos del struct proporcionado. Retorna true si tuvo exito o false si hubo un error.
bool actualizarHeader(const char* nombreArchivo, ArchivoHeader h) {

    // ios::in|ios::out evita que se borre el contenido del archivo
    fstream archivo(nombreArchivo, ios::binary | ios::in | ios::out);

    if (!archivo.is_open()) {
        return false;
    }

    archivo.seekp(0, ios::beg); // Inicio absoluto del archivo
    archivo.write(reinterpret_cast<char*>(&h), sizeof(ArchivoHeader));
    archivo.close();

    return true;
}

long calcularOffset(int indice, size_t tamanoEstructura) {
    long bytesDelHeader   = (long)sizeof(ArchivoHeader);
    long bytesHastaElSlot = (long)indice * (long)tamanoEstructura;
    return bytesDelHeader + bytesHastaElSlot;
}

int buscarIndiceProductoPorID(int idBuscado) {
    ArchivoHeader h = leerHeader("productos.bin");
    ifstream f("productos.bin", ios::binary);
    if (!f.is_open()) return -1;

    Producto p; // Usamos el struct directamente
    for (int i = 0; i < h.cantidadRegistros; i++) {
        f.seekg(calcularOffset(i, sizeof(Producto)), ios::beg);
        f.read(reinterpret_cast<char*>(&p), sizeof(Producto));

        if (p.id == idBuscado && !p.eliminado) {
            f.close();
            return i;
        }
    }
    f.close();
    return -1;
}

int buscarIndiceProveedorPorID(int idBuscado) {
    ArchivoHeader h = leerHeader("proveedores.bin");
    ifstream f("proveedores.bin", ios::binary);
    if (!f.is_open()) return -1;

    Proveedor p; 
    for (int i = 0; i < h.cantidadRegistros; i++) {
        f.seekg(calcularOffset(i, sizeof(Proveedor)), ios::beg);
        f.read(reinterpret_cast<char*>(&p), sizeof(Proveedor));

        if (p.id == idBuscado && !p.eliminado) {
            f.close();
            return i;
        }
    }
    f.close();
    return -1;
}

int buscarIndiceClientePorID(int idBuscado) {
    ArchivoHeader h = leerHeader("clientes.bin");
    ifstream f("clientes.bin", ios::binary);
    if (!f.is_open()) return -1;

    Cliente c; 
    for (int i = 0; i < h.cantidadRegistros; i++) {
        f.seekg(calcularOffset(i, sizeof(Cliente)), ios::beg);
        f.read(reinterpret_cast<char*>(&c), sizeof(Cliente));

        if (c.id == idBuscado && !c.eliminado) {
            f.close();
            return i;
        }
    }
    f.close();
    return -1;
}

int buscarIndiceTransaccionPorID(int idBuscado) {
    ArchivoHeader h = leerHeader("transacciones.bin");
    ifstream f("transacciones.bin", ios::binary);
    if (!f.is_open()) return -1;

    Transaccion t; 
    for (int i = 0; i < h.cantidadRegistros; i++) {
        f.seekg(calcularOffset(i, sizeof(Transaccion)), ios::beg);
        f.read(reinterpret_cast<char*>(&t), sizeof(Transaccion));

        if (t.id == idBuscado && !t.eliminado) {
            f.close();
            return i;
        }
    }
    f.close();
    return -1;
}

// Similar a buscarIndiceProductoPorID pero devuelve el struct completo del producto encontrado. Si no encuentra nada, devuelve un struct con id=0 (no confundir con id=-1 que es un indice invalido).

Producto leerProductoPorIndice(int indice) {
    Producto p;
    memset(&p, 0, sizeof(Producto)); // Limpiamos para evitar basura

    ifstream f("productos.bin", ios::binary);
    if (f.is_open()) {
        f.seekg(calcularOffset(indice, sizeof(Producto)), ios::beg);
        f.read(reinterpret_cast<char*>(&p), sizeof(Producto));
        f.close();
    }
    return p;
}

Proveedor leerProveedorPorIndice(int indice) {
    Proveedor p;
    memset(&p, 0, sizeof(Proveedor));

    ifstream f("proveedores.bin", ios::binary);
    if (f.is_open()) {
        f.seekg(calcularOffset(indice, sizeof(Proveedor)), ios::beg);
        f.read(reinterpret_cast<char*>(&p), sizeof(Proveedor));
        f.close();
    }
    return p;
}

Cliente leerClientePorIndice(int indice) {
    Cliente c;
    memset(&c, 0, sizeof(Cliente));

    ifstream f("clientes.bin", ios::binary);
    if (f.is_open()) {
        f.seekg(calcularOffset(indice, sizeof(Cliente)), ios::beg);
        f.read(reinterpret_cast<char*>(&c), sizeof(Cliente));
        f.close();
    }
    return c;
}

// Similar a actualizarHeader pero para un registro de producto, proveedor o cliente. Reescribe SOLO ese registro sin tocar el resto del archivo.

void actualizarProductoEnDisco(int indice, Producto p) {
    // Actualizamos la fecha de modificacion antes de guardar
    p.fechaUltimaModificacion = time(0);

    fstream f("productos.bin", ios::binary | ios::in | ios::out);
    if (f.is_open()) {
        f.seekp(calcularOffset(indice, sizeof(Producto)), ios::beg);
        f.write(reinterpret_cast<char*>(&p), sizeof(Producto));
        f.close();
    } else {
        cout << ROJO << "No se pudo abrir productos.bin para guardar."
             << RST << endl;
    }
}

void actualizarProveedorEnDisco(int indice, Proveedor p) {
    fstream f("proveedores.bin", ios::binary | ios::in | ios::out);
    if (f.is_open()) {
        f.seekp(calcularOffset(indice, sizeof(Proveedor)), ios::beg);
        f.write(reinterpret_cast<char*>(&p), sizeof(Proveedor));
        f.close();
    } else {
        cout << ROJO << "No se pudo abrir proveedores.bin para guardar."
             << RST << endl;
    }
}

void actualizarClienteEnDisco(int indice, Cliente c) {
    fstream f("clientes.bin", ios::binary | ios::in | ios::out);
    if (f.is_open()) {
        f.seekp(calcularOffset(indice, sizeof(Cliente)), ios::beg);
        f.write(reinterpret_cast<char*>(&c), sizeof(Cliente));
        f.close();
    } else {
        cout << ROJO << "No se pudo abrir clientes.bin para guardar."
             << RST << endl;
    }
}

// Similar a registrarProveedor pero para productos. Inicializa tambien los campos relacionados a las ventas del producto.

bool registrarProducto(Producto nuevo) {

    ArchivoHeader h = leerHeader("productos.bin");

    // Asignamos el ID y los valores que van por defecto
    nuevo.id                      = h.proximoID;
    nuevo.eliminado               = false;
    nuevo.fechaCreacion           = time(0);
    nuevo.fechaUltimaModificacion = time(0);
    nuevo.totalVendido            = 0;
    nuevo.cantidadVentas          = 0;

    // Inicializamos el historial de ventas con ceros
    for (int i = 0; i < 50; i++) {
        nuevo.historialVentas[i] = 0;
    }

    // Calculamos el offset del siguiente slot disponible
    long posNuevo = calcularOffset(h.cantidadRegistros, sizeof(Producto));

    fstream f("productos.bin", ios::binary | ios::in | ios::out);
    if (!f.is_open()) {
        cout << ROJO << "No se pudo abrir productos.bin para registrar."
             << RST << endl;
        return false;
    }
    cout << FVRD << " Registrado con exito. ID asignado:  "  << nuevo.id << RST << endl;
    f.seekp(posNuevo, ios::beg);
    f.write(reinterpret_cast<char*>(&nuevo), sizeof(Producto));
    f.close();

    h.cantidadRegistros++;
    h.proximoID++;
    h.registrosActivos++;

    return actualizarHeader("productos.bin", h);
}

bool registrarProveedor(Proveedor nuevo) {

    ArchivoHeader h = leerHeader("proveedores.bin");

    nuevo.id            = h.proximoID;
    nuevo.eliminado     = false;
    nuevo.fechaRegistro = time(0);

    long posNuevo = calcularOffset(h.cantidadRegistros, sizeof(Proveedor));

    fstream f("proveedores.bin", ios::binary | ios::in | ios::out);
    if (!f.is_open()) {
        cout << ROJO << "No se pudo abrir proveedores.bin para registrar."
             << RST << endl;
        return false;
    }
    cout << FVRD << " Registrado con exito. ID asignado:  "  << nuevo.id << RST << endl;
    f.seekp(posNuevo, ios::beg);
    f.write(reinterpret_cast<char*>(&nuevo), sizeof(Proveedor));
    f.close();

    h.cantidadRegistros++;
    h.proximoID++;
    h.registrosActivos++;

    return actualizarHeader("proveedores.bin", h);
}

// Similar a registrarProveedor pero para clientes. Inicializa tambien los campos relacionados a las transacciones del cliente.
bool registrarCliente(Cliente nuevo) {

    ArchivoHeader h = leerHeader("clientes.bin");

    nuevo.id               = h.proximoID;
    nuevo.eliminado        = false;
    nuevo.fechaRegistro    = time(0);
    nuevo.totalGastado     = 0.0f;
    nuevo.numTransacciones = 0;

    // Limpiamos el historial de transacciones
    for (int i = 0; i < 50; i++) {
        nuevo.historialTransacciones[i] = 0;
    }

    long posNuevo = calcularOffset(h.cantidadRegistros, sizeof(Cliente));

    fstream f("clientes.bin", ios::binary | ios::in | ios::out);
    if (!f.is_open()) {
        cout << ROJO << "No se pudo abrir clientes.bin para registrar."
             << RST << endl;
        return false;
    }
    cout << FVRD << " Registrado con exito. ID asignado:  "  << nuevo.id << RST << endl;
    f.seekp(posNuevo, ios::beg);
    f.write(reinterpret_cast<char*>(&nuevo), sizeof(Cliente));
    f.close();

    h.cantidadRegistros++;
    h.proximoID++;
    h.registrosActivos++;

    return actualizarHeader("clientes.bin", h);
}

// Retorna el ID de la transaccion registrada, o -1 si hubo un error.
int registrarTransaccion(Transaccion nueva) {

    ArchivoHeader h = leerHeader("transacciones.bin");

    nueva.id        = h.proximoID;
    nueva.eliminado = false;
    nueva.fecha     = time(0);

    long posNueva = calcularOffset(h.cantidadRegistros, sizeof(Transaccion));

    fstream f("transacciones.bin", ios::binary | ios::in | ios::out);
    if (!f.is_open()) {
        cout << ROJO << "No se pudo abrir transacciones.bin para registrar."
             << RST << endl;
        return -1;
    }

    f.seekp(posNueva, ios::beg);
    f.write(reinterpret_cast<char*>(&nueva), sizeof(Transaccion));
    f.close();

    h.cantidadRegistros++;
    h.proximoID++;
    h.registrosActivos++;

    actualizarHeader("transacciones.bin", h);

    return nueva.id; // Retornamos el ID para el historial del cliente 
}

 
void limpiarBuffer() {
    cin.clear();               // Limpiamos la bandera de error            
    cin.ignore(INT_MAX, '\n'); // Descartamos hasta el proximo Enter 
}

// Lee un numero entero dentro de un rango definido. Repite hasta que el usuario ingrese algo valido.
int leerEntero(const char* mensaje, int minimo, int maximo) {
    int valor;

    while (true) {
        cout << mensaje;
        cin >> valor;

        if (cin.fail()) {
            cout << ROJO << "  Error: ingrese un numero entero valido."
                 << RST << endl;
            limpiarBuffer();
            continue;
        }

        limpiarBuffer();

        if (valor < minimo || valor > maximo) {
            cout << ROJO << "  Error: el valor debe estar entre "
                 << minimo << " y " << maximo << "."
                 << RST << endl;
            return 0;
        }

        return valor;
    }
}

// Similar a leerEntero pero para numeros con decimales. El maximo es opcional.
float leerFlotante(const char* mensaje, float minimo) {
    float valor;

    while (true) {
        cout << mensaje;
        cin >> valor;

        if (cin.fail()) {
            cout << ROJO << "  Error: ingrese un numero valido." << RST << endl;
            limpiarBuffer();
            continue;
        }

        limpiarBuffer();

        if (valor < minimo) {
            cout << ROJO << "  Error: el valor no puede ser menor a "
                 << minimo << "." << RST << endl;
            continue;
        }

        return valor;
    }
}

// Convierte una cadena a minusculas in-place para facilitar comparaciones case-insensitive.
void convertirAMinusculas(char* cadena) {
    for (int i = 0; cadena[i] != '\0'; i++) {
        cadena[i] = (char)tolower((unsigned char)cadena[i]);
    }
}

// Verifica si 'busqueda' es substring de 'texto' (case-insensitive). Retorna true o false.
bool contieneSubstring(const char* texto, const char* busqueda) {
    if (!texto || !busqueda) {
        return false;
    }
    return strstr(texto, busqueda) != NULL;
}

// Obtiene la fecha actual en formato "YYYY-MM-DD" y la guarda en el buffer proporcionado (debe tener al menos 11 bytes).
void obtenerFechaActual(char* buffer) {
    time_t t        = time(0);
    struct tm* info = localtime(&t);
    strftime(buffer, 11, "%Y-%m-%d", info);
}

// Valida que el email tenga un formato basico con '@' y '.' en posiciones razonables. No es una validacion exhaustiva pero ayuda a evitar errores tipicos.
bool validarEmail(const char* email) {
    if (!email || strlen(email) < 5) return false;

    const char* arroba = strchr(email, '@');
    if (!arroba || arroba == email) return false;

    const char* punto = strchr(arroba, '.');
    if (!punto || punto == arroba + 1) return false;
    if (*(punto + 1) == '\0') return false;

    return true;
}

 // Busca en productos.bin si ya existe un producto activo con ese codigo.

bool codigoDuplicado(const char* cod) {
    ArchivoHeader h = leerHeader("productos.bin");
    Producto p;

    ifstream f("productos.bin", ios::binary);
    if (!f.is_open()) return false;

    for (int i = 0; i < h.cantidadRegistros; i++) {
        f.seekg(calcularOffset(i, sizeof(Producto)), ios::beg);
        f.read(reinterpret_cast<char*>(&p), sizeof(Producto));
        if (!p.eliminado && strcmp(p.codigo, cod) == 0) {
            f.close();
            return true;
        }
    }

    f.close();
    return false;
}


  //Verifica si ya hay un proveedor activo con ese RIF.
 
bool rifDuplicado(const char* rif) {
    ArchivoHeader h = leerHeader("proveedores.bin");
    Proveedor p;

    ifstream f("proveedores.bin", ios::binary);
    if (!f.is_open()) return false;

    for (int i = 0; i < h.cantidadRegistros; i++) {
        f.seekg(calcularOffset(i, sizeof(Proveedor)), ios::beg);
        f.read(reinterpret_cast<char*>(&p), sizeof(Proveedor));
        if (!p.eliminado && strcmp(p.rif, rif) == 0) {
            f.close();
            return true;
        }
    }

    f.close();
    return false;
}

  //Verifica si ya hay un cliente activo con esa cedula.
 
bool cedulaDuplicada(const char* cedula) {
    ArchivoHeader h = leerHeader("clientes.bin");
    Cliente c;

    ifstream f("clientes.bin", ios::binary);
    if (!f.is_open()) return false;

    for (int i = 0; i < h.cantidadRegistros; i++) {
        f.seekg(calcularOffset(i, sizeof(Cliente)), ios::beg);
        f.read(reinterpret_cast<char*>(&c), sizeof(Cliente));
        if (!c.eliminado && strcmp(c.cedula, cedula) == 0) {
            f.close();
            return true;
        }
    }

    f.close();
    return false;
}

/* Funciones helper que usan los wrappers de busqueda */
bool existeProveedor(int id) { return buscarIndiceProveedorPorID(id) != -1; }
bool existeCliente(int id)   { return buscarIndiceClientePorID(id)   != -1; }



  // Imprime 'ancho' repeticiones del caracter dado y luego un salto.
 
void imprimirLinea(char c, int ancho) {
    for (int i = 0; i < ancho; i++) cout << c;
    cout << endl;
}


 //Imprime un titulo centrado entre lineas de '=' con colores.
 
void imprimirTitulo(const char* titulo) {
    cout << endl << AZUL << NEG;
    imprimirLinea('=', 65);

    int lon = (int)strlen(titulo);
    int sp  = (63 - lon) / 2;
    for (int i = 0; i < sp; i++) cout << ' ';
    cout << titulo << endl;

    imprimirLinea('=', 65);
    cout << RST;
}


  //Espera que el usuario presione ENTER. Reemplaza system("pause")
 
void pausar() {
    cout << endl << AMARI << "  Presione ENTER para continuar..." << RST;
    cin.ignore(INT_MAX, '\n');
    cin.get();
}

/*
 * En lugar de mostrar solo el ID del proveedor, busca su nombre
 * en proveedores.bin para que sea mas informativo.
 */
void imprimirProductoDetallado(Producto p) {

    /* Buscamos el nombre del proveedor para mostrarlo */
    char nomProv[100] = "Desconocido";
    int idxProv = buscarIndiceProveedorPorID(p.idProveedor);
    if (idxProv != -1) {
        Proveedor pr = leerProveedorPorIndice(idxProv);
        strcpy(nomProv, pr.nombre);
    }

    /* Color del stock segun nivel */
    const char* colorStock = VERDE;
    const char* etqStock   = "";
    if      (p.stock <= p.stockMinimo)     { colorStock = ROJO;  etqStock = " [CRITICO]"; }
    else if (p.stock <= p.stockMinimo * 2) { colorStock = AMARI; etqStock = " [BAJO]";    }

    cout << CIAN  << "  ID:          " << RST << NEG << p.id  << RST
         << "    " << CIAN << "Codigo: " << RST << p.codigo   << endl;
    cout << CIAN  << "  Nombre:      " << RST << NEG << p.nombre << RST << endl;
    cout << CIAN  << "  Descripcion: " << RST << p.descripcion   << endl;
    cout << CIAN  << "  Precio:      " << RST << AMARI
         << "$" << fixed << setprecision(2) << p.precio << RST;
    cout << "    " << CIAN << "Stock: " << RST
         << colorStock << p.stock << etqStock << RST;
    cout << "    " << CIAN << "Minimo: " << RST << p.stockMinimo << endl;
    cout << CIAN  << "  Proveedor:   " << RST
         << VERDE << nomProv << RST << "  (ID: " << p.idProveedor << ")" << endl;
    imprimirLinea('-', 65);
}

// Similar a imprimirProductoDetallado pero para proveedores. Muestra el RIF en la misma linea que el ID para ahorrar espacio.
void imprimirProveedorDetallado(Proveedor p) {
    cout << CIAN  << "  ID:        " << RST << NEG << p.id << RST
         << "    " << CIAN << "RIF: " << RST << p.rif << endl;
    cout << CIAN  << "  Nombre:    " << RST << NEG << p.nombre << RST << endl;
    cout << CIAN  << "  Telefono:  " << RST << p.telefono
         << "    " << CIAN << "Email: " << RST << p.email << endl;
    cout << CIAN  << "  Direccion: " << RST << p.direccion << endl;
    imprimirLinea('-', 65);
}

// Similar a imprimirProveedorDetallado pero para clientes. Muestra la cedula en la misma linea que el ID para ahorrar espacio. Agrega tambien el total gastado y numero de transacciones para dar una idea del valor del cliente.
void imprimirClienteDetallado(Cliente c) {
    cout << CIAN  << "  ID:            " << RST << NEG << c.id << RST
         << "    " << CIAN << "Cedula: " << RST << c.cedula << endl;
    cout << CIAN  << "  Nombre:        " << RST << NEG << c.nombre << RST << endl;
    cout << CIAN  << "  Telefono:      " << RST << c.telefono
         << "    " << CIAN << "Email: " << RST << c.email << endl;
    cout << CIAN  << "  Direccion:     " << RST << c.direccion << endl;
    cout << CIAN  << "  Total Gastado: " << RST << AMARI
         << "$" << fixed << setprecision(2) << c.totalGastado << RST;
    cout << "    " << CIAN << "Transacciones: " << RST << c.numTransacciones << endl;
    imprimirLinea('-', 65);
}


 // Solicita todos los datos, los valida y los guarda en productos.bin.
 
void crearProducto() {

    // preguntamos si quiere registrar un nuevo producto. Si responde 'n' o cualquier cosa que no sea 's', se cancela la operacion.
    char respInicial;
    cout << endl << "  Desea registrar un nuevo producto? (S/N): ";
    cin >> respInicial;
    limpiarBuffer();

    if (tolower(respInicial) != 's') {
        cout << AMARI << "  Registro cancelado." << RST << endl;
        return;
    }

    Producto nuevo;
    memset(&nuevo, 0, sizeof(Producto));

    // Nombre del producto, validando que no este vacio. Si el usuario escribe "cancelar", se cancela toda la operacion.
    cout << endl;
    cout << "  Nombre del producto (o escriba 'cancelar'): ";
    cin.getline(nuevo.nombre, 100);

    if (strcmp(nuevo.nombre, "cancelar") == 0) {
        cout << AMARI << "  Operacion cancelada." << RST << endl;
        return;
    }

    if (strlen(nuevo.nombre) == 0) {
        cout << ROJO << "  Error: el nombre no puede estar vacio." << RST << endl;
        return;
    }

    //codigo del producto, validando que no este vacio ni duplicado. Repite hasta que el usuario ingrese algo valido o cancele.
    bool codigoBueno = false;
    do {
        cout << "  Codigo del producto: ";
        cin >> nuevo.codigo;
        limpiarBuffer();

        if (strlen(nuevo.codigo) == 0) {
            cout << ROJO << "  Error: el codigo no puede estar vacio." << RST << endl;
        } else if (codigoDuplicado(nuevo.codigo)) {
            cout << ROJO << "  Error: el codigo '" << nuevo.codigo
                 << "' ya existe. Use otro." << RST << endl;
        } else {
            codigoBueno = true;
        }
    } while (!codigoBueno);

    // Descripcion del producto, validando que no este vacia. Si el usuario escribe "cancelar", se cancela toda la operacion.
    cout << "  Descripcion: ";
    cin.getline(nuevo.descripcion, 200);

    // Precio del producto, validando que sea un numero positivo. Repite hasta que el usuario ingrese algo valido o cancele.
    nuevo.precio = leerFlotante("  Precio (mayor que 0): $", 0.01f);

    // Stock inicial, validando que sea un numero entero no negativo. Repite hasta que el usuario ingrese algo valido o cancele.
    nuevo.stock = leerEntero("  Stock inicial (0 o mas): ", 0, INT_MAX);

    // Stock minimo de alerta, validando que sea un numero entero no negativo. Repite hasta que el usuario ingrese algo valido o cancele.
    nuevo.stockMinimo = leerEntero("  Stock minimo de alerta: ", 0, INT_MAX);

    // ID del proveedor, validando que exista un proveedor con ese ID. Repite hasta que el usuario ingrese algo valido o cancele.
    bool provBueno = false;
    do {
        nuevo.idProveedor = leerEntero("  ID del Proveedor: ", 1, INT_MAX);
        if (!existeProveedor(nuevo.idProveedor)) {
            cout << ROJO << "  Error: no existe un proveedor con el ID "
                 << nuevo.idProveedor << ". Verifique e intente de nuevo."
                 << RST << endl;
                 return;
        } else {
            provBueno = true;
        }
    } while (!provBueno);

    // Leemos el proveedor elegido para mostrar su nombre en el resumen final.
    int idxProv = buscarIndiceProveedorPorID(nuevo.idProveedor);
    Proveedor provElegido = leerProveedorPorIndice(idxProv);

    // Imprimimos un resumen de toda la informacion ingresada para que el usuario pueda confirmarla antes de guardarla definitivamente.
    cout << endl;
    imprimirTitulo("RESUMEN DEL NUEVO PRODUCTO");
    cout << "  Nombre:       " << NEG << nuevo.nombre << RST << endl;
    cout << "  Codigo:       " << nuevo.codigo        << endl;
    cout << "  Descripcion:  " << nuevo.descripcion   << endl;
    cout << "  Precio:       $" << fixed << setprecision(2) << nuevo.precio << endl;
    cout << "  Stock:        " << nuevo.stock
         << "  (minimo de alerta: " << nuevo.stockMinimo << ")" << endl;
    cout << "  Proveedor:    " << VERDE << provElegido.nombre
         << RST << "  (ID: " << nuevo.idProveedor << ")" << endl;
    imprimirLinea('=', 65);

    char guardar;
    cout << "  Guardar en productos.bin? (S/N): ";
    cin >> guardar;
    limpiarBuffer();

    if (tolower(guardar) == 's') {
        if (registrarProducto(nuevo)) {
            cout << FVRD << "   Producto guardado " << RST << endl;
        } else {
            cout << FROJ << "  Error: no se pudo escribir en el disco."
                 << RST << endl;
        }
    } else {
        cout << AMARI << "  Producto descartado, no se guardo nada." << RST << endl;
    }
}

// Adaptacion de buscarProducto() del original, con opciones de busqueda y sin arreglos dinamicos.

void buscarProducto() {

    ArchivoHeader h = leerHeader("productos.bin");
    if (h.registrosActivos == 0) {
        cout << AMARI << "  No hay productos registrados para buscar." << RST << endl;
        return;
    }

    imprimirTitulo("MENU DE BUSQUEDA DE PRODUCTOS");
    cout << "  " << CIAN << "1." << RST << " Buscar por ID (exacto)"           << endl;
    cout << "  " << CIAN << "2." << RST << " Buscar por nombre (parcial)"       << endl;
    cout << "  " << CIAN << "3." << RST << " Buscar por codigo (parcial)"       << endl;
    cout << "  " << CIAN << "4." << RST << " Listar todos los de un proveedor"  << endl;
    cout << "  " << ROJO  << "0." << RST << " Cancelar"                         << endl;

    int op = leerEntero("  Seleccione: ", 0, 4);
    if (op == 0) return;

    bool encontrado = false;
    Producto p;

   // opcion 1: Por ID exacto
    if (op == 1) {
        int id = leerEntero("  ID del producto: ", 1, INT_MAX);
        int idx = buscarIndiceProductoPorID(id);
        if (idx != -1) {
            cout << endl;
            imprimirProductoDetallado(leerProductoPorIndice(idx));
            encontrado = true;
        }
    }

    //opcion 2: Por nombre parcial (case-insensitive)
    else if (op == 2) {
        char busq[100];
        cout << "  Nombre (o parte del nombre) a buscar: ";
        cin.getline(busq, 100);

        if (strlen(busq) == 0) {
            cout << ROJO << "  Error: el campo no puede estar vacio." << RST << endl;
            return;
        }

        // Convertimos la busqueda a minusculas para comparaciones case-insensitive, sin alterar el original que se muestra en los resultados.
        char busqMin[100];
        strcpy(busqMin, busq);
        convertirAMinusculas(busqMin);

        cout << endl;
        ifstream f("productos.bin", ios::binary);
        if (!f.is_open()) {
            cout << ROJO << "  Error al abrir el archivo." << RST << endl;
            return;
        }

        for (int i = 0; i < h.cantidadRegistros; i++) {
            f.seekg(calcularOffset(i, sizeof(Producto)), ios::beg);
            f.read(reinterpret_cast<char*>(&p), sizeof(Producto));

            if (p.eliminado) continue;

            // Copiamos el nombre en minusculas sin alterar el original
            char nombreMin[100];
            strcpy(nombreMin, p.nombre);
            convertirAMinusculas(nombreMin);

            if (contieneSubstring(nombreMin, busqMin)) {
                imprimirProductoDetallado(p);
                encontrado = true;
            }
        }
        f.close();
    }

    //opcion 3: Por codigo parcial (case-insensitive)
    else if (op == 3) {
        char busq[20];
        cout << "  Codigo (o parte del codigo) a buscar: ";
        cin >> busq;
        limpiarBuffer();

        char busqMin[20];
        strcpy(busqMin, busq);
        convertirAMinusculas(busqMin);

        cout << endl;
        ifstream f("productos.bin", ios::binary);
        if (!f.is_open()) {
            cout << ROJO << "  Error al abrir el archivo." << RST << endl;
            return;
        }

        for (int i = 0; i < h.cantidadRegistros; i++) {
            f.seekg(calcularOffset(i, sizeof(Producto)), ios::beg);
            f.read(reinterpret_cast<char*>(&p), sizeof(Producto));

            if (p.eliminado) continue;

            char codMin[20];
            strcpy(codMin, p.codigo);
            convertirAMinusculas(codMin);

            if (contieneSubstring(codMin, busqMin)) {
                imprimirProductoDetallado(p);
                encontrado = true;
            }
        }
        f.close();
    }

    //opcion 4: Listar por proveedor
    else if (op == 4) {
        int idProv = leerEntero("  ID del proveedor a filtrar: ", 1, INT_MAX);

        if (!existeProveedor(idProv)) {
            cout << ROJO << "  Error: no existe ese proveedor." << RST << endl;
            return;
        }

        int idxPr = buscarIndiceProveedorPorID(idProv);
        Proveedor pr = leerProveedorPorIndice(idxPr);

        cout << endl << AZUL << "  Productos del proveedor: "
             << NEG << pr.nombre << RST << endl << endl;

        ifstream f("productos.bin", ios::binary);
        if (!f.is_open()) {
            cout << ROJO << "  Error al abrir el archivo." << RST << endl;
            return;
        }

        for (int i = 0; i < h.cantidadRegistros; i++) {
            f.seekg(calcularOffset(i, sizeof(Producto)), ios::beg);
            f.read(reinterpret_cast<char*>(&p), sizeof(Producto));

            if (!p.eliminado && p.idProveedor == idProv) {
                imprimirProductoDetallado(p);
                encontrado = true;
            }
        }
        f.close();
    }

    if (!encontrado) {
        cout << AMARI << "  No se encontraron resultados para esa busqueda."
             << RST << endl;
    }
}

// Adaptacion de buscarProducto() del original, con opciones de busqueda y sin arreglos dinamicos.

void actualizarProducto() {

    ArchivoHeader h = leerHeader("productos.bin");
    if (h.registrosActivos == 0) {
        cout << AMARI << "  No hay productos para editar." << RST << endl;
        return;
    }

    int id = leerEntero("  ID del producto que desea modificar: ", 1, INT_MAX);

   // Buscamos el indice del producto para cargarlo en el borrador. Si no existe, mostramos un error y retornamos al menu principal.
    int indiceEncontrado = buscarIndiceProductoPorID(id);

    if (indiceEncontrado == -1) {
        cout << ROJO << "  Error: no existe un producto con el ID "
             << id << "." << RST << endl;
        return;
    }

    //creamos un borrador en memoria con los datos del producto encontrado para ir editandolo sin afectar el disco hasta que el usuario decida guardar los cambios.
    Producto borrador = leerProductoPorIndice(indiceEncontrado);

    int opcion;

    do {
        system("cls");
        imprimirTitulo("EDITANDO PRODUCTO (BORRADOR EN MEMORIA)");

        /* Mostramos el estado actual del borrador */
        cout << AMARI << "  Estado actual del borrador:" << RST << endl;
        cout << "  Nombre:       " << borrador.nombre      << endl;
        cout << "  Codigo:       " << borrador.codigo      << endl;
        cout << "  Descripcion:  " << borrador.descripcion << endl;
        cout << "  Precio:       $" << fixed << setprecision(2) << borrador.precio << endl;
        cout << "  Stock:        " << borrador.stock       << endl;
        cout << "  Stock minimo: " << borrador.stockMinimo << endl;
        cout << "  ID Proveedor: " << borrador.idProveedor << endl;
        imprimirLinea('-', 65);

        cout << "  " << CIAN << "1." << RST << " Editar Codigo"           << endl;
        cout << "  " << CIAN << "2." << RST << " Editar Nombre"           << endl;
        cout << "  " << CIAN << "3." << RST << " Editar Descripcion"      << endl;
        cout << "  " << CIAN << "4." << RST << " Editar ID Proveedor"     << endl;
        cout << "  " << CIAN << "5." << RST << " Editar Precio"           << endl;
        cout << "  " << CIAN << "6." << RST << " Editar Stock Minimo"     << endl;
        cout << "  " << VERDE << "7." << RST << " GUARDAR CAMBIOS EN DISCO" << endl;
        cout << "  " << ROJO  << "0." << RST << " CANCELAR (sin guardar)" << endl;

        opcion = leerEntero("  Seleccione una opcion: ", 0, 7);

        if (opcion == 1) {
            // Editar codigo con validacion de que no quede vacio ni duplicado (si se cambio el codigo)
            char nuevoCod[20];
            cout << "  Nuevo Codigo: ";
            cin >> nuevoCod;
            limpiarBuffer();

            if (strlen(nuevoCod) == 0) {
                cout << ROJO << "  Error: el codigo no puede estar vacio." << RST << endl;
            } else if (strcmp(nuevoCod, borrador.codigo) != 0 &&
                       codigoDuplicado(nuevoCod)) {
                cout << ROJO << "  Error: ese codigo ya existe en otro producto."
                     << RST << endl;
            } else {
                strcpy(borrador.codigo, nuevoCod);
                cout << VERDE << "  Codigo actualizado en el borrador." << RST << endl;
            }

        } else if (opcion == 2) {
            // Editar nombre con validacion de que no quede vacio
            char nuevoNom[100];
            cout << "  Nuevo Nombre: ";
            cin.getline(nuevoNom, 100);
            if (strlen(nuevoNom) == 0) {
                cout << ROJO << "  Error: el nombre no puede estar vacio." << RST << endl;
            } else {
                strcpy(borrador.nombre, nuevoNom);
                cout << VERDE << "  Nombre actualizado en el borrador." << RST << endl;
            }

        } else if (opcion == 3) {
            // Editar descripcion
            cout << "  Nueva Descripcion: ";
            cin.getline(borrador.descripcion, 200);
            cout << VERDE << "  Descripcion actualizada en el borrador." << RST << endl;

        } else if (opcion == 4) {
            // Editar ID del proveedor con validacion de existencia
            int nuevoProv = leerEntero("  Nuevo ID Proveedor: ", 1, INT_MAX);
            if (!existeProveedor(nuevoProv)) {
                cout << ROJO << "  Error: no existe ese proveedor."
                     << RST << endl;
            } else {
                borrador.idProveedor = nuevoProv;
                cout << VERDE << "  Proveedor actualizado en el borrador."
                     << RST << endl;
            }

        } else if (opcion == 5) {
            // Editar precio
            borrador.precio = leerFlotante("  Nuevo Precio: $", 0.01f);
            cout << VERDE << "  Precio actualizado en el borrador." << RST << endl;

        } else if (opcion == 6) {
            // Editar stock minimo
            borrador.stockMinimo = leerEntero("  Nuevo Stock Minimo: ", 0, INT_MAX);
            cout << VERDE << "  Stock minimo actualizado en el borrador." << RST << endl;

        } else if (opcion == 7) {
            // Guardar el borrador en disco - pide confirmacion primero
            char confirmar;
            cout << "  Aplicar todos los cambios permanentemente? (S/N): ";
            cin >> confirmar;
            limpiarBuffer();

            if (tolower(confirmar) == 's') {
                actualizarProductoEnDisco(indiceEncontrado, borrador);
                cout << FVRD << "  Cambios guardados exitosamente en disco."
                     << RST << endl;
                return;  // Salimos de la funcion despues de guardar para evitar que el usuario siga editando un producto que ya se guardo.
            } else {
                cout << AMARI << "  Cambios NO guardados. Puede seguir editando."
                     << RST << endl;
            }
        }

        if (opcion != 0 && opcion != 7) {
            system("pause");
        }

    } while (opcion != 0);

    cout << AMARI << "  Edicion cancelada. No se modifico nada en disco."
         << RST << endl;
}

// Adaptacion de ajusteStockProducto(Tienda*) del original, con validacion de que el stock no quede negativo.

void ajusteStockProducto() {

    ArchivoHeader h = leerHeader("productos.bin");
    if (h.registrosActivos == 0) {
        cout << AMARI << "  No hay productos registrados." << RST << endl;
        return;
    }

    cout << endl << AZUL << "  --- AJUSTE MANUAL DE STOCK ---" << RST << endl;

    int id = leerEntero("  Ingrese el ID del producto: ", 1, INT_MAX);
    int idx = buscarIndiceProductoPorID(id);

    if (idx == -1) {
        cout << ROJO << "  Error: producto no encontrado." << RST << endl;
        return;
    }

    Producto p = leerProductoPorIndice(idx);

    cout << "  Producto:    " << NEG << p.nombre << RST << endl;
    cout << "  Stock actual: " << AMARI << p.stock << RST << endl;

    int ajuste = leerEntero("  Cantidad a sumar (+) o restar (-): ",
                             INT_MIN, INT_MAX);

    // Validamos que el stock resultante no sea negativo antes de aplicar el ajuste.
    if (p.stock + ajuste < 0) {
        cout << ROJO << "  Error: el stock resultante ("
             << (p.stock + ajuste) << ") no puede ser negativo."
             << RST << endl;
        return;
    }

    p.stock += ajuste;
    actualizarProductoEnDisco(idx, p);

    cout << VERDE << "  Stock actualizado. Nuevo total: "
         << p.stock << RST << endl;
}

// Adaptacion de ajusteStockProducto(Tienda*) del original, con validacion de que el stock no quede negativo.

void listarProductos() {

    ArchivoHeader h = leerHeader("productos.bin");
    if (h.registrosActivos == 0) {
        cout << AMARI << "  No hay productos registrados." << RST << endl;
        return;
    }

    imprimirTitulo("LISTADO COMPLETO DE PRODUCTOS");

    // Encabezado de la tabla
    cout << NEG << " " << left
         << setw(4)  << "ID"
         << setw(11) << " Codigo"
         << setw(20) << " Nombre"
         << setw(16) << " Proveedor"
         << setw(10) << " Precio"
         << setw(8)  << " Stock"
         << " Minimo"
         << RST << endl;
    imprimirLinea('-', 76);

    ifstream f("productos.bin", ios::binary);
    if (!f.is_open()) {
        cout << ROJO << "  Error al abrir el archivo." << RST << endl;
        return;
    }

    Producto p;
    int cnt = 0;

    for (int i = 0; i < h.cantidadRegistros; i++) {
        f.seekg(calcularOffset(i, sizeof(Producto)), ios::beg);
        f.read(reinterpret_cast<char*>(&p), sizeof(Producto));

        if (p.eliminado) continue;

        // Buscamos el nombre del proveedor para mostrarlo en la tabla. Si no se encuentra, mostramos "Desconocido".
        char nomProv[14] = "Desconocido";
        int idxPr = buscarIndiceProveedorPorID(p.idProveedor);
        if (idxPr != -1) {
            Proveedor pr = leerProveedorPorIndice(idxPr);
            strncpy(nomProv, pr.nombre, 13);
            nomProv[13] = '\0';
        }

        cout << " " << setw(4)  << left << p.id
             << setw(11) << left << p.codigo
             << setw(20) << left << p.nombre
             << setw(16) << left << nomProv
             << "$" << setw(9)  << left << fixed << setprecision(2) << p.precio;

        // Stock en rojo si esta en nivel critico
        if (p.stock <= p.stockMinimo)
            cout << ROJO << NEG << setw(8) << left << p.stock << RST;
        else
            cout << VERDE << setw(8) << left << p.stock << RST;

        cout << p.stockMinimo << endl;
        cnt++;
    }

    f.close();
    imprimirLinea('=', 76);
    cout << AZUL << "  Total de productos activos: " << cnt << RST << endl;
}

// Adaptacion de eliminarProducto(Tienda*) del original, usando borrado logico.

void eliminarProducto() {

    ArchivoHeader h = leerHeader("productos.bin");
    if (h.registrosActivos == 0) {
        cout << AMARI << "  No hay productos para eliminar." << RST << endl;
        return;
    }

    cout << endl << AZUL << "  --- ELIMINAR PRODUCTO ---" << RST << endl;

    int id = leerEntero("  ID del producto a eliminar: ", 1, INT_MAX);
    int idx = buscarIndiceProductoPorID(id);

    if (idx == -1) {
        cout << ROJO << "  Error: producto no encontrado." << RST << endl;
        return;
    }

    Producto p = leerProductoPorIndice(idx);

    cout << endl;
    cout << ROJO << "  Producto: " << NEG << p.nombre << RST << endl;
    cout << AMARI << "  Esta accion aplica BORRADO LOGICO." << endl;
    cout << "  El registro queda en disco pero no sera visible."
         << RST << endl;

    // Confirmacion con entero como en el original
    int confirmar = leerEntero("  Confirmar eliminacion? (1: SI / 0: NO): ", 0, 1);

    if (confirmar == 1) {
        p.eliminado = true;
        actualizarProductoEnDisco(idx, p);

        h.registrosActivos--;
        actualizarHeader("productos.bin", h);

        cout << FVRD << "  Producto eliminado exitosamente." << RST << endl;

    } else if (confirmar == 0) {
        cout << AMARI << "  Operacion cancelada por el usuario." << RST << endl;
    } else {
        cout << ROJO << "  Opcion no valida. Operacion cancelada por seguridad."
             << RST << endl;
    }
}

// Adaptacion de crearProveedor() del original, con validaciones y sin arreglos dinamicos.

void crearProveedor() {

    Proveedor nuevo;
    memset(&nuevo, 0, sizeof(Proveedor));

    imprimirTitulo("REGISTRAR NUEVO PROVEEDOR");

    //nombre del proveedor, validando que no quede vacio. Repite hasta que el usuario ingrese algo valido.
    do {
        cout << "  Nombre de la empresa: ";
        cin.getline(nuevo.nombre, 100);
        if (strlen(nuevo.nombre) == 0)
            cout << ROJO << "  Error: el nombre no puede estar vacio." << RST << endl;
    } while (strlen(nuevo.nombre) == 0);

    // RIF del proveedor, validando que no quede vacio ni duplicado. Repite hasta que el usuario ingrese algo valido.
    bool rifBueno = false;
    do {
        cout << "  RIF: ";
        cin.getline(nuevo.rif, 20);
        if (strlen(nuevo.rif) == 0) {
            cout << ROJO << "  Error: el RIF no puede estar vacio." << RST << endl;
        } else if (rifDuplicado(nuevo.rif)) {
            cout << ROJO << "  Error: el RIF '" << nuevo.rif
                 << "' ya esta registrado." << RST << endl;
        } else {
            rifBueno = true;
        }
    } while (!rifBueno);

    //telefono sin validacion de formato, pero no puede quedar vacio
    cout << "  Telefono: ";
    cin.getline(nuevo.telefono, 20);

    //email con validacion de formato
    do {
        cout << "  Email: ";
        cin.getline(nuevo.email, 100);
        if (!validarEmail(nuevo.email))
            cout << ROJO << "  Error: formato invalido. Ejemplo: empresa@correo.com"
                 << RST << endl;
    } while (!validarEmail(nuevo.email));

    //direccion sin validacion, pero no puede quedar vacia
    cout << "  Direccion: ";
    cin.getline(nuevo.direccion, 200);

    if (registrarProveedor(nuevo)) {
        cout << FVRD << "  Proveedor registrado " << RST << endl;
    } else {
        cout << FROJ << "  Error: no se pudo guardar el proveedor." << RST << endl;
    }
}

// Adaptacion de buscarProveedor() del original, con opciones de busqueda y sin arreglos dinamicos.

void buscarProveedor() {

    ArchivoHeader h = leerHeader("proveedores.bin");
    if (h.registrosActivos == 0) {
        cout << AMARI << "  No hay proveedores registrados para buscar." << RST << endl;
        return;
    }

    imprimirTitulo("BUSQUEDA DE PROVEEDORES");
    cout << "  " << CIAN << "1." << RST << " Por ID (exacto)"          << endl;
    cout << "  " << CIAN << "2." << RST << " Por nombre (parcial)"     << endl;
    cout << "  " << CIAN << "3." << RST << " Por RIF (exacto)"         << endl;
    cout << "  " << ROJO  << "0." << RST << " Cancelar"                << endl;

    int op = leerEntero("  Seleccione: ", 0, 3);
    if (op == 0) return;

    bool encontrado = false;
    Proveedor p;

    if (op == 1) {
        int id = leerEntero("  ID del proveedor: ", 1, INT_MAX);
        int idx = buscarIndiceProveedorPorID(id);
        if (idx != -1) {
            cout << endl;
            imprimirProveedorDetallado(leerProveedorPorIndice(idx));
            encontrado = true;
        }

    } else if (op == 2) {
        char busq[100];
        cout << "  Nombre (o parte del nombre): ";
        cin.getline(busq, 100);

        if (strlen(busq) == 0) {
            cout << ROJO << "  Error: campo vacio." << RST << endl;
            return;
        }

        char busqMin[100];
        strcpy(busqMin, busq);
        convertirAMinusculas(busqMin);

        cout << endl;
        ifstream f("proveedores.bin", ios::binary);
        if (!f.is_open()) { cout << ROJO << "  Error." << RST << endl; return; }

        for (int i = 0; i < h.cantidadRegistros; i++) {
            f.seekg(calcularOffset(i, sizeof(Proveedor)), ios::beg);
            f.read(reinterpret_cast<char*>(&p), sizeof(Proveedor));
            if (p.eliminado) continue;

            char nomMin[100];
            strcpy(nomMin, p.nombre);
            convertirAMinusculas(nomMin);

            if (contieneSubstring(nomMin, busqMin)) {
                imprimirProveedorDetallado(p);
                encontrado = true;
            }
        }
        f.close();

    } else if (op == 3) {
        char rifB[20];
        cout << "  RIF exacto: ";
        cin >> rifB;
        limpiarBuffer();

        ifstream f("proveedores.bin", ios::binary);
        if (!f.is_open()) { cout << ROJO << "  Error." << RST << endl; return; }

        for (int i = 0; i < h.cantidadRegistros; i++) {
            f.seekg(calcularOffset(i, sizeof(Proveedor)), ios::beg);
            f.read(reinterpret_cast<char*>(&p), sizeof(Proveedor));
            if (!p.eliminado && strcmp(p.rif, rifB) == 0) {
                cout << endl;
                imprimirProveedorDetallado(p);
                encontrado = true;
                break; // Como es busqueda exacta, podemos romper el ciclo al encontrar la primera coincidencia.
            }
        }
        f.close();
    }

    if (!encontrado)
        cout << AMARI << "  No se encontraron coincidencias." << RST << endl;
}

// Adaptacion de actualizarProveedor() del original, con validaciones y sin arreglos dinamicos.
void actualizarProveedor() {

    ArchivoHeader h = leerHeader("proveedores.bin");
    if (h.registrosActivos == 0) {
        cout << AMARI << "  No hay proveedores para editar." << RST << endl;
        return;
    }

    int id = leerEntero("  ID del proveedor a editar: ", 1, INT_MAX);
    int idx = buscarIndiceProveedorPorID(id);

    if (idx == -1) {
        cout << ROJO << "  Error: proveedor no encontrado." << RST << endl;
        return;
    }

    Proveedor p = leerProveedorPorIndice(idx);

    cout << endl;
    cout << AMARI << "  Editando: " << NEG << p.nombre << RST << endl;
    cout << CIAN  << "  (Deje en blanco para conservar el valor actual)"
         << RST << endl << endl;

    char entrada[200];

    // RIF no se puede editar porque es el identificador unico del proveedor. Si se permitiera cambiarlo, se perderia la referencia a ese proveedor desde los productos que lo usan, quedando con un ID de proveedor que ya no existe. Para cambiar el RIF, primero se tendria que crear un nuevo proveedor con el nuevo RIF y luego actualizar manualmente cada producto que usaba el proveedor anterior para que apunte al nuevo ID, lo cual es propenso a errores y no es recomendable. Por eso se decide que el RIF no sea editable.
    cout << "  Nuevo nombre    [" << p.nombre    << "]: ";
    cin.getline(entrada, 100);
    if (strlen(entrada) > 0) strcpy(p.nombre, entrada);

    cout << "  Nuevo telefono  [" << p.telefono  << "]: ";
    cin.getline(entrada, 20);
    if (strlen(entrada) > 0) strcpy(p.telefono, entrada);

    cout << "  Nuevo email     [" << p.email     << "]: ";
    cin.getline(entrada, 100);
    if (strlen(entrada) > 0) {
        if (validarEmail(entrada)) strcpy(p.email, entrada);
        else cout << ROJO << "  Email invalido, se conserva el anterior." << RST << endl;
    }

    cout << "  Nueva direccion [" << p.direccion << "]: ";
    cin.getline(entrada, 200);
    if (strlen(entrada) > 0) strcpy(p.direccion, entrada);

    actualizarProveedorEnDisco(idx, p);
    cout << FVRD << "  Proveedor actualizado correctamente." << RST << endl;
}

// Adaptacion de eliminarProveedor() del original, usando borrado logico.

void listarProveedores() {

    ArchivoHeader h = leerHeader("proveedores.bin");
    if (h.registrosActivos == 0) {
        cout << AMARI << "  No hay proveedores registrados." << RST << endl;
        return;
    }

    imprimirTitulo("LISTADO DE PROVEEDORES");

    cout << NEG << " " << left
         << setw(4)  << "ID"
         << setw(22) << " Nombre"
         << setw(14) << " RIF"
         << setw(14) << " Telefono"
         << " Email" << RST << endl;
    imprimirLinea('-', 72);

    ifstream f("proveedores.bin", ios::binary);
    if (!f.is_open()) {
        cout << ROJO << "  Error al abrir el archivo." << RST << endl;
        return;
    }

    Proveedor p;
    int cnt = 0;

    for (int i = 0; i < h.cantidadRegistros; i++) {
        f.seekg(calcularOffset(i, sizeof(Proveedor)), ios::beg);
        f.read(reinterpret_cast<char*>(&p), sizeof(Proveedor));
        if (!p.eliminado) {
            cout << " " << setw(4)  << left << p.id
                 << setw(22) << left << p.nombre
                 << setw(14) << left << p.rif
                 << setw(14) << left << p.telefono
                 << p.email << endl;
            cnt++;
        }
    }

    f.close();
    imprimirLinea('=', 72);
    cout << AZUL << "  Total de proveedores activos: " << cnt << RST << endl;
}

// Adaptacion de eliminarProveedor() del original, usando borrado logico y con advertencia sobre referencias rotas.

void eliminarProveedor() {

    ArchivoHeader h = leerHeader("proveedores.bin");
    if (h.registrosActivos == 0) {
        cout << AMARI << "  No hay proveedores para eliminar." << RST << endl;
        return;
    }

    int id = leerEntero("  ID del proveedor a eliminar: ", 1, INT_MAX);
    int idx = buscarIndiceProveedorPorID(id);

    if (idx == -1) {
        cout << ROJO << "  Error: proveedor no encontrado." << RST << endl;
        return;
    }

    Proveedor p = leerProveedorPorIndice(idx);

    cout << endl;
    cout << ROJO << "  Proveedor: " << NEG << p.nombre << RST << endl;
    cout << AMARI << "  ATENCION: sus productos quedaran con referencia rota."
         << RST << endl;

    char conf;
    cout << "  Esta seguro de eliminar este proveedor? (S/N): ";
    cin >> conf;
    limpiarBuffer();

    if (tolower(conf) == 's') {
        p.eliminado = true;
        actualizarProveedorEnDisco(idx, p);
        h.registrosActivos--;
        actualizarHeader("proveedores.bin", h);
        cout << FVRD << "  Proveedor eliminado." << RST << endl;
    } else {
        cout << AMARI << "  Operacion cancelada." << RST << endl;
    }
}

// Adaptacion de crearCliente() del original, con validaciones y sin arreglos dinamicos.

void crearCliente() {

    Cliente nuevo;
    memset(&nuevo, 0, sizeof(Cliente));

    imprimirTitulo("REGISTRAR NUEVO CLIENTE");

    //nombre sin validacion de formato, pero no puede quedar vacio
    do {
        cout << "  Nombre completo: ";
        cin.getline(nuevo.nombre, 100);
        if (strlen(nuevo.nombre) == 0)
            cout << ROJO << "  Error: el nombre no puede estar vacio." << RST << endl;
    } while (strlen(nuevo.nombre) == 0);

    //cedula con validacion de duplicados, no puede quedar vacia
    bool cedBuena = false;
    do {
        cout << "  Cedula: ";
        cin >> nuevo.cedula;
        limpiarBuffer();
        if (strlen(nuevo.cedula) == 0) {
            cout << ROJO << "  Error: la cedula no puede estar vacia." << RST << endl;
        } else if (cedulaDuplicada(nuevo.cedula)) {
            cout << ROJO << "  Error: la cedula '" << nuevo.cedula
                 << "' ya esta registrada." << RST << endl;
        } else {
            cedBuena = true;
        }
    } while (!cedBuena);

    //telefono sin validacion de formato, pero no puede quedar vacio
    cout << "  Telefono: ";
    cin >> nuevo.telefono;
    limpiarBuffer();

  //email con validacion de formato
    do {
        cout << "  Email: ";
        cin.getline(nuevo.email, 100);
        if (!validarEmail(nuevo.email))
            cout << ROJO << "  Error: formato invalido. Ejemplo: usuario@correo.com"
                 << RST << endl;
    } while (!validarEmail(nuevo.email));

   //direccion sin validacion, pero no puede quedar vacia
    cout << "  Direccion: ";
    cin.getline(nuevo.direccion, 200);

    if (registrarCliente(nuevo)) {
        cout << FVRD << "  Cliente registrado "<< RST << endl;
    } else {
        cout << FROJ << "  Error: no se pudo guardar el cliente." << RST << endl;
    }
}

// Adaptacion de buscarCliente() del original, con opciones de busqueda y sin arreglos dinamicos.

void buscarCliente() {

    ArchivoHeader h = leerHeader("clientes.bin");
    if (h.registrosActivos == 0) {
        cout << AMARI << "  No hay clientes registrados para buscar." << RST << endl;
        return;
    }

    imprimirTitulo("BUSQUEDA DE CLIENTES");
    cout << "  " << CIAN << "1." << RST << " Buscar por ID (exacto)"       << endl;
    cout << "  " << CIAN << "2." << RST << " Buscar por Cedula (exacto)"   << endl;
    cout << "  " << CIAN << "3." << RST << " Buscar por nombre (parcial)"  << endl;
    cout << "  " << ROJO  << "0." << RST << " Cancelar"                    << endl;

    int op = leerEntero("  Seleccione: ", 0, 3);
    if (op == 0) return;

    bool encontrado = false;
    Cliente c;

    if (op == 1) {
        int id = leerEntero("  ID del cliente: ", 1, INT_MAX);
        int idx = buscarIndiceClientePorID(id);
        if (idx != -1) {
            cout << endl;
            imprimirClienteDetallado(leerClientePorIndice(idx));
            encontrado = true;
        }

    } else if (op == 2) {
        char ced[20];
        cout << "  Cedula del cliente: ";
        cin >> ced;
        limpiarBuffer();

        ifstream f("clientes.bin", ios::binary);
        if (!f.is_open()) { cout << ROJO << "  Error." << RST << endl; return; }

        for (int i = 0; i < h.cantidadRegistros; i++) {
            f.seekg(calcularOffset(i, sizeof(Cliente)), ios::beg);
            f.read(reinterpret_cast<char*>(&c), sizeof(Cliente));
            if (!c.eliminado && strcmp(c.cedula, ced) == 0) {
                cout << endl;
                imprimirClienteDetallado(c);
                encontrado = true;
                break;
            }
        }
        f.close();

    } else if (op == 3) {
        char busq[100];
        cout << "  Nombre o parte del nombre: ";
        cin.getline(busq, 100);

        if (strlen(busq) == 0) {
            cout << ROJO << "  Error: campo vacio." << RST << endl;
            return;
        }

        cout << endl << AZUL << "  --- Coincidencias ---" << RST << endl;

        ifstream f("clientes.bin", ios::binary);
        if (!f.is_open()) { cout << ROJO << "  Error." << RST << endl; return; }

        for (int i = 0; i < h.cantidadRegistros; i++) {
            f.seekg(calcularOffset(i, sizeof(Cliente)), ios::beg);
            f.read(reinterpret_cast<char*>(&c), sizeof(Cliente));
            if (!c.eliminado && strstr(c.nombre, busq) != NULL) {
                imprimirClienteDetallado(c);
                encontrado = true;
            }
        }
        f.close();
    }

    if (!encontrado)
        cout << AMARI << "  No se encontraron coincidencias." << RST << endl;
}

// Adaptacion de actualizarCliente() del original, con validaciones y sin arreglos dinamicos.

void actualizarCliente() {

    ArchivoHeader h = leerHeader("clientes.bin");
    if (h.registrosActivos == 0) {
        cout << AMARI << "  No hay clientes para editar." << RST << endl;
        return;
    }

    int id = leerEntero("  ID del cliente a editar: ", 1, INT_MAX);
    int idx = buscarIndiceClientePorID(id);

    if (idx == -1) {
        cout << ROJO << "  Error: cliente no encontrado." << RST << endl;
        return;
    }

    Cliente c = leerClientePorIndice(idx);

    cout << endl;
    cout << AZUL << "  Editando: " << NEG << c.nombre << RST << endl;
    cout << CIAN << "  (Deje en blanco para conservar el valor actual)"
         << RST << endl << endl;

    char ent[200];

    cout << "  Nuevo nombre    [" << c.nombre    << "]: ";
    cin.getline(ent, 100);
    if (strlen(ent) > 0) strcpy(c.nombre, ent);

    cout << "  Nueva cedula    [" << c.cedula    << "]: ";
    cin.getline(ent, 20);
    if (strlen(ent) > 0) strcpy(c.cedula, ent);

    cout << "  Nuevo telefono  [" << c.telefono  << "]: ";
    cin.getline(ent, 20);
    if (strlen(ent) > 0) strcpy(c.telefono, ent);

    cout << "  Nuevo email     [" << c.email     << "]: ";
    cin.getline(ent, 100);
    if (strlen(ent) > 0) {
        if (validarEmail(ent)) strcpy(c.email, ent);
        else cout << ROJO << "  Email invalido, se conserva el anterior." << RST << endl;
    }

    cout << "  Nueva direccion [" << c.direccion << "]: ";
    cin.getline(ent, 200);
    if (strlen(ent) > 0) strcpy(c.direccion, ent);

    actualizarClienteEnDisco(idx, c);
    cout << FVRD << "  Cliente actualizado correctamente en el archivo." << RST << endl;
}

// Adaptacion de eliminarCliente() del original, usando borrado logico.
void listarClientes() {

    ArchivoHeader h = leerHeader("clientes.bin");
    if (h.registrosActivos == 0) {
        cout << AMARI << "  No hay clientes registrados." << RST << endl;
        return;
    }

    imprimirTitulo("LISTADO DE CLIENTES");

    cout << NEG << " " << left
         << setw(4)  << "ID"
         << setw(22) << " Nombre"
         << setw(14) << " Cedula"
         << setw(14) << " Telefono"
         << setw(14) << " Total $"
         << " Email" << RST << endl;
    imprimirLinea('-', 78);

    ifstream f("clientes.bin", ios::binary);
    if (!f.is_open()) {
        cout << ROJO << "  Error al abrir el archivo." << RST << endl;
        return;
    }

    Cliente c;
    int cnt = 0;

    for (int i = 0; i < h.cantidadRegistros; i++) {
        f.seekg(calcularOffset(i, sizeof(Cliente)), ios::beg);
        f.read(reinterpret_cast<char*>(&c), sizeof(Cliente));
        if (!c.eliminado) {
            cout << " " << setw(4)  << left << c.id
                 << setw(22) << left << c.nombre
                 << setw(14) << left << c.cedula
                 << setw(14) << left << c.telefono
                 << "$" << setw(13) << left << fixed << setprecision(2) << c.totalGastado
                 << c.email << endl;
            cnt++;
        }
    }

    f.close();
    imprimirLinea('=', 78);
    cout << AZUL << "  Total de clientes activos: " << cnt << RST << endl;
}

// Adaptacion de eliminarCliente() del original, usando borrado logico y con confirmacion de seguridad.

void eliminarCliente() {

    ArchivoHeader h = leerHeader("clientes.bin");
    if (h.registrosActivos == 0) {
        cout << AMARI << "  No hay clientes para eliminar." << RST << endl;
        return;
    }

    int id = leerEntero("  ID del cliente a eliminar: ", 1, INT_MAX);
    int idx = buscarIndiceClientePorID(id);

    if (idx == -1) {
        cout << ROJO << "  Error: cliente no encontrado." << RST << endl;
        return;
    }

    Cliente c = leerClientePorIndice(idx);

    cout << endl;
    cout << "  Se eliminara a: " << ROJO << NEG << c.nombre << RST << endl;

    char conf;
    cout << "  Esta seguro de eliminar este registro? (S/N): ";
    cin >> conf;
    limpiarBuffer();

    if (tolower(conf) == 's') {
        c.eliminado = true;
        actualizarClienteEnDisco(idx, c);
        h.registrosActivos--;
        actualizarHeader("clientes.bin", h);
        cout << FVRD << "  Cliente eliminado satisfactoriamente." << RST << endl;
    } else {
        cout << AMARI << "  Operacion cancelada." << RST << endl;
    }
}

// Adaptacion de registrarCompra() del original, con soporte para multi-producto, validaciones y actualizacion inmediata de stock en disco.

void registrarCompraBinaria() {

    imprimirTitulo("NUEVA COMPRA (Proveedor)");

   // Validamos el proveedor primero
    int idProv = leerEntero("  ID del Proveedor: ", 1, INT_MAX);
    int idxProv = buscarIndiceProveedorPorID(idProv);

    if (idxProv == -1) {
        cout << ROJO << "  Error: proveedor no encontrado." << RST << endl;
        return;
    }

    Proveedor prov = leerProveedorPorIndice(idxProv);
    cout << VERDE << "  Proveedor: " << prov.nombre << RST << endl << endl;

    // Preparamos la estructura de la transaccion
    Transaccion t;
    memset(&t, 0, sizeof(Transaccion));
    strcpy(t.tipo, "COMPRA");
    t.idRelacionado = idProv;
    t.numItems      = 0;
    t.totalFactura  = 0.0f;

    char masProductos = 's';

    while (t.numItems < 15 && tolower(masProductos) == 's') {

        cout << "  --- Producto " << (t.numItems + 1) << " ---" << endl;

        int idProd = leerEntero("  ID del Producto: ", 1, INT_MAX);
        int idxProd = buscarIndiceProductoPorID(idProd);

        if (idxProd == -1) {
            cout << ROJO << "  Error: producto no encontrado. Compra cancelada."
                 << RST << endl;
            return;
        }

        Producto p = leerProductoPorIndice(idxProd);
        cout << "  Producto: " << NEG << p.nombre << RST
             << "  |  Stock actual: " << AMARI << p.stock << RST << endl;

        int cantidad = leerEntero("  Cantidad comprada: ", 1, INT_MAX);
        float precioUnitario = leerFlotante("  Precio unitario: $", 0.01f);

        // Guardamos el item en la transaccion
        t.idsProductos[t.numItems] = idProd;
        t.cantidades[t.numItems]   = cantidad;
        t.totalFactura            += (float)cantidad * precioUnitario;
        t.numItems++;

        // Actualizamos el stock del producto en disco inmediatamente
        p.stock += cantidad;
        p.fechaUltimaModificacion = time(0);
        actualizarProductoEnDisco(idxProd, p);

        cout << VERDE << "  Stock actualizado. Nuevo total: " << p.stock << RST << endl;
        cout << endl;

        if (t.numItems < 15) {
            cout << "  Agregar otro producto a esta compra? (S/N): ";
            cin >> masProductos;
            limpiarBuffer();
        }
    }

    if (t.numItems == 0) {
        cout << AMARI << "  Compra cancelada, no se agrego ningun producto."
             << RST << endl;
        return;
    }

    int idAsignado = registrarTransaccion(t);

    if (idAsignado != -1) {
        cout << endl << FVRD
             << "  Compra #" << idAsignado << " registrada. "
             << t.numItems << " producto(s). Total: $"
             << fixed << setprecision(2) << t.totalFactura
             << RST << endl;
    } else {
        cout << FROJ << "  Error: no se pudo guardar la transaccion." << RST << endl;
    }
}

// Adaptacion de registrarVenta() del original, con soporte para multi-producto, validaciones y actualizacion inmediata de stock en disco.

void registrarVentaBinaria() {

    imprimirTitulo("NUEVA VENTA (Cliente)");

    // Validamos el cliente primero
    int idCli = leerEntero("  ID del Cliente: ", 1, INT_MAX);
    int idxCli = buscarIndiceClientePorID(idCli);

    if (idxCli == -1) {
        cout << ROJO << "  Error: cliente no encontrado." << RST << endl;
        return;
    }

    Cliente cli = leerClientePorIndice(idxCli);
    cout << VERDE << "  Cliente: " << cli.nombre << RST << endl << endl;

    // Preparamos la estructura de la transaccion
    Transaccion t;
    memset(&t, 0, sizeof(Transaccion));
    strcpy(t.tipo, "VENTA");
    t.idRelacionado = idCli;
    t.numItems      = 0;
    t.totalFactura  = 0.0f;

    char masProductos = 's';

    while (t.numItems < 15 && tolower(masProductos) == 's') {

        cout << "  --- Producto " << (t.numItems + 1) << " ---" << endl;

        int idProd = leerEntero("  ID del Producto: ", 1, INT_MAX);
        int idxProd = buscarIndiceProductoPorID(idProd);

        if (idxProd == -1) {
            cout << ROJO << "  Error: producto no encontrado. Venta cancelada."
                 << RST << endl;
            return;
        }

        Producto p = leerProductoPorIndice(idxProd);
        cout << "  Producto: " << NEG << p.nombre << RST
             << "  |  Stock: " << AMARI << p.stock << RST
             << "  |  Precio: $" << p.precio << endl;

        // Validamos que haya stock disponible para la venta
        if (p.stock <= 0) {
            cout << ROJO << "  Error: este producto no tiene stock disponible."
                 << RST << endl;
            cout << "  Agregar otro producto? (S/N): ";
            cin >> masProductos;
            limpiarBuffer();
            continue;
        }

        // La cantidad maxima que pueden pedir es la que hay en stock
        int cantidad = leerEntero("  Cantidad a vender: ", 1, p.stock);

        // Guardamos el item en la transaccion
        t.idsProductos[t.numItems] = idProd;
        t.cantidades[t.numItems]   = cantidad;
        t.totalFactura            += p.precio * (float)cantidad;
        t.numItems++;

        // Actualizamos stock y estadisticas del producto en disco
        p.stock        -= cantidad;
        p.totalVendido += cantidad;
        p.fechaUltimaModificacion = time(0);
        actualizarProductoEnDisco(idxProd, p);

        cout << VERDE << "  Stock descontado. Stock restante: "
             << p.stock << RST << endl << endl;

        if (t.numItems < 15) {
            cout << "  Agregar otro producto a esta venta? (S/N): ";
            cin >> masProductos;
            limpiarBuffer();
        }
    }

    if (t.numItems == 0) {
        cout << AMARI << "  Venta cancelada." << RST << endl;
        return;
    }

    int idAsignado = registrarTransaccion(t);

    if (idAsignado != -1) {
        // Actualizamos las estadisticas del cliente en disco
        cli = leerClientePorIndice(idxCli);
        cli.totalGastado += t.totalFactura;

        if (cli.numTransacciones < 50) {
            cli.historialTransacciones[cli.numTransacciones] = idAsignado;
            cli.numTransacciones++;
        }

        actualizarClienteEnDisco(idxCli, cli);

        cout << FVRD
             << "  Venta #" << idAsignado << " registrada. "
             << t.numItems << " producto(s). Total: $"
             << fixed << setprecision(2) << t.totalFactura
             << RST << endl;
    } else {
        cout << FROJ << "  Error: no se pudo guardar la transaccion." << RST << endl;
    }
}

// Adaptacion de listarTransacciones() del original, mostrando informacion relevante de cliente/proveedor y primer producto, y con formato tabular.
void listarTransacciones() {

    ArchivoHeader h = leerHeader("transacciones.bin");
    if (h.cantidadRegistros == 0) {
        cout << AMARI << "  No hay transacciones registradas." << RST << endl;
        return;
    }

    imprimirTitulo("HISTORIAL DE TRANSACCIONES");

    cout << NEG << " " << left
         << setw(4)  << "ID"
         << setw(8)  << " Tipo"
         << setw(22) << " Producto(s)"
         << setw(20) << " Cliente/Proveedor"
         << setw(7)  << " Items"
         << setw(12) << " Total $"
         << " Fecha" << RST << endl;
    imprimirLinea('-', 82);

    ifstream f("transacciones.bin", ios::binary);
    if (!f.is_open()) {
        cout << ROJO << "  Error al abrir el archivo." << RST << endl;
        return;
    }

    // Saltamos el header y leemos registro por registro
    f.seekg(sizeof(ArchivoHeader), ios::beg);

    Transaccion t;
    int cnt = 0;

    while (f.read(reinterpret_cast<char*>(&t), sizeof(Transaccion))) {

        if (t.eliminado) continue; // Saltamos las canceladas

        // Nombre del primer producto
        char nomProd[22] = "Desconocido";
        int idxP = buscarIndiceProductoPorID(t.idsProductos[0]);
        if (idxP != -1) {
            Producto p = leerProductoPorIndice(idxP);
            strncpy(nomProd, p.nombre, 19);
            nomProd[19] = '\0';
            if (t.numItems > 1) strcat(nomProd, "..");
        }

        // Nombre del cliente o proveedor relacionado
        char nomRel[22] = "Desconocido";
        if (strcmp(t.tipo, "VENTA") == 0) {
            int idxC = buscarIndiceClientePorID(t.idRelacionado);
            if (idxC != -1) {
                Cliente c = leerClientePorIndice(idxC);
                strncpy(nomRel, c.nombre, 20);
                nomRel[20] = '\0';
            }
        } else {
            int idxPr = buscarIndiceProveedorPorID(t.idRelacionado);
            if (idxPr != -1) {
                Proveedor pr = leerProveedorPorIndice(idxPr);
                strncpy(nomRel, pr.nombre, 20);
                nomRel[20] = '\0';
            }
        }

        // Formateamos la fecha
        char bufF[11];
        struct tm* info = localtime(&t.fecha);
        strftime(bufF, 11, "%Y-%m-%d", info);

        // Color segun tipo de operacion
        const char* colTipo = (strcmp(t.tipo, "VENTA") == 0) ? VERDE : AZUL;

        cout << " " << setw(4)  << left << t.id
             << colTipo << setw(8) << left << t.tipo << RST
             << setw(22) << left << nomProd
             << setw(20) << left << nomRel
             << setw(7)  << left << t.numItems
             << "$" << setw(11) << left << fixed << setprecision(2) << t.totalFactura
             << bufF << endl;

        cnt++;
    }

    f.close();
    imprimirLinea('=', 82);
    cout << AZUL << "  Total de transacciones activas: " << cnt << RST << endl;
}

// Adaptacion de buscarTransacciones() del original, con opciones de busqueda flexibles y mostrando resultados relevantes en formato tabular.
void buscarTransacciones() {

    ArchivoHeader h = leerHeader("transacciones.bin");
    if (h.registrosActivos == 0) {
        cout << AMARI << "  No hay transacciones registradas para buscar."
             << RST << endl;
        return;
    }

    imprimirTitulo("BUSQUEDA DE TRANSACCIONES");
    cout << "  " << CIAN << "1." << RST << " Por ID de Transaccion"      << endl;
    cout << "  " << CIAN << "2." << RST << " Por ID de Producto"          << endl;
    cout << "  " << CIAN << "3." << RST << " Por ID de Cliente"           << endl;
    cout << "  " << CIAN << "4." << RST << " Por ID de Proveedor"         << endl;
    cout << "  " << CIAN << "5." << RST << " Por Tipo (COMPRA o VENTA)"   << endl;
    cout << "  " << ROJO  << "0." << RST << " Regresar"                   << endl;

    int op = leerEntero("  Seleccione: ", 0, 5);
    if (op == 0) return;

    int idBuscado   = 0;
    char tipoFiltro[10] = "";
    bool encontrado = false;

    // Pedimos el parametro de busqueda segun la opcion elegida
    if (op == 1) {
        idBuscado = leerEntero("  ID de la transaccion: ", 1, INT_MAX);
    } else if (op == 2) {
        idBuscado = leerEntero("  ID del Producto: ", 1, INT_MAX);
    } else if (op == 3) {
        idBuscado = leerEntero("  ID del Cliente: ", 1, INT_MAX);
    } else if (op == 4) {
        idBuscado = leerEntero("  ID del Proveedor: ", 1, INT_MAX);
    } else if (op == 5) {
        cout << "  Tipo (COMPRA o VENTA): ";
        cin >> tipoFiltro;
        limpiarBuffer();
        // Convertimos a mayusculas para comparacion segura
        for (int i = 0; tipoFiltro[i]; i++)
            tipoFiltro[i] = (char)toupper((unsigned char)tipoFiltro[i]);
        if (strcmp(tipoFiltro, "COMPRA") != 0 && strcmp(tipoFiltro, "VENTA") != 0) {
            cout << ROJO << "  Error: tipo invalido. Use COMPRA o VENTA."
                 << RST << endl;
            return;
        }
    }

    cout << endl;
    imprimirLinea('-', 65);

    ifstream f("transacciones.bin", ios::binary);
    if (!f.is_open()) {
        cout << ROJO << "  Error al abrir el archivo." << RST << endl;
        return;
    }

    f.seekg(sizeof(ArchivoHeader), ios::beg);
    Transaccion t;

    while (f.read(reinterpret_cast<char*>(&t), sizeof(Transaccion))) {

        if (t.eliminado) continue;

        bool coincide = false;

        if (op == 1) {
            coincide = (t.id == idBuscado);
        } else if (op == 2) {
            // Buscamos el ID del producto en todos los items de la transaccion
            for (int j = 0; j < t.numItems; j++) {
                if (t.idsProductos[j] == idBuscado) { coincide = true; break; }
            }
        } else if (op == 3) {
            coincide = (strcmp(t.tipo, "VENTA")  == 0 && t.idRelacionado == idBuscado);
        } else if (op == 4) {
            coincide = (strcmp(t.tipo, "COMPRA") == 0 && t.idRelacionado == idBuscado);
        } else if (op == 5) {
            coincide = (strcmp(t.tipo, tipoFiltro) == 0);
        }

        if (coincide) {
            char bufF[11];
            struct tm* info = localtime(&t.fecha);
            strftime(bufF, 11, "%Y-%m-%d", info);

            const char* col = (strcmp(t.tipo, "VENTA") == 0) ? VERDE : AZUL;

            cout << "  ID: " << t.id
                 << "  Tipo: " << col << t.tipo << RST
                 << "  Items: " << t.numItems
                 << "  Total: $" << fixed << setprecision(2) << t.totalFactura
                 << "  Fecha: " << bufF << endl;

            encontrado = true;
        }
    }

    f.close();
    imprimirLinea('-', 65);

    if (!encontrado)
        cout << AMARI << "  No se encontraron transacciones para esa busqueda."
             << RST << endl;
}

// Adaptacion de cancelarTransaccion() del original, con confirmacion de seguridad y reversion del stock en disco.
void cancelarTransaccion() {

    ArchivoHeader h = leerHeader("transacciones.bin");
    if (h.registrosActivos == 0) {
        cout << AMARI << "  El historial de transacciones esta vacio." << RST << endl;
        return;
    }

    cout << endl << AZUL << "  --- CANCELAR TRANSACCION ---" << RST << endl;

    int id = leerEntero("  ID de la transaccion a cancelar: ", 1, INT_MAX);
    int idx = buscarIndiceTransaccionPorID(id);

    if (idx == -1) {
        cout << ROJO << "  Error: transaccion no encontrada." << RST << endl;
        return;
    }

    // Leemos la transaccion para mostrar su informacion
    fstream archT("transacciones.bin", ios::binary | ios::in | ios::out);
    if (!archT.is_open()) {
        cout << ROJO << "  Error al abrir el archivo." << RST << endl;
        return;
    }

    long posTrans = calcularOffset(idx, sizeof(Transaccion));
    archT.seekg(posTrans, ios::beg);

    Transaccion t;
    archT.read(reinterpret_cast<char*>(&t), sizeof(Transaccion));

    char bufF[11];
    struct tm* info = localtime(&t.fecha);
    strftime(bufF, 11, "%Y-%m-%d", info);

    // Mostramos los datos para que el usuario confirme
    cout << endl << AMARI << "  Transaccion encontrada:" << RST << endl;
    cout << "  Tipo:   " << t.tipo  << endl;
    cout << "  Items:  " << t.numItems << endl;
    cout << "  Total:  $" << fixed << setprecision(2) << t.totalFactura << endl;
    cout << "  Fecha:  " << bufF << endl;
    cout << endl;
    cout << AMARI << "  ATENCION: Esta accion revertira el impacto en el stock."
         << RST << endl;

    char confirmacion;
    cout << "  Desea cancelar esta operacion y revertir el stock? (S/N): ";
    cin >> confirmacion;
    limpiarBuffer();

    if (tolower(confirmacion) != 's') {
        cout << AMARI << "  Operacion cancelada. No se realizaron cambios."
             << RST << endl;
        archT.close();
        return;
    }

    // Revertimos el impacto en el stock de CADA producto de la transaccion
    for (int i = 0; i < t.numItems; i++) {

        int idxProd = buscarIndiceProductoPorID(t.idsProductos[i]);

        if (idxProd == -1) {
            cout << AMARI << "  Advertencia: producto ID "
                 << t.idsProductos[i] << " no existe, no se revirtio su stock."
                 << RST << endl;
            continue;
        }

        Producto p = leerProductoPorIndice(idxProd);

        if (strcmp(t.tipo, "COMPRA") == 0) {
            // Si fue COMPRA, el stock subio -> lo bajamos
            p.stock -= t.cantidades[i];
            if (p.stock < 0) p.stock = 0; // Evitamos que quede negativo
            cout << VERDE << "  Stock ajustado (reversion COMPRA): "
                 << p.nombre << " -> " << p.stock << RST << endl;
        } else {
            // Si fue VENTA, el stock bajo -> lo subimos (devolucion)
            p.stock        += t.cantidades[i];
            p.totalVendido -= t.cantidades[i];
            if (p.totalVendido < 0) p.totalVendido = 0;
            cout << VERDE << "  Stock ajustado (devolucion VENTA): "
                 << p.nombre << " -> " << p.stock << RST << endl;
        }

        actualizarProductoEnDisco(idxProd, p);
    }

    // Si era VENTA, tambien descontamos el totalGastado del cliente
    if (strcmp(t.tipo, "VENTA") == 0) {
        int idxCli = buscarIndiceClientePorID(t.idRelacionado);
        if (idxCli != -1) {
            Cliente c = leerClientePorIndice(idxCli);
            c.totalGastado -= t.totalFactura;
            if (c.totalGastado < 0.0f) c.totalGastado = 0.0f;
            actualizarClienteEnDisco(idxCli, c);
            cout << VERDE << "  Total gastado del cliente actualizado."
                 << RST << endl;
        }
    }

    // Marcamos la transaccion como eliminada (borrado logico)
    t.eliminado = true;
    archT.seekp(posTrans, ios::beg);
    archT.write(reinterpret_cast<char*>(&t), sizeof(Transaccion));
    archT.close();

    h.registrosActivos--;
    actualizarHeader("transacciones.bin", h);

    cout << FVRD << "  Transaccion cancelada y stock revertido exitosamente."
         << RST << endl;
}


// Adaptacion de verificarIntegridadReferencial() del original, con chequeo completo de relaciones entre productos, proveedores, clientes y transacciones, y reporte detallado de errores encontrados.
void verificarIntegridadReferencial() {

    imprimirTitulo("DIAGNOSTICO DE INTEGRIDAD REFERENCIAL");
    int totalErrores = 0;

   // Verificacion 1: Productos -> Proveedores
    cout << CIAN << "  Verificando Productos -> Proveedores..." << RST << endl;

    ArchivoHeader hProd = leerHeader("productos.bin");
    ifstream fProd("productos.bin", ios::binary);

    if (fProd.is_open()) {
        fProd.seekg(sizeof(ArchivoHeader), ios::beg);
        for (int i = 0; i < hProd.cantidadRegistros; i++) {
            Producto p;
            fProd.read(reinterpret_cast<char*>(&p), sizeof(Producto));
            if (!p.eliminado && buscarIndiceProveedorPorID(p.idProveedor) == -1) {
                cout << ROJO << "  [ERROR] Producto '" << p.nombre
                     << "' (ID:" << p.id << ") -> proveedor ID "
                     << p.idProveedor << " NO existe." << RST << endl;
                totalErrores++;
            }
        }
        fProd.close();
    }

    // Verificacion 2: Transacciones -> Productos y Clientes/Proveedores
    cout << CIAN << "  Verificando Transacciones -> Entidades..." << RST << endl;

    ArchivoHeader hTrans = leerHeader("transacciones.bin");
    ifstream fTrans("transacciones.bin", ios::binary);

    if (fTrans.is_open()) {
        fTrans.seekg(sizeof(ArchivoHeader), ios::beg);
        for (int i = 0; i < hTrans.cantidadRegistros; i++) {
            Transaccion t;
            fTrans.read(reinterpret_cast<char*>(&t), sizeof(Transaccion));
            if (t.eliminado) continue;

            // Verificamos que cada producto referenciado exista
            for (int j = 0; j < t.numItems; j++) {
                if (buscarIndiceProductoPorID(t.idsProductos[j]) == -1) {
                    cout << ROJO << "  [ERROR] Transaccion #" << t.id
                         << " -> producto ID " << t.idsProductos[j]
                         << " NO existe." << RST << endl;
                    totalErrores++;
                }
            }

            // Verificamos el cliente o proveedor relacionado
            bool relOk;
            if (strcmp(t.tipo, "VENTA") == 0)
                relOk = (buscarIndiceClientePorID(t.idRelacionado) != -1);
            else
                relOk = (buscarIndiceProveedorPorID(t.idRelacionado) != -1);

            if (!relOk) {
                cout << ROJO << "  [ERROR] Transaccion #" << t.id
                     << " (" << t.tipo << ") -> "
                     << (strcmp(t.tipo, "VENTA") == 0 ? "cliente" : "proveedor")
                     << " ID " << t.idRelacionado << " NO existe."
                     << RST << endl;
                totalErrores++;
            }
        }
        fTrans.close();
    }

    imprimirLinea('-', 65);

    if (totalErrores == 0) {
        cout << FVRD << "  Sistema integro: no se encontraron referencias rotas."
             << RST << endl;
    } else {
        cout << FROJ << "  Total de errores encontrados: " << totalErrores
             << RST << endl;
    }
}

// Adaptacion de crearBackup() del original, con timestamp en el nombre del backup, chequeo de existencia de archivos y reporte detallado de resultados.
void crearBackup() {

    // Generamos un timestamp para el nombre del backup
    time_t ahora = time(0);
    struct tm* info = localtime(&ahora);
    char sufijo[25];
    strftime(sufijo, 25, "%Y%m%d_%H%M%S", info);

    const char* archivos[] = {
        "productos.bin",
        "clientes.bin",
        "proveedores.bin",
        "transacciones.bin",
        "tienda.bin"     // Faltaba en el original
    };

    int totalArch = 5;

    imprimirTitulo("RESPALDO DE DATOS (BACKUP)");
    cout << "  Timestamp: " << AMARI << sufijo << RST << endl;
    imprimirLinea('-', 65);

    int copiados = 0;

    for (int i = 0; i < totalArch; i++) {

        ifstream origen(archivos[i], ios::binary);

        if (!origen.is_open()) {
            cout << AMARI << "  [OMITIDO] " << archivos[i]
                 << " (no existe todavia)" << RST << endl;
            continue;
        }

        // Construimos el nombre del backup con timestamp
        char nombreBackup[120];
        sprintf(nombreBackup, "backup_%s_%s", sufijo, archivos[i]);

        ofstream destino(nombreBackup, ios::binary);

        if (!destino.is_open()) {
            cout << ROJO << "  [ERROR] No se pudo crear: " << nombreBackup
                 << RST << endl;
            origen.close();
            continue;
        }

        // Copiamos byte a byte usando rdbuf()
        destino << origen.rdbuf();

        origen.close();
        destino.close();

        cout << VERDE << "  [OK] " << RST << archivos[i]
             << "  ->  " << nombreBackup << endl;
        copiados++;
    }

    imprimirLinea('=', 65);
    cout << AZUL << "  Respaldo completado: "
         << copiados << "/" << totalArch << " archivos copiados." << RST << endl;
}

// Adaptacion de reporteStockCritico() del original, mostrando productos con stock menor o igual al minimo, e incluyendo informacion del proveedor, con formato tabular y resaltado de los criticos.
void reporteStockCritico() {

    imprimirTitulo("REPORTE: PRODUCTOS CON STOCK CRITICO");

    ArchivoHeader h = leerHeader("productos.bin");
    if (h.registrosActivos == 0) {
        cout << AMARI << "  No hay productos registrados." << RST << endl;
        return;
    }

    cout << NEG << " " << left
         << setw(4)  << "ID"
         << setw(22) << " Nombre"
         << setw(9)  << " Stock"
         << setw(10) << " Minimo"
         << setw(22) << " Proveedor"
         << " Telefono" << RST << endl;
    imprimirLinea('-', 74);

    ifstream f("productos.bin", ios::binary);
    if (!f.is_open()) {
        cout << ROJO << "  Error al abrir el archivo." << RST << endl;
        return;
    }

    Producto p;
    bool hayAlguno = false;

    for (int i = 0; i < h.cantidadRegistros; i++) {
        f.seekg(calcularOffset(i, sizeof(Producto)), ios::beg);
        f.read(reinterpret_cast<char*>(&p), sizeof(Producto));

        if (p.eliminado) continue;

        if (p.stock <= p.stockMinimo) {
            // Buscamos el nombre y telefono del proveedor
            char nomProv[20] = "N/A";
            char telProv[20] = "N/A";
            int idxPr = buscarIndiceProveedorPorID(p.idProveedor);
            if (idxPr != -1) {
                Proveedor pr = leerProveedorPorIndice(idxPr);
                strncpy(nomProv, pr.nombre,   18); nomProv[18] = '\0';
                strncpy(telProv, pr.telefono, 18); telProv[18] = '\0';
            }

            cout << ROJO;
            cout << " " << setw(4)  << left << p.id
                 << setw(22) << left << p.nombre
                 << setw(9)  << left << p.stock
                 << setw(10) << left << p.stockMinimo
                 << setw(22) << left << nomProv
                 << telProv;
            cout << RST << endl;

            hayAlguno = true;
        }
    }

    f.close();
    imprimirLinea('=', 74);

    if (!hayAlguno)
        cout << FVRD << "  Todo en orden! No hay productos en nivel critico." << RST << endl;
}

// Adaptacion de historialDetalladoCliente() del original, mostrando informacion completa del cliente y su historial de transacciones, con formato tabular y manejo de casos donde no hay transacciones o productos eliminados.
void historialDetalladoCliente() {

    imprimirTitulo("REPORTE: HISTORIAL DETALLADO DE CLIENTE");

    int idCli = leerEntero("  Ingrese el ID del cliente: ", 1, INT_MAX);
    int idxCli = buscarIndiceClientePorID(idCli);

    if (idxCli == -1) {
        cout << ROJO << "  Error: cliente no encontrado." << RST << endl;
        return;
    }

    Cliente c = leerClientePorIndice(idxCli);

    // Mostramos informacion del cliente
    cout << endl;
    cout << CIAN << "  ============================================" << RST << endl;
    cout << CIAN << "  CLIENTE:        " << RST << NEG << c.nombre << RST << endl;
    cout << CIAN << "  CEDULA:         " << RST << c.cedula << endl;
    cout << CIAN << "  EMAIL:          " << RST << c.email  << endl;
    cout << CIAN << "  TOTAL GASTADO:  " << RST << AMARI
         << "$" << fixed << setprecision(2) << c.totalGastado << RST << endl;
    cout << CIAN << "  TRANSACCIONES:  " << RST << c.numTransacciones << endl;
    cout << CIAN << "  ============================================" << RST << endl;

    if (c.numTransacciones == 0) {
        cout << AMARI << "  El cliente no tiene ventas registradas todavia."
             << RST << endl;
        return;
    }

    cout << endl;
    cout << NEG << "  " << left
         << setw(7)  << "Ticket"
         << setw(22) << " Producto"
         << setw(8)  << " Cant"
         << setw(12) << " Total $"
         << " Fecha" << RST << endl;
    imprimirLinea('-', 65);

    // Recorremos el historial de IDs de transacciones del cliente
    for (int i = 0; i < c.numTransacciones; i++) {

        int idTrans  = c.historialTransacciones[i];
        int idxTrans = buscarIndiceTransaccionPorID(idTrans);

        if (idxTrans == -1) {
            cout << AMARI << "  [Transaccion #" << idTrans
                 << " no encontrada]" << RST << endl;
            continue;
        }

        // Leemos la transaccion completa del disco
        ifstream fT("transacciones.bin", ios::binary);
        if (!fT.is_open()) continue;

        fT.seekg(calcularOffset(idxTrans, sizeof(Transaccion)), ios::beg);
        Transaccion t;
        fT.read(reinterpret_cast<char*>(&t), sizeof(Transaccion));
        fT.close();

        char bufF[11];
        struct tm* tInfo = localtime(&t.fecha);
        strftime(bufF, 11, "%Y-%m-%d", tInfo);

        // Mostramos CADA producto de la transaccion (multi-producto)
        for (int j = 0; j < t.numItems; j++) {

            int idxProd = buscarIndiceProductoPorID(t.idsProductos[j]);
            char nomProd[22] = "(eliminado)";

            if (idxProd != -1) {
                Producto p = leerProductoPorIndice(idxProd);
                strncpy(nomProd, p.nombre, 21);
                nomProd[21] = '\0';
            }

            cout << "  #" << setw(5)  << left << t.id
                 << setw(22) << left << nomProd
                 << setw(8)  << left << t.cantidades[j]
                 << "$" << setw(11) << left << fixed << setprecision(2) << t.totalFactura
                 << bufF << endl;
        }
    }

    imprimirLinea('=', 65);
    cout << AMARI << "  Total de transacciones vinculadas: "
         << c.numTransacciones << RST << endl;
}


int main() {

    //inicializamos los archivos con header si no existen, para evitar errores al leerlos posteriormente

    inicializarArchivo("productos.bin");
    inicializarArchivo("proveedores.bin");
    inicializarArchivo("clientes.bin");
    inicializarArchivo("transacciones.bin");
    inicializarArchivo("tienda.bin");

    char opcionPrincipal = ' ';

    do {
        system("cls");

        // Pantalla de bienvenida con colores
        cout << endl << AZUL << NEG;
        cout << "  ==============================================================" << endl;
        cout << "      SISTEMA DE GESTION DE INVENTARIO  v2.0                   " << endl;
        cout << "      Tienda: La Bodeguita 2.0                                 " << endl;
        cout << "  ==============================================================" << endl;
        cout << RST << endl;

        cout << "  " << CIAN  << "1." << RST << " Gestion de Productos"     << endl;
        cout << "  " << CIAN  << "2." << RST << " Gestion de Proveedores"   << endl;
        cout << "  " << CIAN  << "3." << RST << " Gestion de Clientes"      << endl;
        cout << "  " << CIAN  << "4." << RST << " Gestion de Transacciones" << endl;
        cout << "  " << MAGEN << "5." << RST << " Mantenimiento y Reportes" << endl;
        cout << "  " << ROJO  << "6." << RST << " Salir del Sistema"        << endl;

        cout << endl << "  Seleccione una opcion: ";
        cin >> opcionPrincipal;
        limpiarBuffer();

        // opcion 1: Gestion de Productos
        if (opcionPrincipal == '1') {

            int opP = -1;
            do {
                system("cls");
                cout << endl << AZUL << NEG;
                cout << "  =========================================" << endl;
                cout << "      GESTION DE PRODUCTOS               " << endl;
                cout << "  =========================================" << endl;
                cout << RST << endl;

                cout << "  " << CIAN << "1." << RST << " Registrar nuevo producto"   << endl;
                cout << "  " << CIAN << "2." << RST << " Buscar producto"             << endl;
                cout << "  " << CIAN << "3." << RST << " Editar producto"             << endl;
                cout << "  " << CIAN << "4." << RST << " Ajustar stock manualmente"  << endl;
                cout << "  " << CIAN << "5." << RST << " Eliminar producto"           << endl;
                cout << "  " << CIAN << "6." << RST << " Listar todos los productos" << endl;
                cout << "  " << ROJO  << "0." << RST << " Volver al menu principal"   << endl;

                cout << endl;
                opP = leerEntero("  Seleccione: ", 0, 6);
                cout << endl;

                if      (opP == 1) crearProducto();
                else if (opP == 2) buscarProducto();
                else if (opP == 3) actualizarProducto();
                else if (opP == 4) ajusteStockProducto();
                else if (opP == 5) eliminarProducto();
                else if (opP == 6) listarProductos();

                if (opP != 0) pausar();

            } while (opP != 0);
        }

        //opcion 2: Gestion de Proveedores
        else if (opcionPrincipal == '2') {

            int opProv = -1;
            do {
                system("cls");
                cout << endl << AZUL << NEG;
                cout << "  =========================================" << endl;
                cout << "      GESTION DE PROVEEDORES             " << endl;
                cout << "  =========================================" << endl;
                cout << RST << endl;

                cout << "  " << CIAN << "1." << RST << " Registrar nuevo proveedor" << endl;
                cout << "  " << CIAN << "2." << RST << " Buscar proveedor"          << endl;
                cout << "  " << CIAN << "3." << RST << " Actualizar proveedor"      << endl;
                cout << "  " << CIAN << "4." << RST << " Eliminar proveedor"        << endl;
                cout << "  " << CIAN << "5." << RST << " Listar proveedores"        << endl;
                cout << "  " << ROJO  << "0." << RST << " Volver al menu principal"  << endl;

                cout << endl;
                opProv = leerEntero("  Seleccione: ", 0, 5);
                cout << endl;

                if      (opProv == 1) crearProveedor();
                else if (opProv == 2) buscarProveedor();
                else if (opProv == 3) actualizarProveedor();
                else if (opProv == 4) eliminarProveedor();
                else if (opProv == 5) listarProveedores();

                if (opProv != 0) pausar();

            } while (opProv != 0);
        }

       //opcion 3: Gestion de Clientes
        else if (opcionPrincipal == '3') {

            int opCli = -1;
            do {
                system("cls");
                cout << endl << AZUL << NEG;
                cout << "  =========================================" << endl;
                cout << "      GESTION DE CLIENTES               " << endl;
                cout << "  =========================================" << endl;
                cout << RST << endl;

                cout << "  " << CIAN << "1." << RST << " Registrar nuevo cliente"   << endl;
                cout << "  " << CIAN << "2." << RST << " Buscar cliente"             << endl;
                cout << "  " << CIAN << "3." << RST << " Actualizar datos"           << endl;
                cout << "  " << CIAN << "4." << RST << " Listar todos los clientes" << endl;
                cout << "  " << CIAN << "5." << RST << " Eliminar cliente"           << endl;
                cout << "  " << ROJO  << "0." << RST << " Volver al menu principal"  << endl;

                cout << endl;
                opCli = leerEntero("  Seleccione: ", 0, 5);
                cout << endl;

                if      (opCli == 1) crearCliente();
                else if (opCli == 2) buscarCliente();
                else if (opCli == 3) actualizarCliente();
                else if (opCli == 4) listarClientes();
                else if (opCli == 5) eliminarCliente();

                if (opCli != 0) pausar();

            } while (opCli != 0);
        }

       //opcion 4: Gestion de Transacciones
        else if (opcionPrincipal == '4') {

            int opT = -1;
            do {
                system("cls");
                cout << endl << AZUL << NEG;
                cout << "  =========================================" << endl;
                cout << "      GESTION DE TRANSACCIONES           " << endl;
                cout << "  =========================================" << endl;
                cout << RST << endl;

                cout << "  " << CIAN  << "1." << RST << " Registrar Compra (a Proveedor)" << endl;
                cout << "  " << CIAN  << "2." << RST << " Registrar Venta (a Cliente)"    << endl;
                cout << "  " << CIAN  << "3." << RST << " Buscar Transacciones"           << endl;
                cout << "  " << CIAN  << "4." << RST << " Listar Transacciones"           << endl;
                cout << "  " << ROJO  << "5." << RST << " Cancelar Transaccion"           << endl;
                cout << "  " << ROJO  << "0." << RST << " Volver al menu principal"       << endl;

                cout << endl;
                opT = leerEntero("  Seleccione: ", 0, 5);
                cout << endl;

                if      (opT == 1) registrarCompraBinaria();
                else if (opT == 2) registrarVentaBinaria();
                else if (opT == 3) buscarTransacciones();
                else if (opT == 4) listarTransacciones();
                else if (opT == 5) cancelarTransaccion();

                if (opT != 0) pausar();

            } while (opT != 0);
        }

       //opcion 5: Mantenimiento y Reportes
        else if (opcionPrincipal == '5') {

            int opM = -1;
            do {
                system("cls");
                cout << endl << MAGEN << NEG;
                cout << "  =========================================" << endl;
                cout << "      MANTENIMIENTO Y REPORTES           " << endl;
                cout << "  =========================================" << endl;
                cout << RST << endl;

                cout << "  " << MAGEN << "1." << RST << " Verificar Integridad Referencial" << endl;
                cout << "  " << MAGEN << "2." << RST << " Crear Respaldo (Backup)"          << endl;
                cout << "  " << MAGEN << "3." << RST << " Reporte de Stock Critico"         << endl;
                cout << "  " << MAGEN << "4." << RST << " Historial Detallado de Cliente"   << endl;
                cout << "  " << ROJO   << "0." << RST << " Volver al menu principal"        << endl;

                cout << endl;
                opM = leerEntero("  Seleccione: ", 0, 4);
                cout << endl;

                if      (opM == 1) verificarIntegridadReferencial();
                else if (opM == 2) crearBackup();
                else if (opM == 3) reporteStockCritico();
                else if (opM == 4) historialDetalladoCliente();

                if (opM != 0) pausar();

            } while (opM != 0);
        }

        else if (opcionPrincipal == '6') {
            cout << endl << VERDE << NEG
                 << "  Hasta luego! Gracias por usar el sistema." << RST << endl;
        }

        else if (opcionPrincipal != '6') {
            cout << endl << ROJO
                 << "  Opcion no valida. Seleccione del 1 al 6."
                 << RST << endl;
            pausar();
        }

    } while (opcionPrincipal != '6');

    return 0;
}


