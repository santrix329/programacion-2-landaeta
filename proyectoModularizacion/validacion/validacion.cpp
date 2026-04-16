#include "validacion.hpp"
#include "datos.hpp"
#include "utilidades.hpp"
#include <iostream>

using std::string;
using std::cout;
using std::cin;
using std::endl;

namespace validacion {

bool validarMatricula(string mat) {
    if (mat.length() != 9) return false;
    if (mat[4] != '-') return false;
    for (int i = 0; i < 4; i++) {
        if (!((mat[i] >= 'A' && mat[i] <= 'Z') || (mat[i] >= 'a' && mat[i] <= 'z')))
            return false;
    }
    for (int i = 5; i < 9; i++) {
        if (mat[i] < '0' || mat[i] > '9') return false;
    }
    return true;
}

bool validarMatriculaUnica(const datos::BDEstudiantes& bd, string mat, int excluirIdx) {
    string matUpper = utilidades::aMayusculas(mat);
    for (int i = 0; i < bd.cantAlumnos; i++) {
        if (i == excluirIdx) continue;
        if (utilidades::aMayusculas(bd.matricula[i]) == matUpper) return false;
    }
    return true;
}

bool validarNota(double nota) {
    return nota >= 0.0 && nota <= 100.0;
}

bool validarNombre(string nombre) {
    if (nombre.length() < 3 || nombre.length() > 60) return false;
    for (int i = 0; i < (int)nombre.length(); i++) {
        char c = nombre[i];
        bool valido = (c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z')
                      || c == ' ' || c == '\'' || c == '-';
        if ((unsigned char)c > 127) valido = true;
        if (!valido) return false;
    }
    return true;
}

bool validarCarrera(const datos::ConfigAcademica& config, string carr) {
    for (int i = 0; i < datos::TOTAL_CARRERAS; i++) {
        if (utilidades::compararIgnorandoCase(carr, config.carrerasValidas[i]))
            return true;
    }
    return false;
}

bool validarSemestre(int sem) {
    return sem >= 1 && sem <= 12;
}

bool validarAsistencia(double pct) {
    return pct >= 0.0 && pct <= 100.0;
}

int validarEntradaEntera(string mensaje, int minimo, int maximo) {
    int valor;
    bool valido = false;
    do {
        cout << mensaje;
        cin >> valor;
        if (cin.fail()) {
            cin.clear();
            cin.ignore(10000, '\n');
            cout << "Error: Ingrese un numero valido." << endl;
        } else if (valor < minimo || valor > maximo) {
            cout << "Error: El valor debe estar entre " << minimo
                 << " y " << maximo << "." << endl;
        } else {
            valido = true;
        }
    } while (!valido);
    return valor;
}

double validarEntradaDecimal(string mensaje, double minimo, double maximo) {
    double valor;
    bool valido = false;
    do {
        cout << mensaje;
        cin >> valor;
        if (cin.fail()) {
            cin.clear();
            cin.ignore(10000, '\n');
            cout << "Error: Ingrese un numero valido." << endl;
        } else if (valor < minimo || valor > maximo) {
            cout << "Error: El valor debe estar entre " << minimo
                 << " y " << maximo << "." << endl;
        } else {
            valido = true;
        }
    } while (!valido);
    return valor;
}

} // namespace validacion