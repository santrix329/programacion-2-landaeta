#include "reportes.hpp"
#include "datos.hpp"
#include "utilidades.hpp"
#include "validacion.hpp"
#include "academicos.hpp"
#include "log.hpp"
#include <iostream>
#include <fstream>
#include <cmath>

using std::string;
using std::cout;
using std::cin;
using std::endl;
using namespace utilidades;

namespace reportes {

void menu(const datos::SistemaNotas& s) {
    cout << endl;
    cout << lineaSeparadora(55, '*') << endl;
    cout << "*   SISTEMA DE GESTION DE NOTAS              *" << endl;
    cout << "*   Instituto Tecnologico ABC                 *" << endl;
    cout << "*   " << obtenerFechaActual() << " | " << obtenerHoraActual() << "   *" << endl;
    cout << lineaSeparadora(55, '*') << endl;
    cout << "  1.  Agregar estudiante"           << endl;
    cout << "  2.  Ver todos los estudiantes"     << endl;
    cout << "  3.  Calcular nota final de un estudiante" << endl;
    cout << "  4.  Calcular notas de todos"       << endl;
    cout << "  5.  Buscar estudiante"             << endl;
    cout << "  6.  Estadisticas del curso"        << endl;
    cout << "  7.  Modificar pesos de evaluacion" << endl;
    cout << "  8.  Tabla de honor"                << endl;
    cout << "  9.  Editar estudiante"             << endl;
    cout << "  10. Retirar/Reinscribir estudiante"<< endl;
    cout << "  11. Reporte por carrera"           << endl;
    cout << "  12. Exportar notas a archivo"      << endl;
    cout << "  13. Estudiantes en riesgo academico" << endl;
    cout << "  14. Ver historial de boletas"      << endl;
    cout << "  15. Ver log del sistema"           << endl;
    cout << "  0.  Salir"                         << endl;
    cout << lineaSeparadora(55, '-') << endl;
    int activos = 0;
    for (int i = 0; i < s.bd.cantAlumnos; i++) if (s.bd.inscrito[i]) activos++;
    cout << "  Estudiantes inscritos: " << activos << "/" << s.bd.cantAlumnos << endl;
    cout << "Seleccione: ";
}

void verEstudiantes(datos::SistemaNotas& s) {
    datos::BDEstudiantes& bd = s.bd;
    if (bd.cantAlumnos == 0) { cout << "No hay estudiantes registrados." << endl; return; }

    cout << "\n" << lineaSeparadora(120, '=') << endl;
    cout << rellenarDerecha("ID", 5) << rellenarDerecha("NOMBRE", 25)
         << rellenarDerecha("MATRICULA", 12) << rellenarDerecha("CARRERA", 16)
         << rellenarDerecha("SEM", 5) << rellenarDerecha("P1", 7)
         << rellenarDerecha("P2", 7) << rellenarDerecha("P3", 7)
         << rellenarDerecha("FINAL", 8) << rellenarDerecha("ASIST", 8)
         << rellenarDerecha("ESTADO", 12) << endl;
    cout << lineaSeparadora(120, '-') << endl;

    for (int i = 0; i < bd.cantAlumnos; i++) {
        string estado = bd.inscrito[i] ? "Inscrito" : "Retirado";
        cout << rellenarDerecha(std::to_string(i + 1), 5)
             << rellenarDerecha(bd.alumno[i], 25)
             << rellenarDerecha(bd.matricula[i], 12)
             << rellenarDerecha(bd.carrera[i], 16)
             << rellenarDerecha(std::to_string(bd.semestre[i]), 5)
             << rellenarDerecha(formatoNota(bd.nota1[i]), 7)
             << rellenarDerecha(formatoNota(bd.nota2[i]), 7)
             << rellenarDerecha(formatoNota(bd.nota3[i]), 7)
             << rellenarDerecha(formatoNota(bd.notaFinal[i]), 8)
             << rellenarDerecha(formatoNota(bd.asistencia[i]) + "%", 8)
             << rellenarDerecha(estado, 12) << endl;
    }
    cout << lineaSeparadora(120, '=') << endl;
    cout << "Total: " << bd.cantAlumnos << " estudiantes" << endl;
    logSistema::registrarLog(s.log, "Consulta de lista de estudiantes");
}

void buscarEstudiante(datos::SistemaNotas& s) {
    datos::BDEstudiantes& bd = s.bd;
    if (bd.cantAlumnos == 0) { cout << "No hay estudiantes." << endl; return; }

    cout << "Buscar por: 1) Nombre  2) Matricula  3) Carrera" << endl;
    int tipo = validacion::validarEntradaEntera("Opcion: ", 1, 3);
    string busq;
    cout << "Texto a buscar: "; cin.ignore(); getline(cin, busq);
    busq = recortarEspacios(busq);

    int resultados = 0;
    cout << "\n--- RESULTADOS ---" << endl;
    for (int i = 0; i < bd.cantAlumnos; i++) {
        bool match = false;
        switch (tipo) {
            case 1: match = contieneTexto(bd.alumno[i], busq); break;
            case 2: match = contieneTexto(bd.matricula[i], busq); break;
            case 3: match = contieneTexto(bd.carrera[i], busq); break;
        }
        if (match) {
            string estado = bd.inscrito[i] ? "Inscrito" : "Retirado";
            cout << "#" << i + 1 << " | " << bd.alumno[i] << " | " << bd.matricula[i]
                 << " | " << bd.carrera[i] << " | Sem " << bd.semestre[i]
                 << " | Final: " << formatoNota(bd.notaFinal[i])
                 << " | " << estado << endl;
            resultados++;
        }
    }
    if (resultados == 0) cout << "No se encontraron resultados." << endl;
    else cout << "Total encontrados: " << resultados << endl;
    logSistema::registrarLog(s.log, "Busqueda: '" + busq + "' - "
                             + std::to_string(resultados) + " resultados");
}

void estadisticas(datos::SistemaNotas& s) {
    datos::BDEstudiantes& bd = s.bd;
    const datos::ConfigAcademica& config = s.config;
    if (bd.cantAlumnos == 0) { cout << "No hay datos." << endl; return; }

    double suma = 0, maxNota = 0, minNota = 100, sumaAsistencia = 0;
    string mejorAlumno = "", peorAlumno = "";
    bd.aprobados = 0; bd.reprobados = 0;
    int cantActivos = 0, bajosAsistencia = 0;

    for (int i = 0; i < bd.cantAlumnos; i++) {
        if (!bd.inscrito[i]) continue;
        cantActivos++;
        double f = academico::calcularFinal(bd, config, i);
        suma += f; sumaAsistencia += bd.asistencia[i];
        if (bd.asistencia[i] < config.asistenciaMinima) bajosAsistencia++;
        if (f > maxNota) { maxNota = f; mejorAlumno = bd.alumno[i]; }
        if (f < minNota) { minNota = f; peorAlumno  = bd.alumno[i]; }
        if (f >= config.notaMinima) bd.aprobados++; else bd.reprobados++;
    }
    if (cantActivos == 0) { cout << "No hay estudiantes inscritos." << endl; return; }

    double promedio = suma / cantActivos;
    double promedioAsist = sumaAsistencia / cantActivos;

    double sumaDif = 0;
    for (int i = 0; i < bd.cantAlumnos; i++) {
        if (!bd.inscrito[i]) continue;
        sumaDif += pow(bd.notaFinal[i] - promedio, 2);
    }
    double desviacion = sqrt(sumaDif / cantActivos);

    // mediana - copia y burbuja
    double notasOrdenadas[datos::MAX_ALUMNOS]; int k = 0;
    for (int i = 0; i < bd.cantAlumnos; i++) {
        if (!bd.inscrito[i]) continue;
        notasOrdenadas[k++] = bd.notaFinal[i];
    }
    for (int i = 0; i < k - 1; i++)
        for (int j = 0; j < k - i - 1; j++)
            if (notasOrdenadas[j] > notasOrdenadas[j+1]) {
                double t = notasOrdenadas[j]; notasOrdenadas[j] = notasOrdenadas[j+1]; notasOrdenadas[j+1] = t;
            }
    double mediana = (k % 2 == 0) ? (notasOrdenadas[k/2-1] + notasOrdenadas[k/2]) / 2.0
                                   : notasOrdenadas[k/2];

    int contA=0, contB=0, contC=0, contD=0, contF=0;
    for (int i = 0; i < bd.cantAlumnos; i++) {
        if (!bd.inscrito[i]) continue;
        string letra = academico::obtenerLetra(bd.notaFinal[i]);
        if (letra=="A") contA++; else if (letra=="B") contB++;
        else if (letra=="C") contC++; else if (letra=="D") contD++; else contF++;
    }

    double pctAprobados = (bd.aprobados * 100.0) / cantActivos;

    cout << "\n" << lineaSeparadora(60, '=') << endl;
    cout << " ESTADISTICAS DEL CURSO" << endl;
    cout << " " << obtenerFechaActual() << endl;
    cout << lineaSeparadora(60, '=') << endl;
    cout << "POBLACION:" << endl;
    cout << " Total registrados:  " << bd.cantAlumnos << endl;
    cout << " Inscritos activos:  " << cantActivos << endl;
    cout << " Retirados:          " << bd.cantAlumnos - cantActivos << endl;
    cout << lineaSeparadora(60, '-') << endl;
    cout << "NOTAS:" << endl;
    cout << " Promedio general:   " << formatoNota(promedio)   << endl;
    cout << " Mediana:            " << formatoNota(mediana)    << endl;
    cout << " Desviacion estandar:" << formatoNota(desviacion) << endl;
    cout << " Nota mas alta: " << mejorAlumno << " (" << formatoNota(maxNota) << ")" << endl;
    cout << " Nota mas baja: " << peorAlumno  << " (" << formatoNota(minNota) << ")" << endl;
    cout << lineaSeparadora(60, '-') << endl;
    cout << "APROBACION:" << endl;
    cout << " Aprobados: " << bd.aprobados << " (" << formatoNota(pctAprobados) << "%)" << endl;
    cout << " Aprobacion: " << barraProgreso(pctAprobados, 30) << endl;
    cout << " Reprobados: " << bd.reprobados << endl;
    cout << lineaSeparadora(60, '-') << endl;
    cout << "ASISTENCIA:" << endl;
    cout << " Promedio asistencia: " << formatoNota(promedioAsist) << "%" << endl;
    cout << " Debajo del minimo:   " << bajosAsistencia << " estudiantes" << endl;
    cout << lineaSeparadora(60, '-') << endl;
    cout << "DISTRIBUCION POR LETRA:" << endl;
    cout << " A (90-100): " << contA << " " << barraProgreso(contA*100.0/cantActivos,15) << endl;
    cout << " B (80-89):  " << contB << " " << barraProgreso(contB*100.0/cantActivos,15) << endl;
    cout << " C (70-79):  " << contC << " " << barraProgreso(contC*100.0/cantActivos,15) << endl;
    cout << " D (60-69):  " << contD << " " << barraProgreso(contD*100.0/cantActivos,15) << endl;
    cout << " F (0-59):   " << contF << " " << barraProgreso(contF*100.0/cantActivos,15) << endl;
    cout << lineaSeparadora(60, '=') << endl;
    logSistema::registrarLog(s.log, "Estadisticas del curso generadas");
}

void modificarPesos(datos::SistemaNotas& s) {
    datos::ConfigAcademica& config = s.config;
    cout << "\n--- CONFIGURACION ACADEMICA ---" << endl;
    cout << "Pesos actuales -> P1: " << config.pesoP1*100 << "%, P2: "
         << config.pesoP2*100 << "%, P3: " << config.pesoP3*100 << "%" << endl;
    cout << "Nuevo peso P1 (ej: 0.25 para 25%): "; cin >> config.pesoP1;
    cout << "Nuevo peso P2: "; cin >> config.pesoP2;
    cout << "Nuevo peso P3: "; cin >> config.pesoP3;
    double total = config.pesoP1 + config.pesoP2 + config.pesoP3;
    if (std::abs(total - 1.0) > 0.01) {
        cout << "ADVERTENCIA: Los pesos suman " << total*100 << "%, no 100%." << endl;
        cout << "Se normalizaran automaticamente." << endl;
        config.pesoP1 /= total; config.pesoP2 /= total; config.pesoP3 /= total;
    }
    cout << "Pesos actualizados: P1=" << config.pesoP1*100
         << "%, P2=" << config.pesoP2*100 << "%, P3=" << config.pesoP3*100 << "%" << endl;
    cout << "\nNota minima aprobatoria actual: " << config.notaMinima << endl;
    config.notaMinima = validacion::validarEntradaDecimal("Nueva nota minima: ", 0, 100);
    cout << "Nota minima para tabla de honor actual: " << config.notaMinimaHonor << endl;
    config.notaMinimaHonor = validacion::validarEntradaDecimal("Nueva nota minima honor: ", 0, 100);
    cout << "Asistencia minima actual: " << config.asistenciaMinima << "%" << endl;
    config.asistenciaMinima = validacion::validarEntradaDecimal("Nueva asistencia minima (%): ", 0, 100);
    cout << "Configuracion guardada." << endl;
    logSistema::registrarLog(s.log, "Configuracion academica modificada");
}

void tablaHonor(datos::SistemaNotas& s) {
    datos::BDEstudiantes& bd = s.bd;
    const datos::ConfigAcademica& config = s.config;
    if (bd.cantAlumnos == 0) { cout << "No hay estudiantes." << endl; return; }

    for (int i = 0; i < bd.cantAlumnos; i++) if (bd.inscrito[i]) academico::calcularFinal(bd, config, i);

    string nombresH[datos::MAX_ALUMNOS], matH[datos::MAX_ALUMNOS], carrH[datos::MAX_ALUMNOS];
    double notasH[datos::MAX_ALUMNOS], asistH[datos::MAX_ALUMNOS];
    int cantH = 0;

    for (int i = 0; i < bd.cantAlumnos; i++) {
        if (!bd.inscrito[i]) continue;
        if (bd.notaFinal[i] >= config.notaMinimaHonor && bd.asistencia[i] >= config.asistenciaMinima) {
            nombresH[cantH] = bd.alumno[i]; matH[cantH] = bd.matricula[i];
            carrH[cantH] = bd.carrera[i]; notasH[cantH] = bd.notaFinal[i];
            asistH[cantH] = bd.asistencia[i]; cantH++;
        }
    }

    if (cantH == 0) {
        cout << "No hay estudiantes que cumplan los requisitos para la tabla de honor." << endl;
        cout << "(Nota >= " << config.notaMinimaHonor << " y Asistencia >= " << config.asistenciaMinima << "%)" << endl;
        return;
    }

    // ordenar por nota desc (burbuja)
    for (int i = 0; i < cantH - 1; i++)
        for (int j = 0; j < cantH - i - 1; j++)
            if (notasH[j] < notasH[j+1]) {
                std::swap(notasH[j], notasH[j+1]); std::swap(asistH[j], asistH[j+1]);
                std::swap(nombresH[j], nombresH[j+1]); std::swap(matH[j], matH[j+1]);
                std::swap(carrH[j], carrH[j+1]);
            }

    cout << "\n" << lineaSeparadora(75, '=') << endl;
    cout << " TABLA DE HONOR" << endl;
    cout << " (Nota >= " << config.notaMinimaHonor << " | Asistencia >= " << config.asistenciaMinima << "%)" << endl;
    cout << lineaSeparadora(75, '=') << endl;
    cout << rellenarDerecha("POS",5) << rellenarDerecha("NOMBRE",25)
         << rellenarDerecha("MATRICULA",12) << rellenarDerecha("CARRERA",16)
         << rellenarDerecha("NOTA",8) << rellenarDerecha("ASIST",8) << endl;
    cout << lineaSeparadora(75, '-') << endl;

    for (int i = 0; i < cantH; i++) {
        string medalla = (i==0) ? " ***" : (i==1) ? " **" : (i==2) ? " *" : "";
        cout << rellenarDerecha(std::to_string(i+1), 5)
             << rellenarDerecha(nombresH[i]+medalla, 25)
             << rellenarDerecha(matH[i], 12)
             << rellenarDerecha(carrH[i], 16)
             << rellenarDerecha(formatoNota(notasH[i]), 8)
             << rellenarDerecha(formatoNota(asistH[i]) + "%", 8) << endl;
    }
    cout << lineaSeparadora(75, '=') << endl;
    logSistema::registrarLog(s.log, "Tabla de honor generada con " + std::to_string(cantH) + " estudiantes");
}

void reportePorCarrera(datos::SistemaNotas& s) {
    datos::BDEstudiantes& bd = s.bd;
    const datos::ConfigAcademica& config = s.config;
    if (bd.cantAlumnos == 0) { cout << "No hay estudiantes." << endl; return; }

    cout << "\n" << lineaSeparadora(65, '=') << endl;
    cout << " REPORTE POR CARRERA" << endl;
    cout << lineaSeparadora(65, '=') << endl;

    for (int c = 0; c < datos::TOTAL_CARRERAS; c++) {
        int cantEnCarrera = 0;
        for (int i = 0; i < bd.cantAlumnos; i++)
            if (bd.inscrito[i] && compararIgnorandoCase(bd.carrera[i], config.carrerasValidas[c]))
                cantEnCarrera++;
        if (cantEnCarrera == 0) continue;

        cout << "\n>> " << aMayusculas(config.carrerasValidas[c])
             << " (" << cantEnCarrera << " estudiantes)" << endl;
        cout << lineaSeparadora(60, '-') << endl;

        double sumaNotas = 0, sumaAsist = 0; int aprobCarrera = 0;
        for (int i = 0; i < bd.cantAlumnos; i++) {
            if (!bd.inscrito[i] || !compararIgnorandoCase(bd.carrera[i], config.carrerasValidas[c])) continue;
            double f = academico::calcularFinal(bd, config, i);
            sumaNotas += f; sumaAsist += bd.asistencia[i];
            if (f >= config.notaMinima) aprobCarrera++;
            cout << "  " << rellenarDerecha(bd.alumno[i], 25)
                 << " | Sem " << bd.semestre[i]
                 << " | Final: " << rellenarDerecha(formatoNota(f), 7)
                 << " (" << academico::obtenerLetra(f) << ")"
                 << " | Asist: " << formatoNota(bd.asistencia[i]) << "%" << endl;
        }
        cout << "  " << lineaSeparadora(55, '.') << endl;
        cout << "  Promedio: " << formatoNota(sumaNotas/cantEnCarrera)
             << " | Asist. promedio: " << formatoNota(sumaAsist/cantEnCarrera) << "%"
             << " | Aprobacion: " << formatoNota(aprobCarrera*100.0/cantEnCarrera) << "%" << endl;
    }
    cout << "\n" << lineaSeparadora(65, '=') << endl;
    logSistema::registrarLog(s.log, "Reporte por carrera generado");
}

void exportarNotas(datos::SistemaNotas& s) {
    datos::BDEstudiantes& bd = s.bd;
    const datos::ConfigAcademica& config = s.config;
    if (bd.cantAlumnos == 0) { cout << "No hay datos para exportar." << endl; return; }

    string nombreArchivo = "notas_" + obtenerFechaActual() + ".txt";
    for (int i = 0; i < (int)nombreArchivo.length(); i++)
        if (nombreArchivo[i] == '/') nombreArchivo[i] = '-';

    std::ofstream archivo(nombreArchivo);
    if (!archivo.is_open()) { cout << "Error: No se pudo crear el archivo." << endl; return; }

    archivo << "REPORTE DE NOTAS - INSTITUTO TECNOLOGICO ABC" << endl;
    archivo << "Fecha: " << obtenerFechaActual() << " " << obtenerHoraActual() << endl;
    archivo << lineaSeparadora(90, '=') << endl;
    archivo << rellenarDerecha("NOMBRE",25) << rellenarDerecha("MATRICULA",12)
            << rellenarDerecha("CARRERA",16) << rellenarDerecha("P1",7)
            << rellenarDerecha("P2",7) << rellenarDerecha("P3",7)
            << rellenarDerecha("FINAL",8) << rellenarDerecha("ESTADO",12) << endl;
    archivo << lineaSeparadora(90, '-') << endl;

    for (int i = 0; i < bd.cantAlumnos; i++) {
        if (!bd.inscrito[i]) continue;
        double f = academico::calcularFinal(bd, config, i);
        archivo << rellenarDerecha(bd.alumno[i],25) << rellenarDerecha(bd.matricula[i],12)
                << rellenarDerecha(bd.carrera[i],16) << rellenarDerecha(formatoNota(bd.nota1[i]),7)
                << rellenarDerecha(formatoNota(bd.nota2[i]),7) << rellenarDerecha(formatoNota(bd.nota3[i]),7)
                << rellenarDerecha(formatoNota(f),8) << rellenarDerecha(academico::obtenerEstado(config,f),12) << endl;
    }
    archivo << lineaSeparadora(90, '=') << endl;
    archivo << "Aprobados: " << bd.aprobados << " | Reprobados: " << bd.reprobados << endl;
    archivo.close();
    cout << "Notas exportadas a: " << nombreArchivo << endl;
    logSistema::registrarLog(s.log, "Notas exportadas a: " + nombreArchivo);
}

void estudiantesEnRiesgo(datos::SistemaNotas& s) {
    datos::BDEstudiantes& bd = s.bd;
    const datos::ConfigAcademica& config = s.config;
    if (bd.cantAlumnos == 0) { cout << "No hay estudiantes." << endl; return; }

    cout << "\n" << lineaSeparadora(75, '=') << endl;
    cout << " ESTUDIANTES EN RIESGO ACADEMICO" << endl;
    cout << lineaSeparadora(75, '=') << endl;

    int enRiesgo = 0;
    for (int i = 0; i < bd.cantAlumnos; i++) {
        if (!bd.inscrito[i]) continue;
        if (academico::estaEnRiesgo(bd, config, i)) {
            double f = bd.notaFinal[i];
            string razones = "";
            if (f < config.notaMinima) razones += "Nota baja";
            if (bd.asistencia[i] < config.asistenciaMinima) {
                if (!razones.empty()) razones += ", ";
                razones += "Baja asistencia";
            }
            if (f >= 55 && f <= 65 && razones.find("Nota baja") == string::npos) {
                if (!razones.empty()) razones += ", ";
                razones += "Zona critica (55-65)";
            }
            cout << "  " << rellenarDerecha(bd.alumno[i], 25)
                 << " | " << bd.matricula[i]
                 << " | Final: " << formatoNota(f)
                 << " | Asist: " << formatoNota(bd.asistencia[i]) << "%"
                 << " | Razon: " << razones << endl;
            enRiesgo++;
        }
    }
    if (enRiesgo == 0) cout << " No hay estudiantes en riesgo academico." << endl;
    else { cout << lineaSeparadora(75, '-') << endl; cout << " Total en riesgo: " << enRiesgo << " estudiantes" << endl; }
    cout << lineaSeparadora(75, '=') << endl;
    logSistema::registrarLog(s.log, "Consulta de estudiantes en riesgo: " + std::to_string(enRiesgo) + " encontrados");
}

void verHistorialBoletas(const datos::Historial& h) {
    if (h.totalHistorial == 0) { cout << "No hay historial de boletas." << endl; return; }
    cout << "\n--- HISTORIAL DE BOLETAS GENERADAS ---" << endl;
    cout << rellenarDerecha("FECHA",15) << rellenarDerecha("ESTUDIANTE",25)
         << rellenarDerecha("NOTA",10) << endl;
    cout << lineaSeparadora(50, '-') << endl;
    int inicio = h.totalHistorial > 20 ? h.totalHistorial - 20 : 0;
    for (int i = inicio; i < h.totalHistorial; i++) {
        cout << rellenarDerecha(h.historialFecha[i], 15)
             << rellenarDerecha(h.historialAlumno[i], 25)
             << rellenarDerecha(formatoNota(h.historialNota[i]), 10) << endl;
    }
    cout << lineaSeparadora(50, '-') << endl;
    cout << "Mostrando ultimos " << h.totalHistorial - inicio << " de " << h.totalHistorial << endl;
}

void verLog(const datos::SistemaLog& log) {
    if (log.totalLog == 0) { cout << "No hay eventos." << endl; return; }
    cout << "\n--- LOG DEL SISTEMA ---" << endl;
    int inicio = log.totalLog > 30 ? log.totalLog - 30 : 0;
    for (int i = inicio; i < log.totalLog; i++) cout << log.logEventos[i] << endl;
    cout << lineaSeparadora(50, '-') << endl;
    cout << "Mostrando ultimos " << log.totalLog - inicio << " de " << log.totalLog << " eventos." << endl;
}

} // namespace reportes