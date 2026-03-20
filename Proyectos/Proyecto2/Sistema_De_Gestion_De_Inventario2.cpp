#include <iostream>
#include <cstring>
#include <string>
#include <iomanip>
#include <ctime>
#include <cstdlib>
#include <cctype>
#include <fstream>
#include <cstddef>    /* offsetof() */
#include <climits>    /* INT_MAX, INT_MIN */

using namespace std;

/* ================================================================
   CODIGOS DE COLOR ANSI (Seccion 6.1 del enunciado)
   Le indican a la terminal que cambie el color del texto.
   Formato: \033[ + codigo + m
   ================================================================ */

#define RST    "\033[0m"    /* Resetea al color por defecto        */
#define NEG    "\033[1m"    /* Negrita                              */
#define ROJO   "\033[31m"   /* Errores, advertencias criticas      */
#define VERDE  "\033[32m"   /* Exito, confirmaciones               */
#define AMARI  "\033[33m"   /* Datos importantes, preguntas        */
#define AZUL   "\033[34m"   /* Titulos de menus                    */
#define MAGEN  "\033[35m"   /* Menu de mantenimiento               */
#define CIAN   "\033[36m"   /* Etiquetas de campos en pantalla     */
#define FVRD   "\033[42m"   /* Fondo verde - mensaje de exito      */
#define FROJ   "\033[41m"   /* Fondo rojo  - error critico         */

/*
 * ArchivoHeader - va al inicio de cada archivo .bin
 * Ocupa exactamente 4 * sizeof(int) = 16 bytes.
 */
struct ArchivoHeader {
    int cantidadRegistros;   /* Total historico (incluye borrados)  */
    int proximoID;           /* ID que tendra el siguiente registro */
    int registrosActivos;    /* Solo los no eliminados              */
    int version;             /* Version del formato del archivo     */
};

/*
 * Producto - articulo del inventario
 */
struct Producto {
    int    id;
    char   codigo[20];
    char   nombre[100];
    char   descripcion[200];
    int    idProveedor;              /* Llave foranea -> proveedores.bin */
    float  precio;
    int    stock;
    int    stockMinimo;              /* Umbral de alerta de stock critico */
    int    totalVendido;             /* Acumulado historico de ventas     */
    bool   eliminado;                /* Flag de borrado logico            */
    time_t fechaCreacion;
    time_t fechaUltimaModificacion;
    int    historialVentas[50];      /* IDs de transacciones del producto */
    int    cantidadVentas;           /* Cuantos slots del historial usados*/
};

/*
 * Proveedor - empresa que nos vende mercancia
 */
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

/*
 * Cliente - persona que nos compra productos
 */
struct Cliente {
    int    id;
    char   nombre[100];
    char   cedula[20];
    char   telefono[20];
    char   email[100];
    char   direccion[200];
    bool   eliminado;
    time_t fechaRegistro;
    float  totalGastado;             /* Suma acumulada de todas sus compras   */
    int    historialTransacciones[50]; /* IDs de ventas vinculadas al cliente */
    int    numTransacciones;           /* Cuantos slots del historial usados   */
};

/*
 * Transaccion - registro de una compra o venta
 * Soporta multi-producto: hasta 15 productos por operacion.
 */
struct Transaccion {
    int    id;
    char   tipo[10];         /* "COMPRA" o "VENTA"                       */
    int    idRelacionado;    /* ID del cliente (VENTA) o proveedor (COMPRA)*/
    int    idsProductos[15]; /* IDs de los productos involucrados         */
    int    cantidades[15];   /* Cantidad de cada producto                 */
    int    numItems;         /* Cuantos de los 15 slots se usan           */
    float  totalFactura;
    time_t fecha;
    char   observaciones[200];
    bool   eliminado;
};

/*
 * Tienda - datos globales de la empresa (un solo registro en tienda.bin)
 */
struct Tienda {
    char  nombre[100];
    char  rif[20];
    float ventasTotales;
    float comprasTotales;
    int   totalTransaccionesRealizadas;
};
/* 
   PROTOTIPOS DE FUNCIONES
   Necesarios porque algunas funciones se llaman entre si antes
   de estar definidas en el archivo.
    */

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

/*
 * inicializarArchivo
 * Si el archivo no existe, lo crea con un header en cero.
 * Si ya existe, no hace nada y devuelve true.
 */
bool inicializarArchivo(const char* nombreArchivo) {

    /* Intentamos abrirlo en lectura para ver si existe */
    ifstream prueba(nombreArchivo, ios::binary);

    if (prueba.good()) {
        /* Ya existe, cerramos y salimos sin hacer nada */
        prueba.close();
        return true;
    }
    prueba.close();

    /* El archivo no existe, lo creamos con su header inicial */
    ofstream nuevo(nombreArchivo, ios::binary);

    if (!nuevo.is_open()) {
        cout << ROJO << "  [ERROR CRITICO] No se pudo crear: "
             << nombreArchivo << RST << endl;
        return false;
    }

    /* Header inicial con todos los contadores en cero */
    ArchivoHeader h;
    h.cantidadRegistros = 0;
    h.proximoID         = 1;   /* IDs arrancan desde 1 */
    h.registrosActivos  = 0;
    h.version           = 1;

    nuevo.write(reinterpret_cast<char*>(&h), sizeof(ArchivoHeader));
    nuevo.close();

    return true;
}

