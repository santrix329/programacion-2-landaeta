#ifndef FORMATOS_HPP
#define FORMATOS_HPP

#include <string>
#include <ctime>

class Formatos {
public:
    static void limpiarPantalla();
    
    // Cambiamos 'pausa' por 'pausar' para que coincida con tu código
    static void pausar(); 

    static int leerEntero(const char* mensaje, int min, int max);

    /* --- ESTAS SON LAS QUE TE FALTABAN --- */

    // Para leer precios o montos
    static float leerFloat(const char* mensaje, float min, float max);

    // Para limpiar el basurero del teclado (el \n que deja el cin)
    static void limpiarBuffer();

    /* ------------------------------------- */

    static std::string formatoFecha(time_t fecha);
    static std::string centrarTexto(std::string texto);
};

#endif