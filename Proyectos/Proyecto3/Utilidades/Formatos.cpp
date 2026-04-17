#include "Formatos.hpp"
#include <iostream>
#include <limits>
#include <cstdlib>

/*
 * pausar
 * Detiene la ejecución hasta que el usuario presione Enter.
 */
void Formatos::pausar() {
    std::cout << "\nPresione Enter para continuar...";
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    std::cin.get();
}

/*
 * limpiarBuffer
 * Limpia el estado de error y los caracteres pendientes en el flujo de entrada.
 */
void Formatos::limpiarBuffer() {
    std::cin.clear();
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
}

/*
 * limpiarPantalla
 * Ejecuta el comando del sistema para limpiar la terminal según el SO.
 */
void Formatos::limpiarPantalla() {
    #ifdef _WIN32
        system("cls");
    #else
        system("clear");
    #endif
}

/*
 * leerEntero
 * Solicita un entero y lo valida dentro de un rango específico.
 */
int Formatos::leerEntero(const char* mensaje, int min, int max) {
    int valor;
    while (true) {
        std::cout << mensaje;
        if (std::cin >> valor && valor >= min && valor <= max) {
            return valor;
        }
        std::cout << "Error: Ingrese un entero entre " << min << " y " << max << ".\n";
        limpiarBuffer();
    }
}

/*
 * leerFloat
 * Solicita un flotante y lo valida dentro de un rango específico.
 */
float Formatos::leerFloat(const char* mensaje, float min, float max) {
    float valor;
    while (true) {
        std::cout << mensaje;
        if (std::cin >> valor && valor >= min && valor <= max) {
            return valor;
        }
        std::cout << "Error: Ingrese un valor entre " << min << " y " << max << ".\n";
        limpiarBuffer();
    }
}