/*
 * leerHeader
 * Abre el archivo y devuelve los primeros 16 bytes como ArchivoHeader.
 * Si el archivo no existe, lo crea primero.
 */
ArchivoHeader leerHeader(const char* nombreArchivo) {

    /* Valores seguros por defecto */
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

/*
 * actualizarHeader
 * Sobrescribe SOLO los primeros 16 bytes del archivo.
 * Los registros de datos que vienen despues quedan intactos.
 */
bool actualizarHeader(const char* nombreArchivo, ArchivoHeader h) {

    /* ios::in|ios::out evita que se borre el contenido del archivo */
    fstream archivo(nombreArchivo, ios::binary | ios::in | ios::out);

    if (!archivo.is_open()) {
        return false;
    }

    archivo.seekp(0, ios::beg); /* Inicio absoluto del archivo */
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


/* ================================================================
   LECTURA DE REGISTROS POR INDICE FISICO
   Saltan al byte exacto con seekg y traen solo ese registro.
   ================================================================ */

Producto leerProductoPorIndice(int indice) {
    Producto p;
    memset(&p, 0, sizeof(Producto)); /* Limpiamos para evitar basura */

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


/* ================================================================
   ESCRITURA EN DISCO (actualizacion de registros existentes)
   Usa seekp para posicionarse en el byte exacto y sobreescribir
   solo ese registro, sin tocar los demas.
   ================================================================ */

void actualizarProductoEnDisco(int indice, Producto p) {
    /* Actualizamos la fecha de modificacion antes de guardar */
    p.fechaUltimaModificacion = time(0);

    fstream f("productos.bin", ios::binary | ios::in | ios::out);
    if (f.is_open()) {
        f.seekp(calcularOffset(indice, sizeof(Producto)), ios::beg);
        f.write(reinterpret_cast<char*>(&p), sizeof(Producto));
        f.close();
    } else {
        cout << ROJO << "  [ERROR] No se pudo abrir productos.bin para guardar."
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
        cout << ROJO << "  [ERROR] No se pudo abrir proveedores.bin para guardar."
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
        cout << ROJO << "  [ERROR] No se pudo abrir clientes.bin para guardar."
             << RST << endl;
    }
}


/* ================================================================
   REGISTRO DE NUEVOS ELEMENTOS - APPEND (Seccion 3.3)

   Logica de append:
    1. Leer el header para saber donde va el nuevo slot y que ID asignar
    2. Calcular offset = sizeof(Header) + (cantidadRegistros * sizeof(Struct))
    3. Escribir el nuevo registro en ese offset
    4. Incrementar contadores y guardar el header actualizado
   ================================================================ */

bool registrarProducto(Producto nuevo) {

    ArchivoHeader h = leerHeader("productos.bin");

    /* Asignamos el ID y los valores que van por defecto */
    nuevo.id                      = h.proximoID;
    nuevo.eliminado               = false;
    nuevo.fechaCreacion           = time(0);
    nuevo.fechaUltimaModificacion = time(0);
    nuevo.totalVendido            = 0;
    nuevo.cantidadVentas          = 0;

    /* Inicializamos el historial de ventas con ceros */
    for (int i = 0; i < 50; i++) {
        nuevo.historialVentas[i] = 0;
    }

    /* Calculamos el offset del siguiente slot disponible */
    long posNuevo = calcularOffset(h.cantidadRegistros, sizeof(Producto));

    fstream f("productos.bin", ios::binary | ios::in | ios::out);
    if (!f.is_open()) {
        cout << ROJO << "  [ERROR] No se pudo abrir productos.bin para registrar."
             << RST << endl;
        return false;
    }

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
        cout << ROJO << "  [ERROR] No se pudo abrir proveedores.bin para registrar."
             << RST << endl;
        return false;
    }

    f.seekp(posNuevo, ios::beg);
    f.write(reinterpret_cast<char*>(&nuevo), sizeof(Proveedor));
    f.close();

    h.cantidadRegistros++;
    h.proximoID++;
    h.registrosActivos++;

    return actualizarHeader("proveedores.bin", h);
}

/*
 * registrarCliente
 * CORRECCIONES vs el original:
 *  - Guarda TODOS los campos (el original solo guardaba nombre y cedula)
 *  - Usa la formula de offset, no seekp(0, ios::end)
 *  - Actualiza registrosActivos en el header (el original no lo hacia)
 *  - Inicializa el historial de transacciones con ceros
 */
bool registrarCliente(Cliente nuevo) {

    ArchivoHeader h = leerHeader("clientes.bin");

    nuevo.id               = h.proximoID;
    nuevo.eliminado        = false;
    nuevo.fechaRegistro    = time(0);
    nuevo.totalGastado     = 0.0f;
    nuevo.numTransacciones = 0;

    /* Limpiamos el historial de transacciones */
    for (int i = 0; i < 50; i++) {
        nuevo.historialTransacciones[i] = 0;
    }

    long posNuevo = calcularOffset(h.cantidadRegistros, sizeof(Cliente));

    fstream f("clientes.bin", ios::binary | ios::in | ios::out);
    if (!f.is_open()) {
        cout << ROJO << "  [ERROR] No se pudo abrir clientes.bin para registrar."
             << RST << endl;
        return false;
    }

    f.seekp(posNuevo, ios::beg);
    f.write(reinterpret_cast<char*>(&nuevo), sizeof(Cliente));
    f.close();

    h.cantidadRegistros++;
    h.proximoID++;
    h.registrosActivos++;

    return actualizarHeader("clientes.bin", h);
}

/*
 * registrarTransaccion
 * Retorna el ID asignado (necesario para vincularlo al historial del cliente
 * en el Paso 6 de registrarVentaBinaria). Retorna -1 si fallo.
 */
int registrarTransaccion(Transaccion nueva) {

    ArchivoHeader h = leerHeader("transacciones.bin");

    nueva.id        = h.proximoID;
    nueva.eliminado = false;
    nueva.fecha     = time(0);

    long posNueva = calcularOffset(h.cantidadRegistros, sizeof(Transaccion));

    fstream f("transacciones.bin", ios::binary | ios::in | ios::out);
    if (!f.is_open()) {
        cout << ROJO << "  [ERROR] No se pudo abrir transacciones.bin para registrar."
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

    return nueva.id; /* Retornamos el ID para el historial del cliente */
}
/* ================================================================
   FUNCIONES DE UTILIDAD GENERAL
   ================================================================ */

/*
 * limpiarBuffer
 * Limpia el buffer de entrada de cin despues de un error.
 * Sin esto, si el usuario escribe letras donde se espera un numero,
 * el programa queda en un bucle infinito sin poder leer mas entrada.
 */
void limpiarBuffer() {
    cin.clear();               /* Limpiamos la bandera de error */            //me quedé aquí
    cin.ignore(INT_MAX, '\n'); /* Descartamos hasta el proximo Enter */
}

/*
 * leerEntero
 * Lee un entero validado. Si el usuario escribe algo invalido
 * o fuera del rango, pide que repita. Evita crasheos.
 */
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
            continue;
        }

        return valor;
    }
}

