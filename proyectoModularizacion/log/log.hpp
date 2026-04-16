#ifndef LOG_H
#define LOG_H

#include <string>
#include "datos.hpp"

/**
 * @file log.h
 * @brief Módulo para el registro de eventos del sistema y el historial de boletas.
 */

namespace logSistema {

    /**
     * @brief Registra un evento en el log del sistema con fecha y hora actuales.
     * Si el log está lleno, descarta el evento más antiguo.
     * @param log     Referencia al SistemaLog donde guardar el evento.
     * @param evento  Descripción del evento a registrar.
     */
    void registrarLog(datos::SistemaLog& log, std::string evento);

    /**
     * @brief Agrega una entrada al historial de boletas consultadas.
     * Si el historial está lleno, la entrada se ignora silenciosamente.
     * @param h         Referencia al Historial.
     * @param nombreAlumno Nombre del estudiante.
     * @param fecha     Fecha de la consulta.
     * @param nota      Nota final del estudiante en la consulta.
     */
    void agregarHistorial(datos::Historial& h, std::string nombreAlumno,
                          std::string fecha, double nota);

} // namespace logSistema

#endif // LOG_H