#ifndef VALIDACION_H
#define VALIDACION_H

#include <string>
#include "datos.hpp"

/**
 * @file validacion.h
 * @brief Funciones para validar entradas del usuario y datos del sistema.
 */

namespace validacion {

    /**
     * @brief Valida que la matrícula tenga el formato AAAA-NNNN.
     * @return true si el formato es correcto.
     */
    bool validarMatricula(std::string mat);

    /**
     * @brief Verifica que la matrícula no esté ya registrada.
     * @param excluirIdx Índice a excluir de la comparación (-1 para ninguno).
     * @return true si la matrícula es única.
     */
    bool validarMatriculaUnica(const datos::BDEstudiantes& bd,
                               std::string mat, int excluirIdx = -1);

    /** @brief Valida que la nota esté entre 0.0 y 100.0 */
    bool validarNota(double nota);

    /**
     * @brief Valida que el nombre tenga entre 3 y 60 caracteres
     *        y contenga solo letras, espacios, apóstrofes o guiones.
     */
    bool validarNombre(std::string nombre);

    /**
     * @brief Verifica que la carrera exista en la lista de carreras válidas.
     * @param config Configuración académica con el arreglo de carreras.
     */
    bool validarCarrera(const datos::ConfigAcademica& config, std::string carr);

    /** @brief Valida que el semestre esté entre 1 y 12 */
    bool validarSemestre(int sem);

    /** @brief Valida que el porcentaje de asistencia esté entre 0 y 100 */
    bool validarAsistencia(double pct);

    /**
     * @brief Solicita y valida una entrada entera en un rango dado.
     * Repite la solicitud hasta recibir un valor válido.
     */
    int validarEntradaEntera(std::string mensaje, int minimo, int maximo);

    /**
     * @brief Solicita y valida una entrada decimal en un rango dado.
     * Repite la solicitud hasta recibir un valor válido.
     */
    double validarEntradaDecimal(std::string mensaje, double minimo, double maximo);

} // namespace validacion

#endif // VALIDACION_H