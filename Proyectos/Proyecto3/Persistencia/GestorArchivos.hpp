#ifndef GESTOR_ARCHIVOS_HPP
#define GESTOR_ARCHIVOS_HPP

#include <fstream>
#include <iostream>
#include "Constantes.hpp"

using namespace std;

// El header se mantiene exactamente igual al Proyecto 2
struct ArchivoHeader {
    int cantidadRegistros;   
    int proximoID;           
    int registrosActivos;    
    int version;             
};

class GestorArchivos {
private:
    // Método auxiliar privado
    static bool inicializarArchivo(const char* nombreArchivo);

public:
    // Métodos normales (implementados en el .cpp)
    static bool inicializarSistemaArchivos();
    static ArchivoHeader leerHeader(const char* nombreArchivo);
    static bool actualizarHeader(const char* nombreArchivo, ArchivoHeader h);
    static long calcularOffset(int indice, size_t tamanoEstructura);

    // ==============================================================
    // IMPLEMENTACIÓN DE TEMPLATES (Deben ir en el .hpp)
    // ==============================================================

    // Guarda un nuevo registro al final del archivo
    template <typename T>
    static bool guardarRegistro(const char* archivo, T& registro) {
        ArchivoHeader h = leerHeader(archivo);
        
        long posNuevo = calcularOffset(h.cantidadRegistros, sizeof(T));

        fstream f(archivo, ios::binary | ios::in | ios::out);
        if (!f.is_open()) {
            return false;
        }

        f.seekp(posNuevo, ios::beg);
        f.write(reinterpret_cast<const char*>(&registro), sizeof(T));
        f.close();

        // Actualizamos los contadores del header
        h.cantidadRegistros++;
        h.proximoID++;
        h.registrosActivos++;

        return actualizarHeader(archivo, h);
    }

    // Lee un registro específico usando su índice en el archivo
    template <typename T>
    static bool leerRegistroPorIndice(const char* archivo, int indice, T& registro) {
        ifstream f(archivo, ios::binary);
        if (!f.is_open()) {
            return false;
        }

        f.seekg(calcularOffset(indice, sizeof(T)), ios::beg);
        f.read(reinterpret_cast<char*>(&registro), sizeof(T));
        f.close();
        
        return true;
    }

    // Sobreescribe un registro existente en su posición exacta
    template <typename T>
    static bool actualizarRegistro(const char* archivo, int indice, T& registro) {
        fstream f(archivo, ios::binary | ios::in | ios::out);
        if (!f.is_open()) {
            return false;
        }

        f.seekp(calcularOffset(indice, sizeof(T)), ios::beg);
        f.write(reinterpret_cast<const char*>(&registro), sizeof(T));
        f.close();

        return true;
    }
};

#endif // GESTOR_ARCHIVOS_HPP