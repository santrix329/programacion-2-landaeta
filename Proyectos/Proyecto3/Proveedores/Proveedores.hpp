#ifndef PROVEEDOR_HPP
#define PROVEEDOR_HPP

#include <cstddef> // Para size_t

class Proveedor {
private:
    int  id;
    char nombreEmpresa[100];
    char rif[20]; // Registro de Información Fiscal o Identificación Tributaria
    char telefono[20];
    char email[100];
    char direccion[200];
    char personaContacto[100];
    bool eliminado;

public:
    // Constructores y Destructor
    Proveedor();
    Proveedor(const char* empresa, const char* numRif, const char* tel, const char* mail, const char* dir, const char* contacto);
    Proveedor(const Proveedor& otro); // Constructor de copia
    ~Proveedor();

    // Getters (constantes)
    int getId() const;
    const char* getNombreEmpresa() const;
    const char* getRif() const;
    const char* getTelefono() const;
    const char* getEmail() const;
    const char* getDireccion() const;
    const char* getPersonaContacto() const;
    bool isEliminado() const;

    // Setters (retornan bool para confirmar si la asignación fue válida)
    void setId(int nuevoId);
    bool setNombreEmpresa(const char* nuevoNombre);
    bool setRif(const char* nuevoRif);
    bool setTelefono(const char* nuevoTelefono);
    bool setEmail(const char* nuevoEmail);
    bool setDireccion(const char* nuevaDireccion);
    bool setPersonaContacto(const char* nuevoContacto);
    void setEliminado(bool estado);

    // Métodos de presentación
    void mostrarInformacionBasica() const;
    void mostrarInformacionCompleta() const;

    // Métodos de validación
    bool esValido() const;

    // Método estático para el GestorArchivos
    static size_t obtenerTamano();
};

#endif // PROVEEDOR_HPP