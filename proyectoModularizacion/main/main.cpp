#include "datos.hpp"
#include "academicos.hpp"
#include "reportes.hpp"
#include "log.hpp"
#include "validacion.hpp"
#include <iostream>

int main() {
    datos::SistemaNotas sistema;

    datos::cargarDatosPrueba(sistema);
    logSistema::registrarLog(sistema.log, "Sistema de notas iniciado");

    int op;
    do {
        reportes::menu(sistema);
        std::cin >> op;

        if (std::cin.fail()) {
            std::cin.clear();
            std::cin.ignore(10000, '\n');
            std::cout << "Error: Ingrese un numero valido." << std::endl;
            continue;
        }

        switch (op) {
            case 1:  academico::agregarEstudiante(sistema);       break;
            case 2:  reportes::verEstudiantes(sistema);           break;
            case 3:  academico::calcularNotaIndividual(sistema);  break;
            case 4:  academico::calcularTodas(sistema);           break;
            case 5:  reportes::buscarEstudiante(sistema);         break;
            case 6:  reportes::estadisticas(sistema);             break;
            case 7:  reportes::modificarPesos(sistema);           break;
            case 8:  reportes::tablaHonor(sistema);               break;
            case 9:  academico::editarEstudiante(sistema);        break;
            case 10: academico::toggleInscripcion(sistema);       break;
            case 11: reportes::reportePorCarrera(sistema);        break;
            case 12: reportes::exportarNotas(sistema);            break;
            case 13: reportes::estudiantesEnRiesgo(sistema);      break;
            case 14: reportes::verHistorialBoletas(sistema.historial); break;
            case 15: reportes::verLog(sistema.log);               break;
            case 0:
                logSistema::registrarLog(sistema.log, "Sistema de notas cerrado");
                std::cout << "Hasta luego." << std::endl;
                break;
            default:
                std::cout << "Opcion invalida." << std::endl;
        }

        if (op != 0) {
            std::cout << "\nPresione Enter para continuar...";
            std::cin.ignore();
            std::cin.get();
        }

    } while (op != 0);

    return 0;
}