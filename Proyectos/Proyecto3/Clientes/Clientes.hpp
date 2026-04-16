#ifndef CLIENTE_HPP
#define CLIENTE_HPP

#include <ctime>
#include <cstddef> // Para size_t

class Cliente {
private:
    int    id;
    char   nombre[100];
    char   cedula[20];
    char   telefono[20];
    char   email[100];
    char   direccion[200];
    bool   eliminado;
    time_t fechaRegistro;
    float  totalGastado;
    int    historialTransacciones[50];
    int    numTransacciones;

public:
    // Constructores y Destructor
    Cliente();
    Cliente(const char* nom, const char* ced, const char* tel, const char* mail, const char* dir);
    Cliente(const Cliente& otro); // Constructor de copia
    ~Cliente();

    // Getters (siempre const)
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
    
    // Setters (con validación básica)
    void setId(int nuevoId);
    bool setNombre(const char* nuevoNombre);
    bool setCedula(const char* nuevaCedula);
    bool setTelefono(const char* nuevoTelefono);
    bool setEmail(const char* nuevoEmail);
    bool setDireccion(const char* nuevaDireccion);
    void setEliminado(bool estado);
    void agregarGasto(float monto);

    // Métodos de gestión de relaciones
    bool agregarTransaccionID(int idTransaccion);

    // Métodos de presentación
    void mostrarInformacionBasica() const;
    void mostrarInformacionCompleta() const;

    // Métodos de validación
    bool esValido() const;

    // Método estático
    static size_t obtenerTamano();
};

#endif // CLIENTE_HPP