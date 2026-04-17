#ifndef CLIENTE_HPP
#define CLIENTE_HPP

#include <ctime>
#include "../persistencia/Constantes.hpp"

/* ========================================================================
    ENTIDAD: CLIENTE
    Descripcion: Representa a un cliente del sistema. Maneja sus datos
     personales, estado de cuenta y vinculo con transacciones.
   ======================================================================== */

class Cliente {
private:
    /* ---- Atributos Privados (Encapsulamiento) ---- */
    int    id;
    char   nombre[Constantes::TAM_NOMBRE];      // Usando Constantes.hpp
    char   cedula[Constantes::TAM_CODIGO]; 
    char   telefono[Constantes::TAM_TELEFONO];
    char   email[Constantes::TAM_EMAIL];
    char   direccion[Constantes::TAM_DESCRIPCION];
    bool   eliminado;                           // Para borrado logico
    time_t fechaRegistro;
    float  totalGastado;
    
    // Relacion con Transacciones
    int    historialTransacciones[Constantes::MAX_COMPRAS_CLIENTE];
    int    numTransacciones;

public:
    /* ---- Constructores y Destructor ---- */
    Cliente();
    Cliente(const char* nom, const char* ced, const char* tel, const char* mail, const char* dir);
    Cliente(const Cliente& otro); // Constructor de copia obligatorio
    ~Cliente();

    /* ---- Getters (Siempre const para proteccion de datos) ---- */
    int getId() const;
    const char* getNombre() const;
    const char* getCedula() const;
    const char* getTelefono() const;
    const char* getEmail() const;
    const char* getDireccion() const;
    bool isEliminado() const;
    time_t getFechaRegistro() const;
    float getTotalGastado() const;
    int getNumTransacciones() const;
    int getTransaccionId(int indice) const;
    
    /* ---- Setters (Retornan bool para confirmar si el dato fue aceptado) ---- */
    void setId(int nuevoId);
    bool setNombre(const char* nuevoNombre);
    bool setCedula(const char* nuevaCedula);
    bool setTelefono(const char* nuevoTelefono);
    bool setEmail(const char* nuevoEmail);
    bool setDireccion(const char* nuevaDireccion);
    void setEliminado(bool estado);
    
    /* ---- Metodos de Logica de Negocio ---- */
    void registrarGasto(float monto);
    bool agregarTransaccionID(int idTransaccion); // Maneja el historial interno

    /* ---- Metodos de Presentacion ---- */
    void mostrarInformacionBasica() const;
    void mostrarInformacionCompleta() const;

    /* ---- Metodos de Validacion Propia ---- */
    bool tieneDatosCompletos() const;
    
    /* ---- Metodo Estatico (Para el Gestor de Archivos) ---- */
    // Segun la imagen image_e77bed.png, debe retornar el sizeof de la clase.
    static int obtenerTamano();
};

#endif // CLIENTE_HPP