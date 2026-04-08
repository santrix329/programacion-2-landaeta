/*
 * ================================================================
 *   SISTEMA DE GESTION DE INVENTARIO  v3.0
 *   productos/Producto.cpp
 *
 *   Implementacion de la clase Producto.
 *   Toda la logica esta contenida aqui; el .hpp solo declara.
 * ================================================================
 */

#include "Producto.hpp"

#include <iostream>   /* cout, endl, fixed, setprecision            */
#include <iomanip>    /* setw, left                                  */
#include <cstring>    /* strcpy, strncpy, strlen, memset, strcmp     */
#include <ctime>      /* time_t, time()                              */

using namespace std;

/* ================================================================
   CODIGOS DE COLOR ANSI  (mismos que el Proyecto 2)
   ================================================================ */
#define RST    "\033[0m"
#define NEG    "\033[1m"
#define ROJO   "\033[31m"
#define VERDE  "\033[32m"
#define AMARI  "\033[33m"
#define AZUL   "\033[34m"
#define CIAN   "\033[36m"

/* Funcion auxiliar interna: imprime una linea de caracteres repetidos */
static void imprimirLineaProducto(char c, int ancho) {
    for (int i = 0; i < ancho; i++) cout << c;
    cout << endl;
}


/* ================================================================
   CONSTRUCTORES Y DESTRUCTOR
   ================================================================ */

/*
 * Constructor por defecto
 * Pone todos los campos en su estado "limpio": 0, false, cadenas vacias.
 * Es el equivalente del memset(&p, 0, sizeof(Producto)) del Proyecto 2.
 */
Producto::Producto() {
    id          = 0;
    idProveedor = 0;
    precio      = 0.0f;
    stock       = 0;
    stockMinimo = 0;
    totalVendido = 0;
    eliminado   = false;
    fechaCreacion          = 0;
    fechaUltimaModificacion = 0;
    cantidadVentas = 0;

    memset(codigo,      0, sizeof(codigo));
    memset(nombre,      0, sizeof(nombre));
    memset(descripcion, 0, sizeof(descripcion));
    memset(historialVentas, 0, sizeof(historialVentas));
}

/*
 * Constructor parametrizado
 * Permite crear un Producto con los campos minimos en una sola linea.
 * La fecha de creacion se asigna automaticamente al momento actual.
 */
Producto::Producto(const char* pNombre,
                   const char* pCodigo,
                   const char* pDescripcion,
                   int         pIdProveedor,
                   float       pPrecio,
                   int         pStock,
                   int         pStockMinimo)
{
    /* Inicializamos todo a 0 primero para no dejar basura */
    id          = 0;
    idProveedor = 0;
    precio      = 0.0f;
    stock       = 0;
    stockMinimo = 0;
    totalVendido = 0;
    eliminado   = false;
    cantidadVentas = 0;
    memset(codigo,         0, sizeof(codigo));
    memset(nombre,         0, sizeof(nombre));
    memset(descripcion,    0, sizeof(descripcion));
    memset(historialVentas, 0, sizeof(historialVentas));

    /* Usamos los setters para que apliquen la validacion */
    setNombre(pNombre);
    setCodigo(pCodigo);
    setDescripcion(pDescripcion);
    setIdProveedor(pIdProveedor);
    setPrecio(pPrecio);
    setStock(pStock);
    setStockMinimo(pStockMinimo);

    /* Fecha de creacion = ahora */
    fechaCreacion           = time(nullptr);
    fechaUltimaModificacion = fechaCreacion;
}

/*
 * Constructor de copia
 * Copia byte a byte todos los atributos, incluido el arreglo de historial.
 */
Producto::Producto(const Producto& otro) {
    id          = otro.id;
    idProveedor = otro.idProveedor;
    precio      = otro.precio;
    stock       = otro.stock;
    stockMinimo = otro.stockMinimo;
    totalVendido = otro.totalVendido;
    eliminado   = otro.eliminado;
    fechaCreacion           = otro.fechaCreacion;
    fechaUltimaModificacion = otro.fechaUltimaModificacion;
    cantidadVentas          = otro.cantidadVentas;

    strncpy(codigo,      otro.codigo,      sizeof(codigo)      - 1);
    strncpy(nombre,      otro.nombre,      sizeof(nombre)      - 1);
    strncpy(descripcion, otro.descripcion, sizeof(descripcion) - 1);
    codigo[sizeof(codigo)           - 1] = '\0';
    nombre[sizeof(nombre)           - 1] = '\0';
    descripcion[sizeof(descripcion) - 1] = '\0';

    for (int i = 0; i < MAX_HISTORIAL_VENTAS; i++) {
        historialVentas[i] = otro.historialVentas[i];
    }
}

/*
 * Destructor
 * No hay memoria dinamica, pero se declara para cumplir el requisito
 * y como buena practica de diseno (la clase es extensible).
 */
Producto::~Producto() {
    /* Sin recursos dinamicos que liberar */
}


/* ================================================================
   GETTERS
   ================================================================ */

