#ifndef ACADEMICO_H
#define ACADEMICO_H

#include <string>
#include "datos.hpp"

/**
 * @file academico.h
 * @brief Cálculos académicos: notas finales, letras, estados y riesgo.
 * También contiene las operaciones CRUD sobre estudiantes.
 */

namespace academico {

    /**
     * @brief Calcula la nota final ponderada de un estudiante y la almacena en bd.
     * @param idx Índice del estudiante en la BDEstudiantes.
     * @return Nota final calculada.
     */
    double calcularFinal(datos::BDEstudiantes& bd,
                         const datos::ConfigAcademica& config, int idx);

    /**
     * @brief Convierte una nota numérica a su letra equivalente.
     * A(90+), B(80-89), C(70-79), D(60-69), F(<60).
     */
    std::string obtenerLetra(double nota);

    /**
     * @brief Retorna "APROBADO" o "REPROBADO" según la nota mínima de config.
     */
    std::string obtenerEstado(const datos::ConfigAcademica& config, double nota);

    /**
     * @brief Retorna "Cumple" o "NO CUMPLE" según la asistencia mínima de config.
     */
    std::string obtenerEstadoAsistencia(const datos::ConfigAcademica& config,
                                        double pctAsist);

    /**
     * @brief Determina si un estudiante está en riesgo académico.
     * Criterios: nota < mínima, zona crítica (55-65), o baja asistencia con nota < 70.
     * @param idx Índice del estudiante.
     */
    bool estaEnRiesgo(datos::BDEstudiantes& bd,
                      const datos::ConfigAcademica& config, int idx);

    /**
     * @brief Registra un nuevo estudiante en el sistema.
     * Solicita todos los datos al usuario con validación.
     */
    void agregarEstudiante(datos::SistemaNotas& sistema);

    /**
     * @brief Permite editar un campo específico de un estudiante existente.
     */
    void editarEstudiante(datos::SistemaNotas& sistema);

    /**
     * @brief Alterna el estado de inscripción (activo/retirado) de un estudiante.
     */
    void toggleInscripcion(datos::SistemaNotas& sistema);

    /**
     * @brief Genera y muestra la boleta de calificaciones de un estudiante.
     * Registra la consulta en el historial.
     */
    void calcularNotaIndividual(datos::SistemaNotas& sistema);

    /**
     * @brief Calcula y muestra las notas finales de todos los estudiantes inscritos.
     */
    void calcularTodas(datos::SistemaNotas& sistema);

} // namespace academico

#endif // ACADEMICO_H