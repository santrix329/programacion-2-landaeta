#include "Cliente.hpp"
#include <iostream>
#include <cstring>
#include <iomanip>
#include <ctime>

using namespace std;

/* ========================================================================
    CONSTRUCTORES Y DESTRUCTOR
   ======================================================================== */

Cliente::Cliente() {
    id = 0;
    // Limpieza profunda de arreglos para evitar basura en el binario
    memset(nombre, 0, sizeof(nombre));
    memset(cedula, 0, sizeof(cedula));
    memset(telefono, 0, sizeof(telefono));
    memset(email, 0, sizeof(email));
    memset(direccion, 0, sizeof(direccion));
    
    eliminado = false;
    fechaRegistro = time(0);
    totalGastado = 0.0f;
    numTransacciones = 0;

    // Usamos la constante global para evitar "Magic Numbers"
    for (int i = 0; i < Constantes::MAX_COMPRAS_CLIENTE; ++i) {
        historialTransacciones[i] = 0;
    }
}

Cliente::Cliente(const char* nom, const char* ced, const char* tel, const char* mail, const char* dir) : Cliente() {
    // Usamos los setters para aprovechar las validaciones internas
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

    for (int i = 0; i < Constantes::MAX_COMPRAS_CLIENTE; ++i) {
        historialTransacciones[i] = otro.historialTransacciones[i];
    }
}

Cliente::~Cliente() {
    // Destructor simple ya que no hay punteros dinámicos
}

/* ========================================================================
    SETTERS CON VALIDACIÓN
   ======================================================================== */

void Cliente::setId(int nuevoId) {
    if (nuevoId >= 0) id = nuevoId;
}

bool Cliente::setNombre(const char* nuevoNombre) {
    if (nuevoNombre == nullptr || strlen(nuevoNombre) < 3) return false;
    strncpy(nombre, nuevoNombre, sizeof(nombre) - 1);
    nombre[sizeof(nombre) - 1] = '\0';
    return true;
}

bool Cliente::setCedula(const char* nuevaCedula) {
    if (nuevaCedula == nullptr || strlen(nuevaCedula) < 7) return false;
    strncpy(cedula, nuevaCedula, sizeof(cedula) - 1);
    cedula[sizeof(cedula) - 1] = '\0';
    return true;
}

bool Cliente::setTelefono(const char* nuevoTelefono) {
    if (nuevoTelefono == nullptr || strlen(nuevoTelefono) < 7) return false;
    strncpy(telefono, nuevoTelefono, sizeof(telefono) - 1);
    telefono[sizeof(telefono) - 1] = '\0';
    return true;
}

bool Cliente::setEmail(const char* nuevoEmail) {
    // Validación básica de presencia de arroba
    if (nuevoEmail == nullptr || strchr(nuevoEmail, '@') == nullptr) return false;
    strncpy(email, nuevoEmail, sizeof(email) - 1);
    email[sizeof(email) - 1] = '\0';
    return true;
}

bool Cliente::setDireccion(const char* nuevaDireccion) {
    if (nuevaDireccion == nullptr || strlen(nuevaDireccion) < 5) return false;
    strncpy(direccion, nuevaDireccion, sizeof(direccion) - 1);
    direccion[sizeof(direccion) - 1] = '\0';
    return true;
}

void Cliente::setEliminado(bool estado) {
    eliminado = estado;
}

/* ========================================================================
    LÓGICA DE NEGOCIO Y RELACIONES
   ======================================================================== */

void Cliente::registrarGasto(float monto) {
    if (monto > 0) {
        totalGastado += monto;
    }
}

bool Cliente::agregarTransaccionID(int idTransaccion) {
    if (numTransacciones < Constantes::MAX_COMPRAS_CLIENTE && idTransaccion > 0) {
        historialTransacciones[numTransacciones] = idTransaccion;
        numTransacciones++;
        return true;
    }
    return false; 
}

/* ========================================================================
    MÉTODOS DE PRESENTACIÓN
   ======================================================================== */

void Cliente::mostrarInformacionBasica() const {
    if (eliminado) return; // No mostrar si está borrado lógicamente
    
    cout << left << " " << setw(5) << id 
         << setw(25) << nombre 
         << setw(15) << cedula 
         << "$" << right << setw(10) << fixed << setprecision(2) << totalGastado 
         << left << "  " << email << endl;
}

void Cliente::mostrarInformacionCompleta() const {
    // Convertir la fecha de registro a string legible
    char bufferFecha[30];
    strftime(bufferFecha, 30, "%d/%m/%Y %H:%M", localtime(&fechaRegistro));

    cout << "\n  " << string(60, '=') << endl;
    cout << "  FICHA DE CLIENTE: " << nombre << (eliminado ? " [INACTIVO]" : "") << endl;
    cout << "  " << string(60, '-') << endl;
    cout << "  ID Sistema:     " << id << endl;
    cout << "  Cedula/DNI:     " << cedula << endl;
    cout << "  Contacto:       " << telefono << " | " << email << endl;
    cout << "  Direccion:      " << direccion << endl;
    cout << "  Fecha Registro: " << bufferFecha << endl;
    cout << "  Acumulado:      $" << fixed << setprecision(2) << totalGastado << endl;
    cout << "  Transacciones:  " << numTransacciones << endl;
    cout << "  " << string(60, '=') << endl;
}

/* ========================================================================
    VALIDACIÓN Y TAMAÑO
   ======================================================================== */

bool Cliente::tieneDatosCompletos() const {
    return (strlen(nombre) > 0 && strlen(cedula) > 0 && !eliminado);
}

size_t Cliente::obtenerTamano() {
    return sizeof(Cliente);
}