int         Producto::getId()                       const { return id;          }
const char* Producto::getCodigo()                   const { return codigo;      }
const char* Producto::getNombre()                   const { return nombre;      }
const char* Producto::getDescripcion()              const { return descripcion; }
int         Producto::getIdProveedor()              const { return idProveedor; }
float       Producto::getPrecio()                  const { return precio;      }
int         Producto::getStock()                    const { return stock;       }
int         Producto::getStockMinimo()              const { return stockMinimo; }
int         Producto::getTotalVendido()             const { return totalVendido;}
bool        Producto::isEliminado()                const { return eliminado;   }
time_t      Producto::getFechaCreacion()           const { return fechaCreacion;            }
time_t      Producto::getFechaUltimaModificacion() const { return fechaUltimaModificacion;  }
int         Producto::getCantidadVentas()          const { return cantidadVentas;           }

int Producto::getVentaEnPosicion(int pos) const {
    if (pos < 0 || pos >= cantidadVentas) return -1;
    return historialVentas[pos];
}


/* ================================================================
   SETTERS
   ================================================================ */

bool Producto::setId(int pId) {
    if (pId < 0) return false;   /* 0 es valido: significa "sin asignar" */
    id = pId;
    return true;
}

bool Producto::setCodigo(const char* pCodigo) {
    if (pCodigo == nullptr || strlen(pCodigo) == 0) return false;
    if (strlen(pCodigo) >= sizeof(codigo))           return false;
    strncpy(codigo, pCodigo, sizeof(codigo) - 1);
    codigo[sizeof(codigo) - 1] = '\0';
    fechaUltimaModificacion = time(nullptr);
    return true;
}

bool Producto::setNombre(const char* pNombre) {
    if (pNombre == nullptr || strlen(pNombre) == 0) return false;
    if (strlen(pNombre) >= sizeof(nombre))           return false;
    strncpy(nombre, pNombre, sizeof(nombre) - 1);
    nombre[sizeof(nombre) - 1] = '\0';
    fechaUltimaModificacion = time(nullptr);
    return true;
}

bool Producto::setDescripcion(const char* pDescripcion) {
    if (pDescripcion == nullptr) return false;
    /* La descripcion puede estar vacia, pero no puede desbordar el buffer */
    if (strlen(pDescripcion) >= sizeof(descripcion)) return false;
    strncpy(descripcion, pDescripcion, sizeof(descripcion) - 1);
    descripcion[sizeof(descripcion) - 1] = '\0';
    fechaUltimaModificacion = time(nullptr);
    return true;
}

bool Producto::setIdProveedor(int pIdProveedor) {
    if (pIdProveedor <= 0) return false;  /* Los IDs validos empiezan en 1 */
    idProveedor = pIdProveedor;
    fechaUltimaModificacion = time(nullptr);
    return true;
}

bool Producto::setPrecio(float pPrecio) {
    if (pPrecio <= 0.0f) return false;   /* El precio debe ser positivo */
    precio = pPrecio;
    fechaUltimaModificacion = time(nullptr);
    return true;
}

bool Producto::setStock(int pStock) {
    if (pStock < 0) return false;         /* El stock no puede ser negativo */
    stock = pStock;
    fechaUltimaModificacion = time(nullptr);
    return true;
}

bool Producto::setStockMinimo(int pStockMinimo) {
    if (pStockMinimo < 0) return false;
    stockMinimo = pStockMinimo;
    fechaUltimaModificacion = time(nullptr);
    return true;
}

/* totalVendido es un acumulador: no validamos el valor, solo lo asignamos */
void Producto::setTotalVendido(int pTotalVendido) {
    totalVendido = pTotalVendido;
}

void Producto::setEliminado(bool pEliminado) {
    eliminado = pEliminado;
    fechaUltimaModificacion = time(nullptr);
}

void Producto::setFechaCreacion(time_t fecha) {
    fechaCreacion = fecha;
}

void Producto::setFechaUltimaModificacion(time_t fecha) {
    fechaUltimaModificacion = fecha;
}


/* ================================================================
   GESTION DEL HISTORIAL DE VENTAS
   ================================================================ */

/*
 * agregarTransaccionID
 * Agrega el ID de una transaccion al historial circular del producto.
 * Retorna false si el historial esta lleno (50 transacciones).
 */
bool Producto::agregarTransaccionID(int idTransaccion) {
    if (idTransaccion <= 0)                    return false;
    if (cantidadVentas >= MAX_HISTORIAL_VENTAS) return false;

    historialVentas[cantidadVentas] = idTransaccion;
    cantidadVentas++;
    totalVendido++;
    fechaUltimaModificacion = time(nullptr);
    return true;
}

/*
 * eliminarTransaccionID
 * Busca el ID en el historial y lo elimina desplazando los siguientes.
 * Retorna false si el ID no estaba en el historial.
 */
bool Producto::eliminarTransaccionID(int idTransaccion) {
    int pos = -1;
    for (int i = 0; i < cantidadVentas; i++) {
        if (historialVentas[i] == idTransaccion) {
            pos = i;
            break;
        }
    }

    if (pos == -1) return false; /* No encontrado */

    /* Desplazamos los elementos a la izquierda para cerrar el hueco */
    for (int i = pos; i < cantidadVentas - 1; i++) {
        historialVentas[i] = historialVentas[i + 1];
    }
    historialVentas[cantidadVentas - 1] = 0;
    cantidadVentas--;
    fechaUltimaModificacion = time(nullptr);
    return true;
}

