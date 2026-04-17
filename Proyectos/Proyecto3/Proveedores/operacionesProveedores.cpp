#include "operacionesProveedores.hpp"
#include "Proveedores.hpp"
#include "../Persistencia/GestorArchivos.hpp"
#include "../Utilidades/Formatos.hpp"
#include "../Persistencia/Constantes.hpp"
#include <iostream>
#include <iomanip>

using namespace std;

/*
 * registrarProveedor
 * Captura datos, crea el objeto Proveedor y lo persiste usando Templates.
 */
void registrarProveedor(Tienda& tienda) {
    Formatos::limpiarPantalla();
    cout << "\n  >>> REGISTRO DE PROVEEDOR <<<" << endl;

    char emp[Constantes::TAM_NOMBRE], rif[Constantes::TAM_CODIGO];
    char tel[Constantes::TAM_TELEFONO], mail[Constantes::TAM_EMAIL];
    char dir[Constantes::TAM_DESCRIPCION], con[Constantes::TAM_NOMBRE];

    cout << "  Nombre Empresa:  "; cin.getline(emp, Constantes::TAM_NOMBRE);
    cout << "  RIF / ID Fiscal: "; cin.getline(rif, Constantes::TAM_CODIGO);
    cout << "  Persona Contacto:"; cin.getline(con, Constantes::TAM_NOMBRE);
    cout << "  Telefono:        "; cin.getline(tel, Constantes::TAM_TELEFONO);
    cout << "  Email:           "; cin.getline(mail, Constantes::TAM_EMAIL);
    cout << "  Direccion:       "; cin.getline(dir, Constantes::TAM_DESCRIPCION);

    // Creamos el objeto y asignamos ID automatico
    Proveedor nuevo(emp, rif, tel, mail, dir, con);
    nuevo.setId(tienda.generarIdProveedor());

    if (GestorArchivos::guardarRegistro<Proveedor>(Constantes::ARCHIVO_PROVEEDORES, nuevo)) {
        cout << "\n  [OK] Proveedor registrado con ID: " << nuevo.getId() << endl;
    } else {
        cout << "\n  [!] Error al guardar en el archivo binario." << endl;
    }
}

/*
 * buscarProveedor
 * Localiza un proveedor por ID recorriendo el archivo binario.
 */
void buscarProveedor(const Tienda& tienda) {
    int idBusqueda = Formatos::leerEntero("\n  ID del proveedor a buscar: ", 1, 9999);
    Proveedor temp;
    bool encontrado = false;

    int i = 0;
    while (GestorArchivos::leerRegistroPorIndice<Proveedor>(Constantes::ARCHIVO_PROVEEDORES, i, temp)) {
        if (temp.getId() == idBusqueda && !temp.isEliminado()) {
            temp.mostrarInformacionCompleta();
            encontrado = true;
            break;
        }
        i++;
    }

    if (!encontrado) cout << "\n  [!] Proveedor no encontrado o inactivo." << endl;
}

/*
 * actualizarProveedor
 * Busca el registro y permite sobreescribir datos de contacto.
 */
void actualizarProveedor(Tienda& tienda) {
    int idBusqueda = Formatos::leerEntero("\n  ID del proveedor a editar: ", 1, 9999);
    Proveedor temp;
    int i = 0;

    while (GestorArchivos::leerRegistroPorIndice<Proveedor>(Constantes::ARCHIVO_PROVEEDORES, i, temp)) {
        if (temp.getId() == idBusqueda && !temp.isEliminado()) {
            char nTel[Constantes::TAM_TELEFONO], nCon[Constantes::TAM_NOMBRE];
            
            cout << "  Editando: " << temp.getNombreEmpresa() << endl;
            cout << "  Nuevo Telefono: "; cin.getline(nTel, Constantes::TAM_TELEFONO);
            cout << "  Nuevo Contacto: "; cin.getline(nCon, Constantes::TAM_NOMBRE);

            temp.setTelefono(nTel);
            temp.setPersonaContacto(nCon);

            GestorArchivos::actualizarRegistro<Proveedor>(Constantes::ARCHIVO_PROVEEDORES, i, temp);
            cout << "\n  [OK] Datos actualizados correctamente." << endl;
            return;
        }
        i++;
    }
    cout << "\n  [!] No se encontro el proveedor." << endl;
}

/*
 * eliminarProveedor
 * Realiza el borrado logico seteando el flag 'eliminado' en true.
 */
void eliminarProveedor(Tienda& tienda) {
    int idBusqueda = Formatos::leerEntero("\n  ID del proveedor a eliminar: ", 1, 9999);
    Proveedor temp;
    int i = 0;

    while (GestorArchivos::leerRegistroPorIndice<Proveedor>(Constantes::ARCHIVO_PROVEEDORES, i, temp)) {
        if (temp.getId() == idBusqueda && !temp.isEliminado()) {
            temp.setEliminado(true);
            GestorArchivos::actualizarRegistro<Proveedor>(Constantes::ARCHIVO_PROVEEDORES, i, temp);
            cout << "\n  [OK] Proveedor dado de baja exitosamente." << endl;
            return;
        }
        i++;
    }
    cout << "\n  [!] ID no encontrado." << endl;
}

/*
 * listarProveedores
 * Muestra todos los registros que no tengan el flag de borrado activo.
 */
void listarProveedores(const Tienda& tienda) {
    Formatos::limpiarPantalla();
    cout << "\n  " << string(80, '=') << endl;
    cout << "  " << left << setw(6) << "ID" << setw(25) << "EMPRESA" 
         << setw(15) << "RIF" << setw(15) << "TEL" << "CONTACTO" << endl;
    cout << "  " << string(80, '-') << endl;

    Proveedor temp;
    int i = 0;
    while (GestorArchivos::leerRegistroPorIndice<Proveedor>(Constantes::ARCHIVO_PROVEEDORES, i, temp)) {
        if (!temp.isEliminado()) {
            temp.mostrarInformacionBasica();
        }
        i++;
    }
    cout << "  " << string(80, '=') << endl;
}