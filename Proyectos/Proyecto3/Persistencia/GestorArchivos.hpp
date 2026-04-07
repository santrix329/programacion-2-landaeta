#ifndef GESTOR_ARCHIVOS_HPP
#define GESTOR_ARCHIVOS_HPP

#include <fstream>
#include "Constantes.hpp"

class GestorArchivos {
public:
    // Este método es "mágico": T puede ser Producto, Cliente o Proveedor
    template <typename T>
    static bool guardarRegistro(const char* ruta, const T& objeto) {
        std::ofstream archivo(ruta, std::ios::binary | std::ios::app);
        if (!archivo) return false;
        
        archivo.write(reinterpret_cast<const char*>(&objeto), sizeof(T));
        archivo.close();
        return true;
    }

    // Método para leer por índice (también genérico)
    template <typename T>
    static bool leerRegistro(const char* ruta, int indice, T& resultado) {
        std::ifstream archivo(ruta, std::ios::binary);
        if (!archivo) return false;

        // Saltamos el Header y llegamos al registro N
        long offset = sizeof(ArchivoHeader) + (indice * sizeof(T));
        archivo.seekg(offset, std::ios::beg);
        archivo.read(reinterpret_cast<char*>(&resultado), sizeof(T));
        
        archivo.close();
        return true;
    }
};

#endif