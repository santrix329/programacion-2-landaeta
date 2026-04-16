#ifndef REPORTES_H
#define REPORTES_H

#include "datos.hpp"

/**
 * @file reportes.h
 * @brief Módulo de visualización: menú principal y todas las pantallas de reporte.
 * Depende de academico, utilidades y log para generar la salida.
 */

namespace reportes {

    /** @brief Muestra el menú principal con la fecha/hora actual y el conteo de inscritos. */
    void menu(const datos::SistemaNotas& sistema);

    /** @brief Muestra la tabla completa de todos los estudiantes registrados. */
    void verEstudiantes(datos::SistemaNotas& sistema);

    /** @brief Permite buscar estudiantes por nombre, matrícula o carrera. */
    void buscarEstudiante(datos::SistemaNotas& sistema);

    /**
     * @brief Muestra estadísticas del curso: promedio, mediana, desviación estándar,
     *        distribución por letra y datos de asistencia.
     */
    void estadisticas(datos::SistemaNotas& sistema);

    /**
     * @brief Permite modificar los pesos de evaluación y umbrales académicos.
     * Normaliza automáticamente si los pesos no suman 100%.
     */
    void modificarPesos(datos::SistemaNotas& sistema);

    /**
     * @brief Muestra la tabla de honor ordenada por nota descendente.
     * Incluye solo estudiantes con nota >= notaMinimaHonor y asistencia suficiente.
     */
    void tablaHonor(datos::SistemaNotas& sistema);

    /** @brief Muestra un resumen de promedios y aprobación por cada carrera. */
    void reportePorCarrera(datos::SistemaNotas& sistema);

    /**
     * @brief Exporta las notas a un archivo .txt con nombre basado en la fecha actual.
     */
    void exportarNotas(datos::SistemaNotas& sistema);

    /**
     * @brief Muestra los estudiantes inscritos en riesgo académico con sus razones.
     */
    void estudiantesEnRiesgo(datos::SistemaNotas& sistema);

    /** @brief Muestra los últimos 20 registros del historial de boletas. */
    void verHistorialBoletas(const datos::Historial& historial);

    /** @brief Muestra los últimos 30 eventos del log del sistema. */
    void verLog(const datos::SistemaLog& log);

} // namespace reportes

#endif // REPORTES_H