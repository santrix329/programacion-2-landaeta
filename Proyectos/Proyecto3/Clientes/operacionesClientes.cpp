#include "operacionesClientes.hpp"
#include "../entidades/Cliente.hpp"
#include "../persistencia/GestorArchivos.hpp"
#include "../utilidades/Formatos.hpp"
#include "../tienda/Constantes.hpp"
#include <iostream>
#include <iomanip>

using namespace std;

/* ========================================================================
    REGISTRAR CLIENTE
   ======================================================================== */
void crearCliente(Tienda& tienda) {
    Formatos::limpiarPantalla();
    cout << "\n  >>> NUEVO REGISTRO DE CLIENTE <<<" << endl;

    char nom[Constantes::TAM_NOMBRE], ced[Constantes::TAM_CODIGO];
    char tel[Constantes::TAM_TELEFONO], mail[Constantes::TAM_EMAIL], dir[Constantes::TAM_DESCRIPCION];

    // Captura de datos simple
    cout << "  Nombre:    "; cin.getline(nom, Constantes::TAM_NOMBRE);
    cout << "  Cedula:    "; cin.getline(ced, Constantes::TAM_CODIGO);
    cout << "  Telefono:  "; cin.getline(tel, Constantes::TAM_TELEFONO);
    cout << "  Email:     "; cin.getline(mail, Constantes::TAM_EMAIL);
    cout << "  Direccion: "; cin.getline(dir, Constantes::TAM_DESCRIPCION);

    // Creamos el objeto y le asignamos el ID automático de la tienda
    Cliente nuevo(nom, ced, tel, mail, dir);
    nuevo.setId(tienda.generarIdCliente());

    // Guardado usando el Template obligatorio (Punto 4.2)
    if (GestorArchivos::guardarRegistro<Cliente>(Constantes::ARCHIVO_CLIENTES, nuevo)) {
        cout << "\n  [OK] Registrado con exito. ID asignado: " << nuevo.getId() << endl;
    } else {
        cout << "\n  [!] Error: No se pudo escribir en el archivo." << endl;
    }
}

/* ========================================================================
    BUSCAR CLIENTE
   ======================================================================== */
void buscarCliente(const Tienda& tienda) {
    int idBusqueda = Formatos::leerEntero("\n  Ingrese ID a buscar: ", 1, 9999);
    Cliente temporal;
    bool encontrado = false;

    int i = 0;
    while (GestorArchivos::leerRegistroPorIndice<Cliente>(Constantes::ARCHIVO_CLIENTES, i, temporal)) {
        if (temporal.getId() == idBusqueda && !temporal.isEliminado()) {
            temporal.mostrarInformacionCompleta();
            encontrado = true;
            break;
        }
        i++;
    }

    if (!encontrado) cout << "\n  [!] El cliente no existe o esta inactivo." << endl;
}

/* ========================================================================
    ACTUALIZAR CLIENTE
   ======================================================================== */
void actualizarCliente(Tienda& tienda) {
    int idBusqueda = Formatos::leerEntero("\n  ID del cliente a modificar: ", 1, 9999);
    Cliente temporal;
    bool encontrado = false;

    int i = 0;
    while (GestorArchivos::leerRegistroPorIndice<Cliente>(Constantes::ARCHIVO_CLIENTES, i, temporal)) {
        if (temporal.getId() == idBusqueda && !temporal.isEliminado()) {
            
            cout << "  Modificando a: " << temporal.getNombre() << endl;
            char nTel[Constantes::TAM_TELEFONO], nMail[Constantes::TAM_EMAIL];
            
            cout << "  Nuevo Telefono: "; cin.getline(nTel, Constantes::TAM_TELEFONO);
            cout << "  Nuevo Email:    "; cin.getline(nMail, Constantes::TAM_EMAIL);

            temporal.setTelefono(nTel);
            temporal.setEmail(nMail);

            // Actualizamos el registro en la misma posicion 'i' usando Template
            GestorArchivos::actualizarRegistro<Cliente>(Constantes::ARCHIVO_CLIENTES, i, temporal);
            cout << "\n  [OK] Informacion actualizada." << endl;
            encontrado = true;
            break;
        }
        i++;
    }

    if (!encontrado) cout << "\n  [!] No se encontro el cliente solicitado." << endl;
}

/* ========================================================================
    ELIMINAR CLIENTE (Borrado Lógico)
   ======================================================================== */
void eliminarCliente(Tienda& tienda) {
    int idBusqueda = Formatos::leerEntero("\n  ID del cliente a dar de baja: ", 1, 9999);
    Cliente temporal;
    bool encontrado = false;

    int i = 0;
    while (GestorArchivos::leerRegistroPorIndice<Cliente>(Constantes::ARCHIVO_CLIENTES, i, temporal)) {
        if (temporal.getId() == idBusqueda && !temporal.isEliminado()) {
            
            temporal.setEliminado(true); // Marcamos como eliminado (Punto 2.1)
            GestorArchivos::actualizarRegistro<Cliente>(Constantes::ARCHIVO_CLIENTES, i, temporal);
            
            cout << "\n  [OK] El cliente " << temporal.getNombre() << " ha sido desactivado." << endl;
            encontrado = true;
            break;
        }
        i++;
    }

    if (!encontrado) cout << "\n  [!] El ID proporcionado no es valido." << endl;
}

/* ========================================================================
    LISTAR CLIENTES
   ======================================================================== */
void listarClientes(const Tienda& tienda) {
    Formatos::limpiarPantalla();
    cout << "\n  " << string(75, '=') << endl;
    cout << "  " << left << setw(5) << "ID" << setw(25) << "NOMBRE" << setw(15) << "CEDULA" << "EMAIL" << endl;
    cout << "  " << string(75, '-') << endl;

    Cliente temporal;
    int i = 0;
    while (GestorArchivos::leerRegistroPorIndice<Cliente>(Constantes::ARCHIVO_CLIENTES, i, temporal)) {
        if (!temporal.isEliminado()) {
            temporal.mostrarInformacionBasica();
        }
        i++;
    }
    cout << "  " << string(75, '=') << endl;
}