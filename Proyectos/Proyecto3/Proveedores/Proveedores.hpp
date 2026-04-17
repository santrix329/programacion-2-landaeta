#ifndef PROVEEDOR_HPP
#define PROVEEDOR_HPP

#include "../Persistencia/Constantes.hpp"
#include <cstring>
#include <iostream>

class Proveedor {
private:
    int id;
    char nombreEmpresa[Constantes::TAM_NOMBRE];
    char rif[Constantes::TAM_CODIGO];
    char telefono[Constantes::TAM_TELEFONO];
    char email[Constantes::TAM_EMAIL];           // <-- Faltaba esto
    char direccion[Constantes::TAM_DESCRIPCION];
    char personaContacto[Constantes::TAM_NOMBRE]; // <-- Faltaba esto
    bool eliminado;

public:
    // Constructores
    Proveedor();
    // Este constructor debe tener 6 parámetros de texto para registrarProveedor
    Proveedor(const char* empresa, const char* numRif, const char* tel, const char* mail, const char* dir, const char* contacto);
    Proveedor(const Proveedor& otro); // Constructor copia (lo pide el compilador)
    ~Proveedor(); // Destructor

    // Getters
    int getId() const;
    const char* getNombreEmpresa() const;
    const char* getRif() const;
    const char* getEmail() const;
    bool isEliminado() const;

    // Setters (Cambiados a bool para que coincidan con tus validaciones del .cpp)
    void setId(int id);
    bool setNombreEmpresa(const char* nombre);
    bool setRif(const char* rif);
    bool setTelefono(const char* tlf);
    bool setEmail(const char* mail);
    bool setDireccion(const char* dir);
    bool setPersonaContacto(const char* contacto);
    void setEliminado(bool estado);

    // Métodos de presentación (Ajustados a los nombres que usas en el resto del código)
    void mostrarInformacionBasica() const;
    void mostrarInformacionCompleta() const;
    
    bool esValido() const;
    size_t obtenerTamano();
};

#endif