/*
 * leerFlotante
 * Igual que leerEntero pero para numeros decimales.
 */
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

/*
 * convertirAMinusculas
 * Convierte cada caracter de la cadena a minuscula.
 * Util para busquedas que no distingan mayusculas de minusculas.
 */
void convertirAMinusculas(char* cadena) {
    for (int i = 0; cadena[i] != '\0'; i++) {
        cadena[i] = (char)tolower((unsigned char)cadena[i]);
    }
}

/*
 * contieneSubstring
 * Devuelve true si 'busqueda' aparece dentro de 'texto'.
 * strstr devuelve un puntero a la primera ocurrencia, o NULL.
 */
bool contieneSubstring(const char* texto, const char* busqueda) {
    if (!texto || !busqueda) {
        return false;
    }
    return strstr(texto, busqueda) != NULL;
}

/*
 * obtenerFechaActual
 * Pone en 'buffer' la fecha del sistema en formato YYYY-MM-DD.
 * El buffer debe tener al menos 11 caracteres.
 */
void obtenerFechaActual(char* buffer) {
    time_t t        = time(0);
    struct tm* info = localtime(&t);
    strftime(buffer, 11, "%Y-%m-%d", info);
}

/*
 * validarEmail
 * Verifica que el email tenga '@' y '.' en posicion valida.
 */
bool validarEmail(const char* email) {
    if (!email || strlen(email) < 5) return false;

    const char* arroba = strchr(email, '@');
    if (!arroba || arroba == email) return false;

    const char* punto = strchr(arroba, '.');
    if (!punto || punto == arroba + 1) return false;
    if (*(punto + 1) == '\0') return false;

    return true;
}

/*
 * codigoDuplicado
 * Busca en productos.bin si ya existe un producto activo con ese codigo.
 */
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

/*
 * rifDuplicado
 * Verifica si ya hay un proveedor activo con ese RIF.
 */
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

/*
 * cedulaDuplicada
 * Verifica si ya hay un cliente activo con esa cedula.
 */
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


/* ================================================================
   FUNCIONES DE IMPRESION CON FORMATO (Seccion 6.1)
   ================================================================ */

/*
 * imprimirLinea
 * Imprime 'ancho' repeticiones del caracter dado y luego un salto.
 */
void imprimirLinea(char c, int ancho) {
    for (int i = 0; i < ancho; i++) cout << c;
    cout << endl;
}

/*
 * imprimirTitulo
 * Imprime un titulo centrado entre lineas de '=' con colores.
 */
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

