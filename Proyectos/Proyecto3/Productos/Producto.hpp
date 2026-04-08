#ifndef PRODUCTO_HPP
#define PRODUCTO_HPP

/*
 * ================================================================
 *   SISTEMA DE GESTION DE INVENTARIO  v3.0
 *   productos/Producto.hpp
 *
 *   Declaracion de la clase Producto.
 *   Evolucion del struct Producto del Proyecto 2:
 *     - Atributos privados con getters/setters
 *     - Constructores, destructor y constructor de copia
 *     - Metodos de validacion propios de la entidad
 *     - Metodos de presentacion (mostrarBasico / mostrarCompleto)
 *     - Metodos de gestion del historial de ventas
 *     - Metodo estatico obtenerTamano()
 *
 *   REGLA: Esta clase NO conoce archivos ni menus.
 *          Esa responsabilidad es de GestorArchivos e Interfaz.
 * ================================================================
 */

#include <ctime>

/* Capacidades maximas (igual que el Proyecto 2) */
static const int MAX_HISTORIAL_VENTAS = 50;

class Producto {
private:
    /* ---- Identificacion ---- */
    int    id;
    char   codigo[20];
    char   nombre[100];
    char   descripcion[200];

    /* ---- Relacion con proveedor ---- */
    int    idProveedor;          /* Llave foranea -> proveedores.bin */

    /* ---- Datos comerciales ---- */
    float  precio;
    int    stock;
    int    stockMinimo;          /* Umbral de alerta de stock critico */
    int    totalVendido;         /* Acumulado historico de ventas     */

    /* ---- Control de ciclo de vida ---- */
    bool   eliminado;            /* Flag de borrado logico            */
    time_t fechaCreacion;
    time_t fechaUltimaModificacion;

    /* ---- Historial de ventas ---- */
    int    historialVentas[MAX_HISTORIAL_VENTAS]; /* IDs de transacciones */
    int    cantidadVentas;       /* Cuantos slots del historial estan ocupados */

public:
    /* ============================================================
       CONSTRUCTORES Y DESTRUCTOR
       ============================================================ */

    /* Constructor por defecto: inicializa todo a cero / vacio */
    Producto();

    /* Constructor parametrizado: campos minimos para crear un producto valido */
    Producto(const char* nombre,
             const char* codigo,
             const char* descripcion,
             int         idProveedor,
             float       precio,
             int         stock,
             int         stockMinimo);

    /* Constructor de copia */
    Producto(const Producto& otro);

    /* Destructor */
    ~Producto();

    /* ============================================================
       GETTERS  (todos const: no modifican el objeto)
       ============================================================ */

    int          getId()                    const;
    const char*  getCodigo()                const;
    const char*  getNombre()                const;
    const char*  getDescripcion()           const;
    int          getIdProveedor()           const;
    float        getPrecio()               const;
    int          getStock()                 const;
    int          getStockMinimo()           const;
    int          getTotalVendido()          const;
    bool         isEliminado()             const;
    time_t       getFechaCreacion()        const;
    time_t       getFechaUltimaModificacion() const;
    int          getCantidadVentas()       const;

    /* Acceso al historial: retorna el ID en la posicion dada (-1 si invalida) */
    int          getVentaEnPosicion(int pos) const;

    /* ============================================================
       SETTERS  (con validacion: retornan false si el valor es invalido)
       ============================================================ */

    bool setId(int id);
    bool setCodigo(const char* codigo);
    bool setNombre(const char* nombre);
    bool setDescripcion(const char* descripcion);
    bool setIdProveedor(int idProveedor);
    bool setPrecio(float precio);
    bool setStock(int stock);
    bool setStockMinimo(int stockMinimo);
    void setTotalVendido(int totalVendido);
    void setEliminado(bool eliminado);
    void setFechaCreacion(time_t fecha);
    void setFechaUltimaModificacion(time_t fecha);

    /* ============================================================
       METODOS DE GESTION DEL HISTORIAL DE VENTAS
       ============================================================ */

    /* Agrega el ID de una transaccion al historial (false si esta lleno) */
    bool agregarTransaccionID(int idTransaccion);

    /* Elimina un ID de transaccion del historial (false si no existe) */
    bool eliminarTransaccionID(int idTransaccion);

    /* Devuelve true si el historial esta lleno */
    bool historialLleno() const;

    /* ============================================================
       METODOS DE VALIDACION (reglas de negocio de la entidad)
       ============================================================ */

    /* Verifica que los campos obligatorios no esten vacios y sean coherentes */
    bool esDatosCompletos() const;

    /* Verifica si el stock esta en nivel critico (<= stockMinimo) */
    bool esStockCritico() const;

    /* Verifica si el stock esta bajo (entre stockMinimo y 2*stockMinimo) */
    bool esStockBajo() const;

    /* ============================================================
       METODOS DE PRESENTACION
       ============================================================ */

    /* Muestra una linea resumen (para listados) */
    void mostrarInformacionBasica() const;

    /* Muestra el detalle completo del producto (para busquedas) */
    void mostrarInformacionCompleta() const;

    /* ============================================================
       METODO ESTATICO
       ============================================================ */

    /* Retorna sizeof(Producto): lo usa GestorArchivos para calcular offsets */
    static int obtenerTamano();
};

#endif /* PRODUCTO_HPP */