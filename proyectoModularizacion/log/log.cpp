#include "../include/log.h"
#include "../include/utilidades.h"

namespace logSistema {

void registrarLog(datos::SistemaLog& log, std::string evento) {
    if (log.totalLog >= datos::MAX_LOG) {
        for (int i = 0; i < datos::MAX_LOG - 1; i++) {
            log.logEventos[i] = log.logEventos[i + 1];
        }
        log.totalLog = datos::MAX_LOG - 1;
    }
    log.logEventos[log.totalLog] =
        "[" + utilidades::obtenerFechaActual() + " " +
        utilidades::obtenerHoraActual() + "] " + evento;
    log.totalLog++;
}

void agregarHistorial(datos::Historial& h, std::string nombreAlumno,
                      std::string fecha, double nota) {
    if (h.totalHistorial < datos::MAX_HISTORIAL) {
        h.historialAlumno[h.totalHistorial] = nombreAlumno;
        h.historialFecha[h.totalHistorial]  = fecha;
        h.historialNota[h.totalHistorial]   = nota;
        h.totalHistorial++;
    }
}

} // namespace logSistema