bool Producto::historialLleno() const {
    return cantidadVentas >= MAX_HISTORIAL_VENTAS;
}


/* ================================================================
   METODOS DE VALIDACION
   ================================================================ */

/*
 * esDatosCompletos
 * Verifica que los campos obligatorios esten presentes y sean coherentes.
 * No verifica la existencia del proveedor: eso es responsabilidad
 * de la capa de operaciones, que tiene acceso a los archivos.
 */
bool Producto::esDatosCompletos() const {
    if (strlen(nombre)  == 0) return false;
    if (strlen(codigo)  == 0) return false;
    if (idProveedor <= 0)     return false;
    if (precio <= 0.0f)       return false;
    if (stock < 0)            return false;
    if (stockMinimo < 0)      return false;
    return true;
}

bool Producto::esStockCritico() const {
    return stock <= stockMinimo;
}

bool Producto::esStockBajo() const {
    return stock > stockMinimo && stock <= (stockMinimo * 2);
}


/* ================================================================
   METODOS DE PRESENTACION
   ================================================================ */

/*
 * mostrarInformacionBasica
 * Una linea de tabla: ID | Codigo | Nombre | Precio | Stock
 * Formato identico al listado del Proyecto 2 (listarProductos).
 * Nota: el nombre del proveedor no se muestra aqui porque esta clase
 *       no tiene acceso a archivos. La capa de operaciones debe
 *       resolverlo antes de llamar a este metodo si lo necesita.
 */
void Producto::mostrarInformacionBasica() const {
    /* Color del stock segun nivel */
    const char* colorStock = VERDE;
    const char* etqStock   = "";
    if      (esStockCritico()) { colorStock = ROJO;  etqStock = " [!]"; }
    else if (esStockBajo())    { colorStock = AMARI; etqStock = " [v]"; }

    cout << " " << left
         << setw(4)  << id
         << setw(11) << codigo
         << setw(22) << nombre
         << AMARI << "$" << setw(9) << fixed << setprecision(2) << precio << RST
         << colorStock << setw(6) << stock << etqStock << RST
         << "  min:" << stockMinimo
         << endl;
}

/*
 * mostrarInformacionCompleta
 * Ficha completa del producto, equivalente a imprimirProductoDetallado()
 * del Proyecto 2. No muestra el nombre del proveedor porque esta clase
 * no lee archivos: la capa superior debe pasarlo si lo necesita.
 */
void Producto::mostrarInformacionCompleta() const {
    /* Color del stock segun nivel */
    const char* colorStock = VERDE;
    const char* etqStock   = "";
    if      (esStockCritico()) { colorStock = ROJO;  etqStock = " [CRITICO]"; }
    else if (esStockBajo())    { colorStock = AMARI; etqStock = " [BAJO]";    }

    /* Fecha de creacion formateada */
    char bufFecha[30];
    struct tm* tmInfo = localtime(&fechaCreacion);
    strftime(bufFecha, sizeof(bufFecha), "%d/%m/%Y %H:%M", tmInfo);

    char bufMod[30];
    struct tm* tmMod = localtime(&fechaUltimaModificacion);
    strftime(bufMod, sizeof(bufMod), "%d/%m/%Y %H:%M", tmMod);

    imprimirLineaProducto('-', 65);
    cout << CIAN  << "  ID:          " << RST << NEG << id  << RST
         << "    " << CIAN << "Codigo: " << RST << codigo   << endl;
    cout << CIAN  << "  Nombre:      " << RST << NEG << nombre << RST << endl;
    cout << CIAN  << "  Descripcion: " << RST << descripcion   << endl;
    cout << CIAN  << "  Precio:      " << RST << AMARI
         << "$" << fixed << setprecision(2) << precio << RST;
    cout << "    " << CIAN << "Stock: " << RST
         << colorStock << stock << etqStock << RST;
    cout << "    " << CIAN << "Minimo: " << RST << stockMinimo << endl;
    cout << CIAN  << "  ID Proveedor:" << RST << " " << idProveedor << endl;
    cout << CIAN  << "  Total vendido:" << RST << " " << totalVendido
         << "    " << CIAN << "Ventas en historial: " << RST << cantidadVentas << endl;
    cout << CIAN  << "  Creado:      " << RST << bufFecha
         << "    " << CIAN << "Modificado: " << RST << bufMod << endl;
    imprimirLineaProducto('-', 65);
}


/* ================================================================
   METODO ESTATICO
   ================================================================ */

/*
 * obtenerTamano
 * Retorna sizeof(Producto).
 * GestorArchivos lo usa para calcular el offset de cada registro:
 *   offset = sizeof(ArchivoHeader) + (indice * Producto::obtenerTamano())
 */
int Producto::obtenerTamano() {
    return static_cast<int>(sizeof(Producto));
}