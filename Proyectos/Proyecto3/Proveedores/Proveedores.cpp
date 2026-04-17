#include "Proveedores.hpp"
#include "../Persistencia/Constantes.hpp"
#include <iostream>
#include <cstring>
#include <iomanip>

using namespace std;

/*
 * Proveedor
 * Constructor por defecto. Inicializa los atributos numericos en cero,
 * los booleanos en falso y las cadenas de caracteres como vacias.
 */
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

/*
 * Proveedor
 * Constructor parametrizado. Delega la asignacion a los setters para
 * asegurar que cualquier regla de validacion se aplique desde el inicio.
 */
Proveedor::Proveedor(const char* empresa, const char* numRif, const char* tel, const char* mail, const char* dir, const char* contacto) : Proveedor() {
    setNombreEmpresa(empresa);
    setRif(numRif);
    setTelefono(tel);
    setEmail(mail);
    setDireccion(dir);
    setPersonaContacto(contacto);
}

/*
 * Proveedor
 * Constructor de copia. Realiza una copia bit a bit de los atributos
 * para duplicar el estado de un objeto proveedor existente.
 */
Proveedor::Proveedor(const Proveedor& otro) {
    id = otro.id;
    strncpy(nombreEmpresa, otro.nombreEmpresa, Constantes::TAM_NOMBRE);
    strncpy(rif, otro.rif, Constantes::TAM_CODIGO);
    strncpy(telefono, otro.telefono, Constantes::TAM_TELEFONO);
    strncpy(email, otro.email, Constantes::TAM_EMAIL);
    strncpy(direccion, otro.direccion, Constantes::TAM_DESCRIPCION);
    strncpy(personaContacto, otro.personaContacto, Constantes::TAM_NOMBRE);
    eliminado = otro.eliminado;
}

/*
 * ~Proveedor
 * Destructor de la clase. Al no manejar memoria dinamica (new/delete),
 * no requiere logica de liberacion explicita.
 */
Proveedor::~Proveedor() {}

/*
 * getId
 * Retorna el identificador unico del proveedor.
 */
int Proveedor::getId() const { return id; }

/*
 * getNombreEmpresa
 * Retorna un puntero constante al nombre de la empresa.
 */
const char* Proveedor::getNombreEmpresa() const { return nombreEmpresa; }

/*
 * getRif
 * Retorna el Registro de Informacion Fiscal.
 */
const char* Proveedor::getRif() const { return rif; }

/*
 * setId
 * Asigna un nuevo ID siempre que sea un valor positivo.
 */
void Proveedor::setId(int nuevoId) {
    if (nuevoId >= 0) id = nuevoId;
}

/*
 * setNombreEmpresa
 * Copia el nombre validando que no sea nulo. Retorna true si tuvo exito.
 */
bool Proveedor::setNombreEmpresa(const char* nuevoNombre) {
    if (nuevoNombre == nullptr || strlen(nuevoNombre) == 0) return false;
    strncpy(nombreEmpresa, nuevoNombre, Constantes::TAM_NOMBRE - 1);
    nombreEmpresa[Constantes::TAM_NOMBRE - 1] = '\0';
    return true;
}

/*
 * setRif
 * Asigna el RIF/ID fiscal usando la constante de tamano de codigo.
 */
bool Proveedor::setRif(const char* nuevoRif) {
    if (nuevoRif == nullptr || strlen(nuevoRif) == 0) return false;
    strncpy(rif, nuevoRif, Constantes::TAM_CODIGO - 1);
    rif[Constantes::TAM_CODIGO - 1] = '\0';
    return true;
}

/*
 * setTelefono
 * Asigna el numero de contacto.
 */
bool Proveedor::setTelefono(const char* nuevoTelefono) {
    if (nuevoTelefono == nullptr) return false;
    strncpy(telefono, nuevoTelefono, Constantes::TAM_TELEFONO - 1);
    telefono[Constantes::TAM_TELEFONO - 1] = '\0';
    return true;
}

/*
 * setEmail
 * Asigna el correo electronico del proveedor.
 */
bool Proveedor::setEmail(const char* nuevoEmail) {
    if (nuevoEmail == nullptr) return false;
    strncpy(email, nuevoEmail, Constantes::TAM_EMAIL - 1);
    email[Constantes::TAM_EMAIL - 1] = '\0';
    return true;
}

/*
 * setDireccion
 * Asigna la ubicacion fisica de la empresa.
 */
bool Proveedor::setDireccion(const char* nuevaDireccion) {
    if (nuevaDireccion == nullptr) return false;
    strncpy(direccion, nuevaDireccion, Constantes::TAM_DESCRIPCION - 1);
    direccion[Constantes::TAM_DESCRIPCION - 1] = '\0';
    return true;
}

/*
 * setPersonaContacto
 * Asigna el nombre del representante de la empresa.
 */
bool Proveedor::setPersonaContacto(const char* nuevoContacto) {
    if (nuevoContacto == nullptr) return false;
    strncpy(personaContacto, nuevoContacto, Constantes::TAM_NOMBRE - 1);
    personaContacto[Constantes::TAM_NOMBRE - 1] = '\0';
    return true;
}

/*
 * setEliminado
 * Cambia el estado de borrado logico del registro.
 */
void Proveedor::setEliminado(bool estado) {
    eliminado = estado;
}

/*
 * isEliminado
 * Informa si el registro esta marcado como inactivo.
 */
bool Proveedor::isEliminado() const { return eliminado; }

/*
 * mostrarInformacionBasica
 * Imprime los datos principales en formato de tabla para listados.
 */
void Proveedor::mostrarInformacionBasica() const {
    cout << left << setw(6) << id 
         << setw(25) << nombreEmpresa 
         << setw(15) << rif 
         << setw(15) << telefono 
         << personaContacto << endl;
}

/*
 * mostrarInformacionCompleta
 * Muestra detalladamente todos los atributos del proveedor.
 */
void Proveedor::mostrarInformacionCompleta() const {
    cout << "\n --- DETALLE DEL PROVEEDOR ---" << endl;
    cout << " ID:               " << id << endl;
    cout << " Empresa:          " << nombreEmpresa << endl;
    cout << " RIF/ID Fiscal:    " << rif << endl;
    cout << " Persona Contacto: " << personaContacto << endl;
    cout << " Telefono:         " << telefono << endl;
    cout << " Email:            " << email << endl;
    cout << " Direccion:        " << direccion << endl;
}

/*
 * esValido
 * Verifica que los campos obligatorios esten completos.
 */
bool Proveedor::esValido() const {
    return (strlen(nombreEmpresa) > 0 && strlen(rif) > 0);
}

/*
 * obtenerTamano
 * Retorna el tamano en bytes de la clase para calculos de punteros de archivo.
 */
size_t Proveedor::obtenerTamano() {
    return sizeof(Proveedor);
}