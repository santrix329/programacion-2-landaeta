#include "Proveedor.hpp"
#include <iostream>
#include <cstring>
#include <iomanip>

using namespace std;

// --- Constructores y Destructor ---

Proveedor::Proveedor() {
    id = 0;
    nombreEmpresa[0] = '\0';
    rif[0] = '\0';
    telefono[0] = '\0';
    email[0] = '\0';
    direccion[0] = '\0';
    personaContacto[0] = '\0';
    eliminado = false;
}

Proveedor::Proveedor(const char* empresa, const char* numRif, const char* tel, const char* mail, const char* dir, const char* contacto) : Proveedor() {
    setNombreEmpresa(empresa);
    setRif(numRif);
    setTelefono(tel);
    setEmail(mail);
    setDireccion(dir);
    setPersonaContacto(contacto);
}

Proveedor::Proveedor(const Proveedor& otro) {
    id = otro.id;
    strncpy(nombreEmpresa, otro.nombreEmpresa, sizeof(nombreEmpresa));
    strncpy(rif, otro.rif, sizeof(rif));
    strncpy(telefono, otro.telefono, sizeof(telefono));
    strncpy(email, otro.email, sizeof(email));
    strncpy(direccion, otro.direccion, sizeof(direccion));
    strncpy(personaContacto, otro.personaContacto, sizeof(personaContacto));
    eliminado = otro.eliminado;
}

Proveedor::~Proveedor() {
    // Sin memoria dinámica que liberar
}

// --- Getters ---

int Proveedor::getId() const { return id; }
const char* Proveedor::getNombreEmpresa() const { return nombreEmpresa; }
const char* Proveedor::getRif() const { return rif; }
const char* Proveedor::getTelefono() const { return telefono; }
const char* Proveedor::getEmail() const { return email; }
const char* Proveedor::getDireccion() const { return direccion; }
const char* Proveedor::getPersonaContacto() const { return personaContacto; }
bool Proveedor::isEliminado() const { return eliminado; }

// --- Setters ---

void Proveedor::setId(int nuevoId) {
    if (nuevoId >= 0) id = nuevoId;
}

bool Proveedor::setNombreEmpresa(const char* nuevoNombre) {
    if (nuevoNombre == nullptr || strlen(nuevoNombre) == 0) return false;
    strncpy(nombreEmpresa, nuevoNombre, sizeof(nombreEmpresa) - 1);
    nombreEmpresa[sizeof(nombreEmpresa) - 1] = '\0';
    return true;
}

bool Proveedor::setRif(const char* nuevoRif) {
    if (nuevoRif == nullptr || strlen(nuevoRif) == 0) return false;
    strncpy(rif, nuevoRif, sizeof(rif) - 1);
    rif[sizeof(rif) - 1] = '\0';
    return true;
}

bool Proveedor::setTelefono(const char* nuevoTelefono) {
    if (nuevoTelefono == nullptr) return false;
    strncpy(telefono, nuevoTelefono, sizeof(telefono) - 1);
    telefono[sizeof(telefono) - 1] = '\0';
    return true;
}

bool Proveedor::setEmail(const char* nuevoEmail) {
    if (nuevoEmail == nullptr) return false;
    strncpy(email, nuevoEmail, sizeof(email) - 1);
    email[sizeof(email) - 1] = '\0';
    return true;
}

bool Proveedor::setDireccion(const char* nuevaDireccion) {
    if (nuevaDireccion == nullptr) return false;
    strncpy(direccion, nuevaDireccion, sizeof(direccion) - 1);
    direccion[sizeof(direccion) - 1] = '\0';
    return true;
}

bool Proveedor::setPersonaContacto(const char* nuevoContacto) {
    if (nuevoContacto == nullptr) return false;
    strncpy(personaContacto, nuevoContacto, sizeof(personaContacto) - 1);
    personaContacto[sizeof(personaContacto) - 1] = '\0';
    return true;
}

void Proveedor::setEliminado(bool estado) {
    eliminado = estado;
}

// --- Métodos de presentación ---

void Proveedor::mostrarInformacionBasica() const {
    cout << left << setw(4) << id 
         << setw(25) << nombreEmpresa 
         << setw(15) << rif 
         << setw(15) << telefono 
         << personaContacto << endl;
}

void Proveedor::mostrarInformacionCompleta() const {
    cout << "  ID:              " << id << endl;
    cout << "  Empresa:         " << nombreEmpresa << endl;
    cout << "  RIF/ID:          " << rif << endl;
    cout << "  Contacto:        " << personaContacto << endl;
    cout << "  Telefono:        " << telefono << endl;
    cout << "  Email:           " << email << endl;
    cout << "  Direccion:       " << direccion << endl;
}

// --- Métodos de validación ---

bool Proveedor::esValido() const {
    // Un proveedor requiere al menos el nombre de la empresa y su identificador fiscal
    return (strlen(nombreEmpresa) > 0 && strlen(rif) > 0);
}

// --- Método estático ---

size_t Proveedor::obtenerTamano() {
    return sizeof(Proveedor);
}