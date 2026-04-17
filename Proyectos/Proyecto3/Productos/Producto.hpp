#ifndef PRODUCTO_HPP
#define PRODUCTO_HPP


#include <ctime>


static const int MAX_HISTORIAL_VENTAS = 50;

class Producto {
private:
    /* ---- Identificacion ---- */
    int    id;
    char   codigo[20];
    char   nombre[100];
    char   descripcion[200];

    /* ---- Relacion con proveedor */
    int    idProveedor;    

    /* ---- Datos comerciales ---- */
    float  precio;
    int    stock;
    int    stockMinimo;    
    int    totalVendido;        

    /* ---- Control de ciclo de vida ---- */
    bool   eliminado;            /* Flag de borrado logico            */
    time_t fechaCreacion;
    time_t fechaUltimaModificacion;

    /* ---- Historial de ventas ---- */
    int    historialVentas[MAX_HISTORIAL_VENTAS]; /* IDs de transacciones */
    int    cantidadVentas;       /* Cuantos slots del historial estan ocupados */

public:
    
    Producto();

    
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