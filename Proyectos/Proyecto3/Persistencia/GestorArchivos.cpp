#include "GestorArchivos.hpp"
#include "../Tienda/Tienda.hpp"
#include "../utilidades/Formatos.hpp"
#include <iostream>
#include <fstream>
#include <iomanip>
#include <cstring>

using namespace std;

/* ========================================================================
    CONFIGURACION DE ESTILOS Y LOGS
   ======================================================================== */
#define RST    "\033[0m"
#define NEG    "\033[1m"
#define VERDE  "\033[32m"
#define ROJO   "\033[31m"
#define AMARI  "\033[33m"
#define CIAN   "\033[36m"

/* ========================================================================
    1. INICIALIZACION DEL SISTEMA (Fase 5 de la Estrategia)
   ======================================================================== */

/*
 * inicializarSistemaArchivos
 * Funcion maestra que prepara el entorno de persistencia. Se invoca una
 * sola vez en el main.cpp antes de iniciar la interfaz.
 */
bool GestorArchivos::inicializarSistemaArchivos() {
    cout << CIAN << NEG << " [SISTEMA] Iniciando modulo de persistencia binaria..." << RST << endl;
    
    bool exitoTotal = true;

    // Listado de rutas de archivos definidos en Constantes.hpp
    const char* repositorios[] = {
        Constantes::ARCHIVO_PRODUCTOS,
        Constantes::ARCHIVO_PROVEEDORES,
        Constantes::ARCHIVO_CLIENTES,
        Constantes::ARCHIVO_TRANSACCIONES
    };

    // --- PASO 1: Inicializar archivos de entidades comunes ---
    for (const char* ruta : repositorios) {
        if (!inicializarArchivo(ruta)) {
            cout << ROJO << "  [!] Error fatal al preparar repositorio: " << ruta << RST << endl;
            exitoTotal = false;
        } else {
            cout << VERDE << "  [OK] Repositorio verificado: " << ruta << RST << endl;
        }
    }

    // --- PASO 2: Inicializar el archivo Maestro de la Tienda ---
    // Este archivo es unico porque guarda el estado global del sistema.
    ifstream check(Constantes::ARCHIVO_TIENDA, ios::binary);
    if (!check.is_open()) {
        cout << AMARI << "  [>] Archivo de Tienda no detectado. Creando parametros..." << RST << endl;
        check.close();

        Tienda nuevaTienda; // Constructor por defecto pone IDs en 1
        
        // Guardamos la tienda en el registro 0 de su propio archivo
        ofstream out(Constantes::ARCHIVO_TIENDA, ios::binary | ios::out);
        if (out.is_open()) {
            out.write(reinterpret_cast<const char*>(&nuevaTienda), sizeof(Tienda));
            out.close();
            cout << VERDE << "  [OK] Parametros de Tienda inicializados correctamente." << RST << endl;
        } else {
            cout << ROJO << "  [!] No se pudo crear tienda.bin. Revise permisos." << RST << endl;
            exitoTotal = false;
        }
    } else {
        check.close();
    }

    return exitoTotal;
}

/*
 * inicializarArchivo
 * Crea el archivo fisico y escribe el Header inicial si no existe.
 */
bool GestorArchivos::inicializarArchivo(const char* nombreArchivo) {
    // Verificamos existencia previa
    ifstream test(nombreArchivo, ios::binary);
    if (test.good()) {
        test.close();
        return true; 
    }
    test.close();

    // Creacion de archivo nuevo con Header de control
    ofstream archivo(nombreArchivo, ios::binary | ios::out);
    if (!archivo) return false;

    /* * Estructura inicial del Header:
     * - cantidadRegistros: 0
     * - registrosActivos: 0
     * - version: 1 (Para control de migracion de datos futura)
     */
    ArchivoHeader headerInicial = {0, 0, 1};
    
    archivo.write(reinterpret_cast<const char*>(&headerInicial), sizeof(ArchivoHeader));
    
    bool exito = !archivo.fail();
    archivo.close();
    return exito;
}

/* ========================================================================
    2. GESTION DE METADATOS (Header)
   ======================================================================== */

/*
 * leerHeader
 * Recupera la estructura de control situada al inicio del archivo binario.
 */
ArchivoHeader GestorArchivos::leerHeader(const char* nombreArchivo) {
    ArchivoHeader h = {0, 0, 1}; // Valores de seguridad
    
    ifstream f(nombreArchivo, ios::binary);
    if (!f) return h;

    f.seekg(0, ios::beg);
    f.read(reinterpret_cast<char*>(&h), sizeof(ArchivoHeader));
    f.close();
    
    return h;
}

/*
 * actualizarHeader
 * Sobrescribe los metadatos. Se usa in|out para no borrar el contenido.
 */
bool GestorArchivos::actualizarHeader(const char* nombreArchivo, ArchivoHeader h) {
    fstream f(nombreArchivo, ios::binary | ios::in | ios::out);
    if (!f.is_open()) return false;

    f.seekp(0, ios::beg);
    f.write(reinterpret_cast<const char*>(&h), sizeof(ArchivoHeader));
    
    bool exito = !f.fail();
    f.close();
    return exito;
}

/* ========================================================================
    3. ACCESO ALEATORIO (Calculo de Offsets)
   ======================================================================== */

/*
 * calcularOffset
 * Implementa la formula de acceso directo para archivos de registro fijo:
 * Posicion = sizeof(Header) + (Indice * sizeof(Clase))
 */
long GestorArchivos::calcularOffset(int indice, size_t tamanoEstructura) {
    // Si el indice es negativo, asumimos que queremos ir al inicio de los datos
    if (indice < 0) return (long)sizeof(ArchivoHeader);

    long posHeader = (long)sizeof(ArchivoHeader);
    long posRegistros = (long)indice * (long)tamanoEstructura;
    
    return posHeader + posRegistros;
}