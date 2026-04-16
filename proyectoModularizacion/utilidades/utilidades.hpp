#ifndef UTILIDADES_H
#define UTILIDADES_H

#include <string>

/**
 * @file utilidades.h
 * @brief Funciones de utilidad general: manejo de strings, fechas y formato.
 * No depende de ningún otro módulo del proyecto.
 */

namespace utilidades {

    /** @brief Retorna la fecha actual en formato DD/MM/AAAA */
    std::string obtenerFechaActual();

    /** @brief Retorna la hora actual en formato HH:MM:SS */
    std::string obtenerHoraActual();

    /** @brief Convierte todos los caracteres ASCII a mayúsculas */
    std::string aMayusculas(std::string texto);

    /** @brief Convierte todos los caracteres ASCII a minúsculas */
    std::string aMinusculas(std::string texto);

    /** @brief Elimina espacios al inicio y al final del texto */
    std::string recortarEspacios(std::string texto);

    /**
     * @brief Rellena texto con caracteres a la derecha hasta alcanzar largo.
     * @param relleno Carácter de relleno (por defecto espacio)
     */
    std::string rellenarDerecha(std::string texto, int largo, char relleno = ' ');

    /**
     * @brief Rellena texto con caracteres a la izquierda hasta alcanzar largo.
     * @param relleno Carácter de relleno (por defecto espacio)
     */
    std::string rellenarIzquierda(std::string texto, int largo, char relleno = ' ');

    /**
     * @brief Redondea un valor double a una cantidad dada de decimales.
     * @param decimales Número de cifras decimales a conservar
     */
    double redondear(double valor, int decimales);

    /**
     * @brief Genera una línea de caracteres repetidos.
     * @param caracter Carácter a repetir (por defecto '-')
     */
    std::string lineaSeparadora(int largo, char caracter = '-');

    /** @brief Compara dos strings ignorando diferencias de mayúsculas/minúsculas */
    bool compararIgnorandoCase(std::string a, std::string b);

    /** @brief Verifica si 'texto' contiene 'busqueda' (sin importar mayúsculas) */
    bool contieneTexto(std::string texto, std::string busqueda);

    /** @brief Formatea un número double con exactamente 2 decimales */
    std::string formatoNota(double nota);

    /**
     * @brief Genera una barra de progreso visual con caracteres '#' y '.'.
     * @param porcentaje Valor entre 0 y 100
     * @param largo Número de caracteres de la barra (por defecto 20)
     */
    std::string barraProgreso(double porcentaje, int largo = 20);

} // namespace utilidades

#endif // UTILIDADES_H