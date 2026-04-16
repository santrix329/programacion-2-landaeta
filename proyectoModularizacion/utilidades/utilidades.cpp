#include "utilidades.hpp"
#include <cmath>
#include <ctime>
#include <string>

using std::string;

namespace utilidades {

string obtenerFechaActual() {
    time_t ahora = time(0);
    tm *ltm = localtime(&ahora);
    string fecha = "";
    int dia = ltm->tm_mday;
    int mes = 1 + ltm->tm_mon;
    int anio = 1900 + ltm->tm_year;
    if (dia < 10) fecha += "0";
    fecha += std::to_string(dia) + "/";
    if (mes < 10) fecha += "0";
    fecha += std::to_string(mes) + "/";
    fecha += std::to_string(anio);
    return fecha;
}

string obtenerHoraActual() {
    time_t ahora = time(0);
    tm *ltm = localtime(&ahora);
    string hora = "";
    if (ltm->tm_hour < 10) hora += "0";
    hora += std::to_string(ltm->tm_hour) + ":";
    if (ltm->tm_min < 10) hora += "0";
    hora += std::to_string(ltm->tm_min) + ":";
    if (ltm->tm_sec < 10) hora += "0";
    hora += std::to_string(ltm->tm_sec);
    return hora;
}

string aMayusculas(string texto) {
    string resultado = "";
    for (int i = 0; i < (int)texto.length(); i++) {
        char c = texto[i];
        if (c >= 'a' && c <= 'z') resultado += (char)(c - 32);
        else resultado += c;
    }
    return resultado;
}

string aMinusculas(string texto) {
    string resultado = "";
    for (int i = 0; i < (int)texto.length(); i++) {
        char c = texto[i];
        if (c >= 'A' && c <= 'Z') resultado += (char)(c + 32);
        else resultado += c;
    }
    return resultado;
}

string recortarEspacios(string texto) {
    int inicio = 0, fin = texto.length() - 1;
    while (inicio <= fin && texto[inicio] == ' ') inicio++;
    while (fin >= inicio && texto[fin] == ' ') fin--;
    string resultado = "";
    for (int i = inicio; i <= fin; i++) resultado += texto[i];
    return resultado;
}

string rellenarDerecha(string texto, int largo, char relleno) {
    string resultado = texto;
    while ((int)resultado.length() < largo) resultado += relleno;
    return resultado;
}

string rellenarIzquierda(string texto, int largo, char relleno) {
    string resultado = texto;
    while ((int)resultado.length() < largo) resultado = relleno + resultado;
    return resultado;
}

double redondear(double valor, int decimales) {
    double factor = pow(10, decimales);
    return round(valor * factor) / factor;
}

string lineaSeparadora(int largo, char caracter) {
    string linea = "";
    for (int i = 0; i < largo; i++) linea += caracter;
    return linea;
}

bool compararIgnorandoCase(string a, string b) {
    if (a.length() != b.length()) return false;
    for (int i = 0; i < (int)a.length(); i++) {
        char ca = a[i], cb = b[i];
        if (ca >= 'A' && ca <= 'Z') ca += 32;
        if (cb >= 'A' && cb <= 'Z') cb += 32;
        if (ca != cb) return false;
    }
    return true;
}

bool contieneTexto(string texto, string busqueda) {
    string textoMin = aMinusculas(texto);
    string busqMin = aMinusculas(busqueda);
    for (int i = 0; i <= (int)textoMin.length() - (int)busqMin.length(); i++) {
        bool encontrado = true;
        for (int j = 0; j < (int)busqMin.length(); j++) {
            if (textoMin[i + j] != busqMin[j]) { encontrado = false; break; }
        }
        if (encontrado) return true;
    }
    return false;
}

string formatoNota(double nota) {
    string resultado = std::to_string(redondear(nota, 2));
    int puntoPos = -1;
    for (int i = 0; i < (int)resultado.length(); i++) {
        if (resultado[i] == '.') { puntoPos = i; break; }
    }
    if (puntoPos >= 0 && (int)resultado.length() > puntoPos + 3) {
        resultado = resultado.substr(0, puntoPos + 3);
    }
    return resultado;
}

string barraProgreso(double porcentaje, int largo) {
    int llenos = (int)(porcentaje / 100.0 * largo);
    if (llenos > largo) llenos = largo;
    if (llenos < 0) llenos = 0;
    string barra = "[";
    for (int i = 0; i < largo; i++) {
        if (i < llenos) barra += "#";
        else barra += ".";
    }
    barra += "] " + formatoNota(porcentaje) + "%";
    return barra;
}

} // namespace utilidades