/*
 * pausar
 * Espera que el usuario presione ENTER. Reemplaza system("pause")
 * para mayor compatibilidad con Linux y Windows.
 */
void pausar() {
    cout << endl << AMARI << "  Presione ENTER para continuar..." << RST;
    cin.ignore(INT_MAX, '\n');
    cin.get();
}

/*
 * imprimirProductoDetallado (Seccion 6.2 - Practicidad)
 * Muestra todos los datos de un producto.
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

/*
 * imprimirProveedorDetallado
 */
void imprimirProveedorDetallado(Proveedor p) {
    cout << CIAN  << "  ID:        " << RST << NEG << p.id << RST
         << "    " << CIAN << "RIF: " << RST << p.rif << endl;
    cout << CIAN  << "  Nombre:    " << RST << NEG << p.nombre << RST << endl;
    cout << CIAN  << "  Telefono:  " << RST << p.telefono
         << "    " << CIAN << "Email: " << RST << p.email << endl;
    cout << CIAN  << "  Direccion: " << RST << p.direccion << endl;
    imprimirLinea('-', 65);
}

/*
 * imprimirClienteDetallado
 */
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


/* ================================================================
   MODULO: GESTION DE PRODUCTOS
   ================================================================ */

/*
 * crearProducto
 * Solicita todos los datos, los valida y los guarda en productos.bin.
 * Mantiene la logica del original (pregunta S/N al inicio, permite
 * escribir "cancelar" en el nombre, muestra resumen antes de guardar).
 */
