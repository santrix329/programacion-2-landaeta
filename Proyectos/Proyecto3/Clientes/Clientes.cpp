#include "Cliente.hpp"
#include <iostream>
#include <cstring>
#include <iomanip>

using namespace std;

// --- Constructores y Destructor ---

Cliente::Cliente() {
    id = 0;
    nombre[0] = '\0';
    cedula[0] = '\0';
    telefono[0] = '\0';
    email[0] = '\0';
    direccion[0] = '\0';
    eliminado = false;
    fechaRegistro = time(0);
    totalGastado = 0.0f;
    numTransacciones = 0;
    for (int i = 0; i < 50; ++i) {
        historialTransacciones[i] = 0;
    }
}

Cliente::Cliente(const char* nom, const char* ced, const char* tel, const char* mail, const char* dir) : Cliente() {
    setNombre(nom);
    setCedula(ced);
    setTelefono(tel);
    setEmail(mail);
    setDireccion(dir);
}

Cliente::Cliente(const Cliente& otro) {
    id = otro.id;
    strncpy(nombre, otro.nombre, sizeof(nombre));
    strncpy(cedula, otro.cedula, sizeof(cedula));
    strncpy(telefono, otro.telefono, sizeof(telefono));
    strncpy(email, otro.email, sizeof(email));
    strncpy(direccion, otro.direccion, sizeof(direccion));
    eliminado = otro.eliminado;
    fechaRegistro = otro.fechaRegistro;
    totalGastado = otro.totalGastado;
    numTransacciones = otro.numTransacciones;
    for (int i = 0; i < 50; ++i) {
        historialTransacciones[i] = otro.historialTransacciones[i];
    }
}

Cliente::~Cliente() {
    // No hay memoria dinámica (new) que liberar manualmente en esta clase
}

// --- Getters ---

int Cliente::getId() const { return id; }
const char* Cliente::getNombre() const { return nombre; }
const char* Cliente::getCedula() const { return cedula; }
const char* Cliente::getTelefono() const { return telefono; }
const char* Cliente::getEmail() const { return email; }
const char* Cliente::getDireccion() const { return direccion; }
bool Cliente::isEliminado() const { return eliminado; }
time_t Cliente::getFechaRegistro() const { return fechaRegistro; }
float Cliente::getTotalGastado() const { return totalGastado; }
int Cliente::getNumTransacciones() const { return numTransacciones; }

// --- Setters ---

void Cliente::setId(int nuevoId) {
    if (nuevoId >= 0) id = nuevoId;
}

bool Cliente::setNombre(const char* nuevoNombre) {
    if (nuevoNombre == nullptr || strlen(nuevoNombre) == 0) return false;
    strncpy(nombre, nuevoNombre, sizeof(nombre) - 1);
    nombre[sizeof(nombre) - 1] = '\0';
    return true;
}

bool Cliente::setCedula(const char* nuevaCedula) {
    if (nuevaCedula == nullptr || strlen(nuevaCedula) == 0) return false;
    strncpy(cedula, nuevaCedula, sizeof(cedula) - 1);
    cedula[sizeof(cedula) - 1] = '\0';
    return true;
}

bool Cliente::setTelefono(const char* nuevoTelefono) {
    if (nuevoTelefono == nullptr) return false;
    strncpy(telefono, nuevoTelefono, sizeof(telefono) - 1);
    telefono[sizeof(telefono) - 1] = '\0';
    return true;
}

bool Cliente::setEmail(const char* nuevoEmail) {
    if (nuevoEmail == nullptr) return false;
    strncpy(email, nuevoEmail, sizeof(email) - 1);
    email[sizeof(email) - 1] = '\0';
    return true;
}

bool Cliente::setDireccion(const char* nuevaDireccion) {
    if (nuevaDireccion == nullptr) return false;
    strncpy(direccion, nuevaDireccion, sizeof(direccion) - 1);
    direccion[sizeof(direccion) - 1] = '\0';
    return true;
}

void Cliente::setEliminado(bool estado) {
    eliminado = estado;
}

void Cliente::agregarGasto(float monto) {
    if (monto > 0) {
        totalGastado += monto;
    }
}

// --- Métodos de gestión de relaciones ---

bool Cliente::agregarTransaccionID(int idTransaccion) {
    if (numTransacciones < 50 && idTransaccion > 0) {
        historialTransacciones[numTransacciones] = idTransaccion;
        numTransacciones++;
        return true;
    }
    return false; // El historial está lleno o el ID es inválido
}

// --- Métodos de presentación ---

void Cliente::mostrarInformacionBasica() const {
    cout << left << setw(4) << id 
         << setw(22) << nombre 
         << setw(14) << cedula 
         << setw(14) << telefono 
         << "$" << setw(13) << fixed << setprecision(2) << totalGastado 
         << email << endl;
}

void Cliente::mostrarInformacionCompleta() const {
    cout << "  ID:            " << id << "    Cedula: " << cedula << endl;
    cout << "  Nombre:        " << nombre << endl;
    cout << "  Telefono:      " << telefono << "    Email: " << email << endl;
    cout << "  Direccion:     " << direccion << endl;
    cout << "  Total Gastado: $" << fixed << setprecision(2) << totalGastado;
    cout << "    Transacciones: " << numTransacciones << endl;
}

// --- Métodos de validación ---

bool Cliente::esValido() const {
    // Un cliente es válido mínimamente si tiene nombre y cédula
    return (strlen(nombre) > 0 && strlen(cedula) > 0);
}

// --- Método estático ---

size_t Cliente::obtenerTamano() {
    return sizeof(Cliente);
}