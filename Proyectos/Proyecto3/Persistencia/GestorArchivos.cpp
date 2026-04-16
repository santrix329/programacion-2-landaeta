#include "GestorArchivos.hpp"

// Inicializa todos los archivos del sistema de una sola vez
bool GestorArchivos::inicializarSistemaArchivos() {
    bool ok = true;
    ok &= inicializarArchivo(Constantes::ARCHIVO_PRODUCTOS);
    ok &= inicializarArchivo(Constantes::ARCHIVO_PROVEEDORES);
    ok &= inicializarArchivo(Constantes::ARCHIVO_CLIENTES);
    ok &= inicializarArchivo(Constantes::ARCHIVO_TRANSACCIONES);
    ok &= inicializarArchivo(Constantes::ARCHIVO_TIENDA);
    return ok;
}

// Verifica si un archivo existe, si no, lo crea con un header en ceros
bool GestorArchivos::inicializarArchivo(const char* nombreArchivo) {
    ifstream prueba(nombreArchivo, ios::binary);

    if (prueba.good()) {
        prueba.close();
        return true; // Ya existe
    }
    prueba.close();

    ofstream nuevo(nombreArchivo, ios::binary);
    if (!nuevo.is_open()) {
        return false;
    }

    ArchivoHeader h = {0, 1, 0, 1}; // {cantidad, proximoID, activos, version}
    nuevo.write(reinterpret_cast<char*>(&h), sizeof(ArchivoHeader));
    nuevo.close();

    return true;
}

// Lee el header de un archivo dado
ArchivoHeader GestorArchivos::leerHeader(const char* nombreArchivo) {
    ArchivoHeader h = {0, 1, 0, 1}; // Valores por defecto seguros
    ifstream archivo(nombreArchivo, ios::binary);

    if (archivo.is_open()) {
        archivo.read(reinterpret_cast<char*>(&h), sizeof(ArchivoHeader));
        archivo.close();
    }
    return h;
}

// Reescribe el header con nuevos datos
bool GestorArchivos::actualizarHeader(const char* nombreArchivo, ArchivoHeader h) {
    fstream archivo(nombreArchivo, ios::binary | ios::in | ios::out);

    if (!archivo.is_open()) {
        return false;
    }

    archivo.seekp(0, ios::beg);
    archivo.write(reinterpret_cast<char*>(&h), sizeof(ArchivoHeader));
    archivo.close();

    return true;
}

// Calcula el byte exacto donde empieza un registro
long GestorArchivos::calcularOffset(int indice, size_t tamanoEstructura) {
    long bytesDelHeader = static_cast<long>(sizeof(ArchivoHeader));
    long bytesHastaElSlot = static_cast<long>(indice) * static_cast<long>(tamanoEstructura);
    return bytesDelHeader + bytesHastaElSlot;
}