void crearProducto() {

    /* El original preguntaba S/N antes de empezar */
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

    /* ---- NOMBRE ---- */
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

    /* ---- CODIGO (con validacion de duplicados) ---- */
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

    /* ---- DESCRIPCION ---- */
    cout << "  Descripcion: ";
    cin.getline(nuevo.descripcion, 200);

    /* ---- PRECIO ---- */
    nuevo.precio = leerFlotante("  Precio (mayor que 0): $", 0.01f);

    /* ---- STOCK ---- */
    nuevo.stock = leerEntero("  Stock inicial (0 o mas): ", 0, INT_MAX);

    /* ---- STOCK MINIMO ---- */
    nuevo.stockMinimo = leerEntero("  Stock minimo de alerta: ", 0, INT_MAX);

    /* ---- PROVEEDOR (validando que exista en disco) ---- */
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

    /* Buscamos el nombre del proveedor para mostrarlo en el resumen */
    int idxProv = buscarIndiceProveedorPorID(nuevo.idProveedor);
    Proveedor provElegido = leerProveedorPorIndice(idxProv);

    /* ---- RESUMEN ANTES DE GUARDAR ---- */
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
            cout << FVRD << "  Exito: Producto guardado con ID "
                 << nuevo.id << RST << endl;
        } else {
            cout << FROJ << "  Error: no se pudo escribir en el disco."
                 << RST << endl;
        }
    } else {
        cout << AMARI << "  Producto descartado, no se guardo nada." << RST << endl;
    }
}
//me quede aqui 
/*
 * buscarProducto
 * Adaptacion de buscarProducto(Tienda*) y buscarProductosPorNombre(Tienda*)
 * del original comentado. 4 modos de busqueda, todos leen del archivo.
 */
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

    /* ---- OPCION 1: Por ID exacto ---- */
    if (op == 1) {
        int id = leerEntero("  ID del producto: ", 1, INT_MAX);
        int idx = buscarIndiceProductoPorID(id);
        if (idx != -1) {
            cout << endl;
            imprimirProductoDetallado(leerProductoPorIndice(idx));
            encontrado = true;
        }
    }

    /* ---- OPCION 2: Por nombre parcial ----
     * Adaptacion de buscarProductosPorNombre() del original.
     * En lugar de arreglo dinamico, leemos directamente del disco. */
    else if (op == 2) {
        char busq[100];
        cout << "  Nombre (o parte del nombre) a buscar: ";
        cin.getline(busq, 100);

        if (strlen(busq) == 0) {
            cout << ROJO << "  Error: el campo no puede estar vacio." << RST << endl;
            return;
        }

        /* Convertimos la busqueda a minusculas para no distinguir mayusculas */
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

            /* Copiamos el nombre en minusculas sin alterar el original */
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

    /* ---- OPCION 3: Por codigo parcial ---- */
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

    /* ---- OPCION 4: Listar por proveedor ---- */
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
/*
 * actualizarProducto
 * Adaptacion de actualizarProducto(Tienda*) del original comentado.
 * Carga el producto en un BORRADOR en memoria, permite editarlo
 * campo por campo y solo escribe al disco cuando el usuario elige
 * la opcion 7 GUARDAR (exactamente como en el original).
 */
void actualizarProducto() {

    ArchivoHeader h = leerHeader("productos.bin");
    if (h.registrosActivos == 0) {
        cout << AMARI << "  No hay productos para editar." << RST << endl;
        return;
    }

    int id = leerEntero("  ID del producto que desea modificar: ", 1, INT_MAX);

    /* Buscamos el indice fisico del producto en el archivo */
    int indiceEncontrado = buscarIndiceProductoPorID(id);

    if (indiceEncontrado == -1) {
        cout << ROJO << "  Error: no existe un producto con el ID "
             << id << "." << RST << endl;
        return;
    }

    /* Cargamos el producto en el borrador.
     * El disco NO cambia hasta que el usuario elija opcion 7. */
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
            /* Editar codigo con validacion de duplicados */
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
            /* Editar nombre */
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
            /* Editar descripcion */
            cout << "  Nueva Descripcion: ";
            cin.getline(borrador.descripcion, 200);
            cout << VERDE << "  Descripcion actualizada en el borrador." << RST << endl;

        } else if (opcion == 4) {
            /* Editar ID del proveedor con validacion de existencia */
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
            /* Editar precio */
            borrador.precio = leerFlotante("  Nuevo Precio: $", 0.01f);
            cout << VERDE << "  Precio actualizado en el borrador." << RST << endl;

        } else if (opcion == 6) {
            /* Editar stock minimo */
            borrador.stockMinimo = leerEntero("  Nuevo Stock Minimo: ", 0, INT_MAX);
            cout << VERDE << "  Stock minimo actualizado en el borrador." << RST << endl;

        } else if (opcion == 7) {
            /* Guardar el borrador en disco - pide confirmacion primero */
            char confirmar;
            cout << "  Aplicar todos los cambios permanentemente? (S/N): ";
            cin >> confirmar;
            limpiarBuffer();

            if (tolower(confirmar) == 's') {
                actualizarProductoEnDisco(indiceEncontrado, borrador);
                cout << FVRD << "  Cambios guardados exitosamente en disco."
                     << RST << endl;
                return; /* Salimos de la funcion igual que el original */
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

/*
 * ajusteStockProducto
 * Adaptacion de actualizarStockProducto(Tienda*) del original comentado.
 * Permite sumar o restar unidades al stock. Valida que no quede negativo.
 */
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

    /* Validamos que el stock resultante no sea negativo */
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

/*
 * listarProductos
 * Adaptacion de listarProductos(Tienda*) del original comentado.
 * Lee del archivo. Muestra el nombre del proveedor en lugar del ID.
 */
void listarProductos() {

    ArchivoHeader h = leerHeader("productos.bin");
    if (h.registrosActivos == 0) {
        cout << AMARI << "  No hay productos registrados." << RST << endl;
        return;
    }

    imprimirTitulo("LISTADO COMPLETO DE PRODUCTOS");

    /* Encabezado de la tabla */
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

        /* Buscamos el nombre del proveedor para mostrarlo (Seccion 6.2) */
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

        /* Stock en rojo si esta en nivel critico */
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

/*
 * eliminarProducto
 * Adaptacion de eliminarProducto(Tienda*) del original comentado.
 * En lugar de desplazar elementos del arreglo (que el original hacia),
 * usamos borrado logico: solo marcamos eliminado=true.
 * La confirmacion usa 1=SI / 0=NO igual que el original.
 */
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

    /* Confirmacion con entero como en el original */
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


/* ================================================================
   MODULO: GESTION DE PROVEEDORES
   ================================================================ */

/*
 * crearProveedor
 * Valida RIF no duplicado y email con formato correcto.
 * CORRECCIONES vs el original: usa formula de offset, no seekp(ios::end).
 * Actualiza registrosActivos en el header (el original no lo hacia).
 */
void crearProveedor() {

    Proveedor nuevo;
    memset(&nuevo, 0, sizeof(Proveedor));

    imprimirTitulo("REGISTRAR NUEVO PROVEEDOR");

    /* ---- NOMBRE ---- */
    do {
        cout << "  Nombre de la empresa: ";
        cin.getline(nuevo.nombre, 100);
        if (strlen(nuevo.nombre) == 0)
            cout << ROJO << "  Error: el nombre no puede estar vacio." << RST << endl;
    } while (strlen(nuevo.nombre) == 0);

    /* ---- RIF (validando duplicados) ---- */
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

    /* ---- TELEFONO ---- */
    cout << "  Telefono: ";
    cin.getline(nuevo.telefono, 20);

    /* ---- EMAIL (validando formato) ---- */
    do {
        cout << "  Email: ";
        cin.getline(nuevo.email, 100);
        if (!validarEmail(nuevo.email))
            cout << ROJO << "  Error: formato invalido. Ejemplo: empresa@correo.com"
                 << RST << endl;
    } while (!validarEmail(nuevo.email));

    /* ---- DIRECCION ---- */
    cout << "  Direccion: ";
    cin.getline(nuevo.direccion, 200);

    if (registrarProveedor(nuevo)) {
        cout << FVRD << "  Proveedor registrado con ID: " << nuevo.id << RST << endl;
    } else {
        cout << FROJ << "  Error: no se pudo guardar el proveedor." << RST << endl;
    }
}

/*
 * buscarProveedor
 * Adaptacion de buscarProveedor(Tienda*) del original comentado.
 * 3 modos de busqueda, todos leen de proveedores.bin.
 */
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
                break; /* RIF es unico, no seguimos buscando */
            }
        }
        f.close();
    }

    if (!encontrado)
        cout << AMARI << "  No se encontraron coincidencias." << RST << endl;
}

