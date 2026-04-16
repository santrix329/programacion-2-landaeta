#ifndef DATOS_H
#define DATOS_H

#include <string>

/**
 * @file datos.h
 * @brief Define las estructuras de datos centrales del sistema.
 * Reemplaza todas las variables globales del monolito original.
 */

namespace datos {

    /** @brief Capacidad máxima de estudiantes registrados */
    const int MAX_ALUMNOS = 80;

    /** @brief Capacidad máxima de entradas en el log del sistema */
    const int MAX_LOG = 500;

    /** @brief Capacidad máxima del historial de boletas */
    const int MAX_HISTORIAL = 300;

    /** @brief Número total de carreras válidas */
    const int TOTAL_CARRERAS = 6;

    /**
     * @brief Configuración académica del curso.
     * Contiene pesos de evaluación, notas mínimas y carreras válidas.
     */
    struct ConfigAcademica {
        double pesoP1 = 0.30;         /**< Peso del parcial 1 (default 30%) */
        double pesoP2 = 0.30;         /**< Peso del parcial 2 (default 30%) */
        double pesoP3 = 0.40;         /**< Peso del parcial 3 (default 40%) */
        double notaMinima = 60.0;     /**< Nota mínima para aprobar */
        double notaMinimaHonor = 90.0;/**< Nota mínima para tabla de honor */
        double asistenciaMinima = 75.0;/**< Porcentaje mínimo de asistencia */
        std::string carrerasValidas[TOTAL_CARRERAS] = {
            "Ingenieria", "Medicina", "Derecho",
            "Administracion", "Psicologia", "Arquitectura"
        };
    };

    /**
     * @brief Base de datos de estudiantes.
     * Encapsula los arreglos paralelos del monolito original.
     */
    struct BDEstudiantes {
        std::string alumno[MAX_ALUMNOS];
        std::string matricula[MAX_ALUMNOS];
        std::string carrera[MAX_ALUMNOS];
        int semestre[MAX_ALUMNOS];
        double nota1[MAX_ALUMNOS];
        double nota2[MAX_ALUMNOS];
        double nota3[MAX_ALUMNOS];
        double notaFinal[MAX_ALUMNOS];
        double asistencia[MAX_ALUMNOS];
        bool inscrito[MAX_ALUMNOS];
        int cantAlumnos = 0;
        int aprobados = 0;
        int reprobados = 0;
    };

    /**
     * @brief Log de eventos del sistema.
     * Registra operaciones importantes con fecha y hora.
     */
    struct SistemaLog {
        std::string logEventos[MAX_LOG];
        int totalLog = 0;
    };

    /**
     * @brief Historial de boletas consultadas.
     * Guarda las últimas consultas de notas individuales.
     */
    struct Historial {
        std::string historialAlumno[MAX_HISTORIAL];
        std::string historialFecha[MAX_HISTORIAL];
        double historialNota[MAX_HISTORIAL];
        int totalHistorial = 0;
    };

    /**
     * @brief Contenedor principal del sistema.
     * Agrupa todas las estructuras necesarias para el funcionamiento.
     */
    struct SistemaNotas {
        BDEstudiantes bd;
        ConfigAcademica config;
        SistemaLog log;
        Historial historial;
    };

    /**
     * @brief Carga 12 estudiantes de prueba en el sistema.
     * @param sistema Referencia al sistema de notas a poblar.
     */
    void cargarDatosPrueba(SistemaNotas& sistema);

} // namespace datos

#endif // DATOS_H