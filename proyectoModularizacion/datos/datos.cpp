#include "datos.hpp"
#include "log.hpp"
#include <iostream>

namespace datos {

void cargarDatosPrueba(SistemaNotas& s) {
    BDEstudiantes& bd = s.bd;

    bd.alumno[0] = "Sofia Ramirez";       bd.matricula[0] = "INGC-1001";
    bd.carrera[0] = "Ingenieria";         bd.semestre[0] = 4;
    bd.nota1[0] = 85.0; bd.nota2[0] = 90.0; bd.nota3[0] = 88.0;
    bd.asistencia[0] = 95.0; bd.inscrito[0] = true; bd.notaFinal[0] = 0;

    bd.alumno[1] = "Alejandro Vargas";    bd.matricula[1] = "MEDC-2001";
    bd.carrera[1] = "Medicina";           bd.semestre[1] = 6;
    bd.nota1[1] = 72.0; bd.nota2[1] = 65.0; bd.nota3[1] = 58.0;
    bd.asistencia[1] = 82.0; bd.inscrito[1] = true; bd.notaFinal[1] = 0;

    bd.alumno[2] = "Valentina Morales";   bd.matricula[2] = "DERC-3001";
    bd.carrera[2] = "Derecho";            bd.semestre[2] = 3;
    bd.nota1[2] = 92.0; bd.nota2[2] = 95.0; bd.nota3[2] = 97.0;
    bd.asistencia[2] = 98.0; bd.inscrito[2] = true; bd.notaFinal[2] = 0;

    bd.alumno[3] = "Miguel Angel Herrera"; bd.matricula[3] = "INGC-1002";
    bd.carrera[3] = "Ingenieria";          bd.semestre[3] = 4;
    bd.nota1[3] = 55.0; bd.nota2[3] = 60.0; bd.nota3[3] = 62.0;
    bd.asistencia[3] = 70.0; bd.inscrito[3] = true; bd.notaFinal[3] = 0;

    bd.alumno[4] = "Camila Andrea Rios";  bd.matricula[4] = "PSIC-4001";
    bd.carrera[4] = "Psicologia";         bd.semestre[4] = 2;
    bd.nota1[4] = 78.0; bd.nota2[4] = 82.0; bd.nota3[4] = 75.0;
    bd.asistencia[4] = 88.0; bd.inscrito[4] = true; bd.notaFinal[4] = 0;

    bd.alumno[5] = "Daniel Felipe Castro"; bd.matricula[5] = "ADMC-5001";
    bd.carrera[5] = "Administracion";      bd.semestre[5] = 5;
    bd.nota1[5] = 45.0; bd.nota2[5] = 50.0; bd.nota3[5] = 40.0;
    bd.asistencia[5] = 60.0; bd.inscrito[5] = true; bd.notaFinal[5] = 0;

    bd.alumno[6] = "Isabella Jimenez";   bd.matricula[6] = "MEDC-2002";
    bd.carrera[6] = "Medicina";           bd.semestre[6] = 6;
    bd.nota1[6] = 88.0; bd.nota2[6] = 91.0; bd.nota3[6] = 93.0;
    bd.asistencia[6] = 96.0; bd.inscrito[6] = true; bd.notaFinal[6] = 0;

    bd.alumno[7] = "Sebastian Acosta";   bd.matricula[7] = "ARQC-6001";
    bd.carrera[7] = "Arquitectura";       bd.semestre[7] = 1;
    bd.nota1[7] = 30.0; bd.nota2[7] = 25.0; bd.nota3[7] = 20.0;
    bd.asistencia[7] = 40.0; bd.inscrito[7] = false; bd.notaFinal[7] = 0;

    bd.alumno[8] = "Mariana Lopez Vega"; bd.matricula[8] = "INGC-1003";
    bd.carrera[8] = "Ingenieria";         bd.semestre[8] = 7;
    bd.nota1[8] = 63.0; bd.nota2[8] = 58.0; bd.nota3[8] = 61.0;
    bd.asistencia[8] = 73.0; bd.inscrito[8] = true; bd.notaFinal[8] = 0;

    bd.alumno[9] = "Andres Felipe Mora"; bd.matricula[9] = "DERC-3002";
    bd.carrera[9] = "Derecho";            bd.semestre[9] = 3;
    bd.nota1[9] = 70.0; bd.nota2[9] = 75.0; bd.nota3[9] = 80.0;
    bd.asistencia[9] = 90.0; bd.inscrito[9] = true; bd.notaFinal[9] = 0;

    bd.alumno[10] = "Paula Garcia Mendez"; bd.matricula[10] = "PSIC-4002";
    bd.carrera[10] = "Psicologia";         bd.semestre[10] = 2;
    bd.nota1[10] = 95.0; bd.nota2[10] = 92.0; bd.nota3[10] = 98.0;
    bd.asistencia[10] = 100.0; bd.inscrito[10] = true; bd.notaFinal[10] = 0;

    bd.alumno[11] = "Juan Pablo Ortiz";  bd.matricula[11] = "ADMC-5002";
    bd.carrera[11] = "Administracion";    bd.semestre[11] = 8;
    bd.nota1[11] = 60.0; bd.nota2[11] = 62.0; bd.nota3[11] = 65.0;
    bd.asistencia[11] = 78.0; bd.inscrito[11] = true; bd.notaFinal[11] = 0;

    bd.cantAlumnos = 12;

    logSistema::registrarLog(s.log, "Datos de prueba cargados: 12 estudiantes (1 retirado)");
}

} // namespace datos