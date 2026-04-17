#ifndef VALIDACIONES_HPP
#define VALIDACIONES_HPP

/* ========================================================================
    UTILIDAD: VALIDACIONES
    Responsabilidad: Proveer funciones estaticas para verificar la integridad
    de los datos antes de que sean procesados por las entidades.
   ======================================================================== */
class Validaciones {
public:
    /*
     * esCadenaValida
     * Verifica que el texto no sea nulo, no este vacio y no supere 
     * el limite de caracteres permitido.
     */
    static bool esCadenaValida(const char* texto, int minLargo, int maxLargo);

    /*
     * esEnteroPositivo
     * Comprueba que un valor numerico sea mayor o igual a cero.
     */
    static bool esEnteroPositivo(int valor);

    /*
     * esPrecioValido
     * Valida que los montos monetarios sean mayores a cero.
     */
    static bool esPrecioValido(float precio);

    /*
     * esEmailValido
     * Realiza una comprobacion basica de formato (presencia de '@' y '.').
     */
    static bool esEmailValido(const char* email);

    /*
     * esRifValido
     * Verifica que el formato del RIF (Venezuela) o ID Fiscal sea coherente.
     */
    static bool esRifValido(const char* rif);
};

#endif