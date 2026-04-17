#ifndef GESTOR_ARCHIVOS_HPP
#define GESTOR_ARCHIVOS_HPP

#include <fstream>
#include <iostream>
#include "Constantes.hpp"

/*
 * ArchivoHeader
 * Se mantiene para compatibilidad con el Proyecto 2, pero ahora 
 * el "proximoID" maestro vive en la clase Tienda.
 */
struct ArchivoHeader {
    int cantidadRegistros;   
    int registrosActivos;    
    int version;             
};

class GestorArchivos {
private:
    /*
     * inicializarArchivo
     * Crea un archivo individual con su respectivo Header.
     */
    static bool inicializarArchivo(const char* nombreArchivo);

public:
    static bool inicializarSistemaArchivos();
    static ArchivoHeader leerHeader(const char* nombreArchivo);
    static bool actualizarHeader(const char* nombreArchivo, ArchivoHeader h);
    
    /*
     * calcularOffset
     * Calcula la posicion en el archivo saltando el Header.
     */
    static long calcularOffset(int indice, size_t tamanoEstructura);

    // ==============================================================
    // TEMPLATES GENERICOS (OBLIGATORIOS)
    // ==============================================================

    /*
     * guardarRegistro
     * Ahora NO asigna el ID. El ID viene ya listo desde el metodo de operacion
     * usando la clase Tienda.
     */
    template <typename T>
    static bool guardarRegistro(const char* archivo, T& registro) {
        ArchivoHeader h = leerHeader(archivo);
        
        long posNuevo = calcularOffset(h.cantidadRegistros, sizeof(T));
        std::fstream f(archivo, std::ios::binary | std::ios::in | std::ios::out);
        if (!f.is_open()) return false;

        f.seekp(posNuevo, std::ios::beg);
        f.write(reinterpret_cast<const char*>(&registro), sizeof(T));
        f.close();

        // Actualizamos metadatos basicos del archivo
        h.cantidadRegistros++;
        h.registrosActivos++;
        return actualizarHeader(archivo, h);
    }

    template <typename T>
    static bool leerRegistroPorIndice(const char* archivo, int indice, T& registro) {
        std::ifstream f(archivo, std::ios::binary);
        if (!f.is_open()) return false;

        f.seekg(calcularOffset(indice, sizeof(T)), std::ios::beg);
        f.read(reinterpret_cast<char*>(&registro), sizeof(T));
        f.close();
        return true;
    }

    /*
     * buscarIndicePorID
     * Escanea el archivo buscando un objeto que coincida con el ID y no este eliminado.
     */
    template <typename T>
    static int buscarIndicePorID(const char* archivo, int idBuscado) {
        ArchivoHeader h = leerHeader(archivo);
        T temp;
        for (int i = 0; i < h.cantidadRegistros; i++) {
            if (leerRegistroPorIndice<T>(archivo, i, temp)) {
                if (temp.getId() == idBuscado && !temp.isEliminado()) {
                    return i;
                }
            }
        }
        return -1;
    }

    /*
     * listarTodo
     * Metodo generico que utiliza el metodo de presentacion de la Entidad.
     */
    template <typename T>
    static void listarTodo(const char* archivo) {
        ArchivoHeader h = leerHeader(archivo);
        T temp;
        bool hayRegistros = false;

        for (int i = 0; i < h.cantidadRegistros; i++) {
            if (leerRegistroPorIndice<T>(archivo, i, temp)) {
                if (!temp.isEliminado()) {
                    temp.mostrarInformacionBasica(); // Delegacion
                    hayRegistros = true;
                }
            }
        }
        if (!hayRegistros) std::cout << "\t[ No hay registros activos ]" << std::endl;
    }

    template <typename T>
    static bool actualizarRegistro(const char* archivo, int indice, T& registro) {
        std::fstream f(archivo, std::ios::binary | std::ios::in | std::ios::out);
        if (!f.is_open()) return false;

        f.seekp(calcularOffset(indice, sizeof(T)), std::ios::beg);
        f.write(reinterpret_cast<const char*>(&registro), sizeof(T));
        f.close();
        return true;
    }

    /*
 * contarRegistros
 * Abre el archivo en modo binario, se desplaza al final para calcular 
 * el tamaño total y lo divide por el tamaño de la estructura T.
 */
template <typename T>
static int contarRegistros(const char* archivo) {
    std::ifstream file(archivo, std::ios::binary | std::ios::ate);
    if (!file) return 0;
    return (int)(file.tellg() / sizeof(T));
}
};

#endif