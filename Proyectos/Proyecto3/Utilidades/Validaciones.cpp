#include "Validaciones.hpp"
#include <cstring>
#include <cctype>

/*
 * esCadenaValida
 * Verifica que el puntero no sea nulo, que tenga una longitud minima
 * y que no exceda el maximo permitido para el arreglo de destino.
 */
bool Validaciones::esCadenaValida(const char* texto, int minLargo, int maxLargo) {
    if (texto == nullptr) return false;
    int largo = strlen(texto);
    return (largo >= minLargo && largo <= maxLargo);
}

/*
 * esEnteroPositivo
 * Valida que cantidades como el Stock o IDs no sean numeros negativos,
 * lo cual romperia la logica del inventario.
 */
bool Validaciones::esEnteroPositivo(int valor) {
    return valor >= 0;
}

/*
 * esPrecioValido
 * Asegura que los montos sean mayores a cero. No se permiten productos 
 * gratis o con precios negativos en este sistema POS.
 */
bool Validaciones::esPrecioValido(float precio) {
    return precio > 0.0f;
}

/*
 * esEmailValido
 * Realiza una comprobacion estructural simple: debe contener al menos 
 * un arroba '@' y un punto '.' despues de este.
 */
bool Validaciones::esEmailValido(const char* email) {
    if (email == nullptr || strlen(email) < 5) return false;
    
    const char* arroba = strchr(email, '@');
    if (!arroba) return false;
    
    const char* punto = strchr(arroba, '.');
    return (punto != nullptr);
}

/*
 * esRifValido
 * Verifica que el RIF comience con una letra valida (V, J, G, E, P)
 * y que el resto sean digitos numericos.
 */
bool Validaciones::esRifValido(const char* rif) {
    if (rif == nullptr || strlen(rif) < 7) return false;

    // Primera letra en mayuscula
    char inicial = toupper(rif[0]);
    if (inicial != 'V' && inicial != 'J' && inicial != 'G' && inicial != 'E' && inicial != 'P') {
        return false;
    }

    // El resto deben ser digitos (saltamos la inicial)
    for (int i = 1; rif[i] != '\0'; i++) {
        if (!isdigit(rif[i]) && rif[i] != '-') {
            // Permitimos guiones, pero no letras en el cuerpo del RIF
            return false;
        }
    }

    return true;
}