//me quede aqui

/*
 * actualizarProveedor
 * Adaptacion de actualizarProveedor(Tienda*) del original comentado.
 * Lee del disco, permite modificar y vuelve a escribir.
 * Si el campo se deja vacio, se conserva el valor anterior.
 */
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

    /* Pedimos cada campo y solo lo cambiamos si el usuario escribe algo */
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

/*
 * listarProveedores
 * Adaptacion de listarProveedores(Tienda*) del original comentado.
 */
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

/*
 * eliminarProveedor
 * Adaptacion de eliminarProveedor(Tienda*) del original comentado.
 * Borrado logico: solo marca eliminado=true.
 */
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
 * Recicla el formato visual del Proyecto 1, pero lee desde el archivo .bin
 * y utiliza búsquedas relacionales para mostrar información coherente.
 */
void listarTransacciones() {
    ifstream archivo("transacciones.bin", ios::binary);
    if (!archivo) {
        cout << "\nNo hay transacciones registradas o el archivo no existe." << endl;
        system("pause");
        return;
    }

    // Leemos el header para saber cuántos registros hay
    ArchivoHeader header;
    archivo.read(reinterpret_cast<char*>(&header), sizeof(ArchivoHeader));

    if (header.cantidadRegistros == 0) {
        cout << "\nNo hay transacciones registradas." << endl;
        archivo.close();
        system("pause");
        return;
    }

    cout << "\n==========================================================================================" << endl;
    cout << "                             LISTADO DE TRANSACCIONES (BINARIO)                           " << endl;
    cout << "==========================================================================================" << endl;
    cout << " ID |  Tipo  |    Producto    |    Relacionado    | Cant |    Total    |     Fecha      " << endl;
    cout << "----|--------|----------------|-------------------|------|-------------|----------------" << endl;

    Transaccion t;
    int contadorReal = 0;

    // Leemos registro por registro del archivo
    while (archivo.read(reinterpret_cast<char*>(&t), sizeof(Transaccion))) {
        if (t.eliminado) continue;

        // --- BUSQUEDA DE NOMBRES (PUNTO 4.1 COHERENCIA) ---
        // Buscamos el nombre del producto para que no sea solo un ID
        int idxP = buscarIndicePorID("productos.bin", t.idsProductos[0]);
        string nombreProd = (idxP != -1) ? leerProductoPorIndice(idxP).nombre : "Desconocido";

        // Buscamos el nombre del Cliente o Proveedor
        string nombreRel = "Desconocido";
        if (strcmp(t.tipo, "VENTA") == 0) {
            int idxC = buscarIndicePorID("clientes.bin", t.idRelacionado);
            if(idxC != -1) nombreRel = leerClientePorIndice(idxC).nombre;
        } else {
            // Aquí iría la lógica para proveedores cuando la tengas lista
            nombreRel = "Proveedor"; 
        }

        // --- TU FORMATO ORIGINAL ---
        cout << " " << setw(3) << left << t.id << "| "
             << setw(7) << t.tipo << "| "
             << setw(15) << nombreProd.substr(0, 14) << "| "
             << setw(18) << nombreRel.substr(0, 17) << "| "
             << setw(5) << t.cantidades[0] << "| "
             << setw(10) << fixed << setprecision(2) << t.totalFactura << " $| ";
        
        // Para la fecha, si la guardaste como time_t:
        char bufferFecha[11];
        tm* infoTiempo = localtime(&t.fecha);
        strftime(bufferFecha, 11, "%Y/%m/%d", infoTiempo);
        cout << bufferFecha << endl;

        contadorReal++;
    }

    cout << "==========================================================================================" << endl;
    cout << "Total de registros activos: " << contadorReal << endl;
    
    archivo.close();
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

/*
 * historialDetalladoCliente
 * Genera un reporte analitico cruzando datos de Clientes, Transacciones y Productos.
 */
void historialDetalladoCliente() {
    int idCli;
    cout << "\n--- REPORTE: HISTORIAL DETALLADO ---" << endl;
    cout << "Ingrese ID del cliente: "; 
    cin >> idCli;

    // 1. Buscar al cliente para obtener su arreglo de transacciones
    int idx = buscarIndicePorID("clientes.bin", idCli);
    if (idx == -1) {
        cout << "Error: Cliente no encontrado en la base de datos." << endl;
        return;
    }

    Cliente c = leerClientePorIndice(idx);
    
    cout << "\n===========================================" << endl;
    cout << " CLIENTE: " << c.nombre << endl;
    cout << " CEDULA:  " << c.cedula << endl;
    cout << " TOTAL GASTADO: " << c.totalGastado << "$" << endl;
    cout << "===========================================" << endl;

    if (c.numTransacciones == 0) {
        cout << "El cliente no tiene ventas registradas todavia." << endl;
        return;
    }

    // 2. Recorrer el historial de IDs del cliente
    for (int i = 0; i < c.numTransacciones; i++) {
        int idTransActual = c.historialTransacciones[i];
        
        // Buscar la transaccion en el archivo de transacciones
        int idxTrans = buscarIndicePorID("transacciones.bin", idTransActual);
        
        if (idxTrans != -1) {
            // Leer la transaccion completa desde disco
            fstream archT("transacciones.bin", ios::binary | ios::in);
            archT.seekg(sizeof(ArchivoHeader) + (idxTrans * sizeof(Transaccion)));
            Transaccion t;
            archT.read(reinterpret_cast<char*>(&t), sizeof(Transaccion));
            archT.close();

            // 3. Buscar el nombre del producto involucrado (Relacion cruzada)
            int idxProd = buscarIndicePorID("productos.bin", t.idsProductos[0]);
            string nombreProd = "Producto no encontrado";
            
            if (idxProd != -1) {
                Producto p = leerProductoPorIndice(idxProd);
                nombreProd = p.nombre;
            }

            // Imprimir la linea del reporte
            cout << "Ticket #" << t.id << " | "
                 << "Prod: " << nombreProd << " | "
                 << "Cant: " << t.cantidades[0] << " | "
                 << "Total: " << t.totalFactura << "$" << endl;
        }
    }
    cout << "===========================================" << endl;
}


void verificarIntegridadReferencial() {
    cout << "\n--- INICIANDO DIAGNOSTICO DE INTEGRIDAD ---" << endl;
    int errores = 0;

    // 1. Validar Productos -> Proveedores
    ArchivoHeader hP = leerHeader("productos.bin");
    ifstream archP("productos.bin", ios::binary);
    archP.seekg(sizeof(ArchivoHeader));
    for(int i = 0; i < hP.cantidadRegistros; i++) {
        Producto p;
        archP.read(reinterpret_cast<char*>(&p), sizeof(Producto));
        if(!p.eliminado && buscarIndicePorID("proveedores.bin", p.idProveedor) == -1) {
            cout << "Referencia Rota: Producto " << p.nombre << " no tiene proveedor valido" << endl;
            errores++;
        }
    }
    archP.close();

    // 2. Validar Transacciones -> Entidades
    ArchivoHeader hT = leerHeader("transacciones.bin");
    ifstream archT("transacciones.bin", ios::binary);
    archT.seekg(sizeof(ArchivoHeader));
    for(int i = 0; i < hT.cantidadRegistros; i++) {
        Transaccion t;
        archT.read(reinterpret_cast<char*>(&t), sizeof(Transaccion));
        if(!t.eliminado) {
            // Validar Producto en la transaccion
            if(buscarIndicePorID("productos.bin", t.idsProductos[0]) == -1) {
                cout << "Referencia Rota: Transaccion " << t.id << " apunta a producto inexistente" << endl;
                errores++;
            }
        }
    }
    archT.close();

    cout << "\nDiagnostico finalizado ,Errores encontrados: " << errores << "," << endl;
}

void crearBackup() {
    const char* archivos[] = {"productos.bin", "clientes.bin", "proveedores.bin", "transacciones.bin"};
    
    for(int i = 0; i < 4; i++) {
        ifstream origen(archivos[i], ios::binary);
        if(!origen) continue;

        string nombreBackup = "backup_";
        nombreBackup += archivos[i];

        ofstream destino(nombreBackup.c_str(), ios::binary);
        destino << origen.rdbuf(); // Copia byte a byte eficientemente

        origen.close();
        destino.close();
    }
    cout << "\nExito: Los 4 archivos operativos han sido respaldados" << endl;
}

void reporteStockCritico() {
    ifstream archivo("productos.bin", ios::binary);
    ArchivoHeader h = leerHeader("productos.bin");
    Producto p;
    bool hayCriticos = false;

    cout << "\n--- PRODUCTOS CON STOCK CRITICO ---" << endl;
    archivo.seekg(sizeof(ArchivoHeader));

    for(int i = 0; i < h.cantidadRegistros; i++) {
        archivo.read(reinterpret_cast<char*>(&p), sizeof(Producto));
        if(!p.eliminado && p.stock <= p.stockMinimo) {
            cout << "Alerta: " << p.nombre << " | Stock: " << p.stock 
                 << " | Minimo: " << p.stockMinimo << endl;
            hayCriticos = true;
        }
    }
    archivo.close();
    if(!hayCriticos) cout << "Todo en orden, no hay stock bajo" << endl;

}


int main() {
    
    inicializarArchivo("productos.bin");
    inicializarArchivo("clientes.bin");
    inicializarArchivo("transacciones.bin");
    inicializarArchivo("proveedores.bin");
    
    char opcionPrincipal = ' '; 

    do {
        system("cls");
        cout << "############################################################" << endl;
        cout << "#          SISTEMA DE GESTION DE INVENTARIO                #" << endl;
        cout << "#          Tienda: La Bodeguita 2.0                        #" << endl;
        cout << "############################################################" << endl;

        cout << "\n1. Gestion de Productos" << endl;
        cout << "2. Gestion de Proveedores" << endl;
        cout << "3. Gestion de Clientes" << endl;
        cout << "4. Gestion de Transacciones" << endl;
        cout << "5. Mantenimiento y Reportes" << endl;
        cout << "6. Salir" << endl;
        
        cout << "\nSeleccione una opcion: ";
        cin >> opcionPrincipal;
        cin.ignore(); 

        switch (opcionPrincipal) {
            case '1': { 
                int opP = -1;
                do {
                    system("cls");
                    cout << "--- GESTION DE PRODUCTOS ---" << endl;
                    cout << "1. Registrar" << endl;
                    cout << "2. Buscar" << endl;
                    cout << "3. Editar" << endl;
                    cout << "4. Stock" << endl;
                    cout << "5. Eliminar" << endl;
                    cout << "6. Listar" << endl;
                    cout << "0. Volver" << endl;
                    cout << "\nSeleccione: "; cin >> opP; cin.ignore();
                    
                    if(opP == 1) crearProducto();
                    else if(opP == 6) mostrarInventario("productos.bin");
                    
                    if(opP != 0) system("pause");
                } while (opP != 0);
                break;
            }
            
            case '2': { 
                int opProv = -1;
                do {
                    system("cls");
                    cout << "--- GESTION DE PROVEEDORES ---" << endl;
                    cout << "1. Registrar Proveedor" << endl;
                    cout << "2. Buscar Proveedor" << endl;
                    cout << "3. Actualizar Proveedor" << endl;
                    cout << "4. Eliminar Proveedor" << endl;
                    cout << "5. Listar Proveedores" << endl;
                    cout << "0. Volver" << endl;
                    cout << "\nSeleccione: "; cin >> opProv; cin.ignore();
                    
                    if(opProv == 1) crearProveedor();
                    if(opProv != 0) system("pause");
                } while (opProv != 0);
                break;
            }

            case '3': { 
                int opCli = -1;
                do {
                    system("cls");
                    cout << "--- MENU GESTION DE CLIENTES ---" << endl;
                    cout << "1. Registrar Nuevo Cliente" << endl;
                    cout << "2. Buscar Cliente (Cedula)" << endl;
                    cout << "3. Actualizar Datos" << endl;
                    cout << "4. Listar Todos los Clientes" << endl;
                    cout << "5. Eliminar Cliente" << endl;
                    cout << "0. Volver al Menu Principal" << endl;
                    cout << "\nSeleccione: "; cin >> opCli; cin.ignore();
                    
                    if(opCli == 1) crearCliente();
                    else if(opCli == 3) actualizarCliente();
                    if(opCli != 0) system("pause");
                } while (opCli != 0);
                break;
            }

            case '4': { 
                int opT = -1;
                do {
                    system("cls");
                    cout << "--- GESTION DE TRANSACCIONES ---" << endl;
                    cout << "1. Registrar Compra (A Proveedor)" << endl;
                    cout << "2. Registrar Venta (A Cliente)" << endl;
                    cout << "3. Buscar Transacciones" << endl;
                    cout << "4. Listar Transacciones" << endl;
                    cout << "5. Cancelar Transaccion" << endl;
                    cout << "0. Volver" << endl;
                    cout << "\nSeleccione: "; cin >> opT; cin.ignore();
                    
                    if(opT == 1) registrarCompraBinaria();
                    else if(opT == 2) registrarVentaBinaria();
                    else if(opT == 4) listarTransacciones(); 

                    if(opT != 0) system("pause");
                } while (opT != 0);
                break;
            }

            case '5': { 
                int opM = -1;
                do {
                    system("cls");
                    cout << "--- MANTENIMIENTO Y REPORTES ---" << endl;
                    cout << "1. Verificar Integridad Referencial" << endl;
                    cout << "2. Crear Respaldo de Datos (Backup)" << endl;
                    cout << "3. Reporte de Stock Critico" << endl;
                    cout << "4. Historial Detallado de Cliente" << endl;
                    cout << "0. Volver" << endl;
                    cout << "\nSeleccione: "; cin >> opM; cin.ignore();
                    
                    if(opM == 1) verificarIntegridadReferencial();
                    else if(opM == 2) crearBackup();
                    else if(opM == 3) reporteStockCritico();
                    else if(opM == 4) historialDetalladoCliente();

                    if(opM != 0) system("pause");
                } while (opM != 0);
                break;
            }

            case '6':
                cout << "\nSaliendo del sistema..." << endl;
                break;

            default:
                cout << "\nOpcion no valida" << endl;
                system("pause");
                break;
        }
    } while (opcionPrincipal != '6');

    return 0;
}