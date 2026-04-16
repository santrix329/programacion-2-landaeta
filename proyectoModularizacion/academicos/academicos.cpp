#include "academicos.hpp"
#include "utilidades.hpp"
#include "validacion.hpp"
#include "log.hpp"
#include <iostream>
#include <cmath>

using std::string;
using std::cout;
using std::cin;
using std::endl;
using namespace utilidades;

namespace academico {

double calcularFinal(datos::BDEstudiantes& bd, const datos::ConfigAcademica& config, int idx) {
    double f = bd.nota1[idx] * config.pesoP1
             + bd.nota2[idx] * config.pesoP2
             + bd.nota3[idx] * config.pesoP3;
    f = redondear(f, 2);
    bd.notaFinal[idx] = f;
    return f;
}

string obtenerLetra(double nota) {
    if (nota >= 90) return "A";
    if (nota >= 80) return "B";
    if (nota >= 70) return "C";
    if (nota >= 60) return "D";
    return "F";
}

string obtenerEstado(const datos::ConfigAcademica& config, double nota) {
    return (nota >= config.notaMinima) ? "APROBADO" : "REPROBADO";
}

string obtenerEstadoAsistencia(const datos::ConfigAcademica& config, double pctAsist) {
    return (pctAsist >= config.asistenciaMinima) ? "Cumple" : "NO CUMPLE";
}

bool estaEnRiesgo(datos::BDEstudiantes& bd, const datos::ConfigAcademica& config, int idx) {
    double f = calcularFinal(bd, config, idx);
    if (f >= 55 && f <= 65) return true;
    if (bd.asistencia[idx] < config.asistenciaMinima && f < 70) return true;
    if (f < config.notaMinima) return true;
    return false;
}

void agregarEstudiante(datos::SistemaNotas& s) {
    datos::BDEstudiantes& bd = s.bd;
    const datos::ConfigAcademica& config = s.config;

    if (bd.cantAlumnos >= datos::MAX_ALUMNOS) {
        cout << "ERROR: Capacidad maxima alcanzada (" << datos::MAX_ALUMNOS << " estudiantes)." << endl;
        logSistema::registrarLog(s.log, "ERROR: Intento de registro con capacidad maxima");
        return;
    }

    cout << "\n--- REGISTRO DE NUEVO ESTUDIANTE ---" << endl;

    // nombre
    string nombre;
    bool nombreValido = false;
    do {
        cout << "Nombre completo: ";
        cin.ignore();
        getline(cin, nombre);
        nombre = recortarEspacios(nombre);
        if (!validacion::validarNombre(nombre))
            cout << "Error: Nombre invalido (3-60 caracteres, solo letras y espacios)." << endl;
        else nombreValido = true;
    } while (!nombreValido);

    // matricula
    string mat;
    bool matValida = false;
    do {
        cout << "Matricula (formato AAAA-NNNN): ";
        getline(cin, mat);
        mat = recortarEspacios(mat);
        if (!validacion::validarMatricula(mat))
            cout << "Error: Formato invalido. Use 4 letras, guion, 4 numeros." << endl;
        else if (!validacion::validarMatriculaUnica(bd, mat))
            cout << "Error: Ya existe un estudiante con esa matricula." << endl;
        else { mat = aMayusculas(mat); matValida = true; }
    } while (!matValida);

    // carrera
    string carr;
    bool carrValida = false;
    cout << "Carreras disponibles: ";
    for (int i = 0; i < datos::TOTAL_CARRERAS; i++) {
        cout << config.carrerasValidas[i];
        if (i < datos::TOTAL_CARRERAS - 1) cout << ", ";
    }
    cout << endl;
    do {
        cout << "Carrera: ";
        getline(cin, carr);
        carr = recortarEspacios(carr);
        if (!validacion::validarCarrera(config, carr))
            cout << "Error: Carrera no valida." << endl;
        else {
            for (int i = 0; i < datos::TOTAL_CARRERAS; i++) {
                if (compararIgnorandoCase(carr, config.carrerasValidas[i])) {
                    carr = config.carrerasValidas[i]; break;
                }
            }
            carrValida = true;
        }
    } while (!carrValida);

    int sem  = validacion::validarEntradaEntera("Semestre (1-12): ", 1, 12);
    double n1 = validacion::validarEntradaDecimal("Nota parcial 1 (0-100): ", 0, 100);
    double n2 = validacion::validarEntradaDecimal("Nota parcial 2 (0-100): ", 0, 100);
    double n3 = validacion::validarEntradaDecimal("Nota parcial 3 (0-100): ", 0, 100);
    double asist = validacion::validarEntradaDecimal("Porcentaje de asistencia (0-100): ", 0, 100);

    int idx = bd.cantAlumnos;
    bd.alumno[idx] = nombre; bd.matricula[idx] = mat; bd.carrera[idx] = carr;
    bd.semestre[idx] = sem;  bd.nota1[idx] = n1; bd.nota2[idx] = n2;
    bd.nota3[idx] = n3;      bd.asistencia[idx] = asist;
    bd.inscrito[idx] = true; bd.notaFinal[idx] = 0;
    bd.cantAlumnos++;

    cout << "\nEstudiante '" << nombre << "' registrado con matricula " << mat << "." << endl;
    logSistema::registrarLog(s.log, "Estudiante registrado: " + nombre + " (" + mat
                             + ", " + carr + ", Sem " + std::to_string(sem) + ")");
}

void editarEstudiante(datos::SistemaNotas& s) {
    datos::BDEstudiantes& bd = s.bd;
    if (bd.cantAlumnos == 0) { cout << "No hay estudiantes." << endl; return; }

    int num = validacion::validarEntradaEntera(
        "Numero de estudiante (1-" + std::to_string(bd.cantAlumnos) + "): ", 1, bd.cantAlumnos);
    num--;

    cout << "\nEditando: " << bd.alumno[num] << " (" << bd.matricula[num] << ")" << endl;
    cout << "1. Nombre ("    << bd.alumno[num]                << ")" << endl;
    cout << "2. Carrera ("   << bd.carrera[num]               << ")" << endl;
    cout << "3. Semestre ("  << bd.semestre[num]              << ")" << endl;
    cout << "4. Nota P1 ("   << formatoNota(bd.nota1[num])    << ")" << endl;
    cout << "5. Nota P2 ("   << formatoNota(bd.nota2[num])    << ")" << endl;
    cout << "6. Nota P3 ("   << formatoNota(bd.nota3[num])    << ")" << endl;
    cout << "7. Asistencia ("<< formatoNota(bd.asistencia[num]) << "%)" << endl;
    cout << "8. Cancelar" << endl;

    int campo = validacion::validarEntradaEntera("Campo a editar: ", 1, 8);

    switch (campo) {
        case 1: {
            string nuevoNombre;
            cout << "Nuevo nombre: "; cin.ignore(); getline(cin, nuevoNombre);
            nuevoNombre = recortarEspacios(nuevoNombre);
            if (validacion::validarNombre(nuevoNombre)) {
                logSistema::registrarLog(s.log, "Nombre cambiado: " + bd.alumno[num] + " -> " + nuevoNombre);
                bd.alumno[num] = nuevoNombre;
                cout << "Nombre actualizado." << endl;
            } else cout << "Nombre invalido." << endl;
            break;
        }
        case 2: {
            string nuevaCarrera;
            cout << "Carreras: ";
            for (int i = 0; i < datos::TOTAL_CARRERAS; i++) cout << s.config.carrerasValidas[i] << " ";
            cout << "\nNueva carrera: "; cin.ignore(); getline(cin, nuevaCarrera);
            nuevaCarrera = recortarEspacios(nuevaCarrera);
            if (validacion::validarCarrera(s.config, nuevaCarrera)) {
                for (int i = 0; i < datos::TOTAL_CARRERAS; i++) {
                    if (compararIgnorandoCase(nuevaCarrera, s.config.carrerasValidas[i])) {
                        nuevaCarrera = s.config.carrerasValidas[i]; break;
                    }
                }
                logSistema::registrarLog(s.log, "Carrera cambiada para " + bd.alumno[num]);
                bd.carrera[num] = nuevaCarrera;
                cout << "Carrera actualizada." << endl;
            } else cout << "Carrera no valida." << endl;
            break;
        }
        case 3:
            bd.semestre[num] = validacion::validarEntradaEntera("Nuevo semestre (1-12): ", 1, 12);
            cout << "Semestre actualizado." << endl; break;
        case 4:
            bd.nota1[num] = validacion::validarEntradaDecimal("Nueva nota P1 (0-100): ", 0, 100);
            cout << "Nota P1 actualizada." << endl;
            logSistema::registrarLog(s.log, "Nota P1 actualizada para " + bd.alumno[num]); break;
        case 5:
            bd.nota2[num] = validacion::validarEntradaDecimal("Nueva nota P2 (0-100): ", 0, 100);
            cout << "Nota P2 actualizada." << endl;
            logSistema::registrarLog(s.log, "Nota P2 actualizada para " + bd.alumno[num]); break;
        case 6:
            bd.nota3[num] = validacion::validarEntradaDecimal("Nueva nota P3 (0-100): ", 0, 100);
            cout << "Nota P3 actualizada." << endl;
            logSistema::registrarLog(s.log, "Nota P3 actualizada para " + bd.alumno[num]); break;
        case 7:
            bd.asistencia[num] = validacion::validarEntradaDecimal("Nueva asistencia (0-100%): ", 0, 100);
            cout << "Asistencia actualizada." << endl; break;
        case 8:
            cout << "Edicion cancelada." << endl; return;
    }
}

void toggleInscripcion(datos::SistemaNotas& s) {
    datos::BDEstudiantes& bd = s.bd;
    if (bd.cantAlumnos == 0) { cout << "No hay estudiantes." << endl; return; }

    int num = validacion::validarEntradaEntera(
        "Numero de estudiante (1-" + std::to_string(bd.cantAlumnos) + "): ", 1, bd.cantAlumnos);
    num--;

    cout << bd.alumno[num] << " esta actualmente: "
         << (bd.inscrito[num] ? "INSCRITO" : "RETIRADO") << endl;
    cout << "Desea " << (bd.inscrito[num] ? "RETIRAR" : "REINSCRIBIR") << "? (s/n): ";
    char resp; cin >> resp;
    if (resp == 's' || resp == 'S') {
        bd.inscrito[num] = !bd.inscrito[num];
        string nuevoEstado = bd.inscrito[num] ? "reinscrito" : "retirado";
        cout << "Estudiante " << nuevoEstado << "." << endl;
        logSistema::registrarLog(s.log, "Estudiante " + nuevoEstado + ": " + bd.alumno[num]);
    }
}

void calcularNotaIndividual(datos::SistemaNotas& s) {
    datos::BDEstudiantes& bd = s.bd;
    const datos::ConfigAcademica& config = s.config;

    if (bd.cantAlumnos == 0) { cout << "No hay estudiantes." << endl; return; }

    int num = validacion::validarEntradaEntera(
        "Numero de estudiante (1-" + std::to_string(bd.cantAlumnos) + "): ", 1, bd.cantAlumnos);
    num--;

    if (!bd.inscrito[num]) {
        cout << "ADVERTENCIA: Este estudiante esta RETIRADO." << endl;
        cout << "Desea continuar? (s/n): "; char resp; cin >> resp;
        if (resp != 's' && resp != 'S') return;
    }

    double f = calcularFinal(bd, config, num);
    string letra = obtenerLetra(f);
    string estado = obtenerEstado(config, f);
    string estadoAsist = obtenerEstadoAsistencia(config, bd.asistencia[num]);

    cout << "\n" << lineaSeparadora(55, '=') << endl;
    cout << " BOLETA DE CALIFICACIONES" << endl;
    cout << " " << obtenerFechaActual() << endl;
    cout << lineaSeparadora(55, '=') << endl;
    cout << "Estudiante: " << bd.alumno[num]   << endl;
    cout << "Matricula:  " << bd.matricula[num] << endl;
    cout << "Carrera:    " << bd.carrera[num]   << endl;
    cout << "Semestre:   " << bd.semestre[num]  << endl;
    cout << lineaSeparadora(55, '-') << endl;
    cout << "CALIFICACIONES:" << endl;
    cout << " Parcial 1 (" << config.pesoP1 * 100 << "%): " << formatoNota(bd.nota1[num])
         << " -> " << formatoNota(bd.nota1[num] * config.pesoP1) << " pts" << endl;
    cout << " Parcial 2 (" << config.pesoP2 * 100 << "%): " << formatoNota(bd.nota2[num])
         << " -> " << formatoNota(bd.nota2[num] * config.pesoP2) << " pts" << endl;
    cout << " Parcial 3 (" << config.pesoP3 * 100 << "%): " << formatoNota(bd.nota3[num])
         << " -> " << formatoNota(bd.nota3[num] * config.pesoP3) << " pts" << endl;
    cout << lineaSeparadora(55, '-') << endl;
    cout << " NOTA FINAL: " << formatoNota(f) << " (" << letra << ") - " << estado << endl;
    cout << lineaSeparadora(55, '-') << endl;
    cout << "ASISTENCIA:" << endl;
    cout << " " << barraProgreso(bd.asistencia[num]) << endl;
    cout << " Estado: " << estadoAsist << " (minimo: " << config.asistenciaMinima << "%)" << endl;

    if (bd.asistencia[num] < config.asistenciaMinima && f >= config.notaMinima)
        cout << "\n *** ADVERTENCIA: Aprobado por nota pero NO cumple asistencia ***" << endl;
    if (estaEnRiesgo(bd, config, num))
        cout << "\n >>> ALERTA: ESTUDIANTE EN RIESGO ACADEMICO <<<" << endl;

    cout << lineaSeparadora(55, '=') << endl;

    logSistema::agregarHistorial(s.historial, bd.alumno[num], obtenerFechaActual(), f);
    logSistema::registrarLog(s.log, "Boleta generada para: " + bd.alumno[num]
                             + " - Final: " + formatoNota(f) + " (" + letra + ")");
}

void calcularTodas(datos::SistemaNotas& s) {
    datos::BDEstudiantes& bd = s.bd;
    const datos::ConfigAcademica& config = s.config;

    if (bd.cantAlumnos == 0) { cout << "No hay estudiantes." << endl; return; }

    bd.aprobados = 0; bd.reprobados = 0;
    cout << "\n" << lineaSeparadora(80, '=') << endl;
    cout << " NOTAS FINALES - " << obtenerFechaActual() << endl;
    cout << lineaSeparadora(80, '=') << endl;
    cout << rellenarDerecha("NOMBRE", 25) << rellenarDerecha("CARRERA", 16)
         << rellenarDerecha("FINAL", 8) << rellenarDerecha("LETRA", 7)
         << rellenarDerecha("ESTADO", 12) << rellenarDerecha("ASIST", 10) << endl;
    cout << lineaSeparadora(80, '-') << endl;

    for (int i = 0; i < bd.cantAlumnos; i++) {
        if (!bd.inscrito[i]) continue;
        double f = calcularFinal(bd, config, i);
        string estado = obtenerEstado(config, f);
        string letra  = obtenerLetra(f);
        string estadoAsist = obtenerEstadoAsistencia(config, bd.asistencia[i]);
        cout << rellenarDerecha(bd.alumno[i], 25) << rellenarDerecha(bd.carrera[i], 16)
             << rellenarDerecha(formatoNota(f), 8) << rellenarDerecha(letra, 7)
             << rellenarDerecha(estado, 12) << rellenarDerecha(estadoAsist, 10) << endl;
        if (f >= config.notaMinima) bd.aprobados++;
        else bd.reprobados++;
    }
    cout << lineaSeparadora(80, '-') << endl;
    cout << "Aprobados: " << bd.aprobados << " | Reprobados: " << bd.reprobados << endl;
    cout << lineaSeparadora(80, '=') << endl;
    logSistema::registrarLog(s.log, "Calculo masivo de notas realizado");
}

} // namespace academico