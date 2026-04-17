#ifndef OPERACIONES_CLIENTES_HPP
#define OPERACIONES_CLIENTES_HPP

#include "../tienda/Tienda.hpp"

/* ========================================================================
    MODULO: OPERACIONES DE CLIENTES
    Descripción: Define las funciones de control para el flujo de clientes.
                 Actúa como mediador entre la Interfaz y el GestorArchivos.
   ======================================================================== */

/**
 * crearCliente
 * Pide los datos al usuario por consola, valida mediante los setters 
 * de la clase Cliente y guarda el nuevo registro usando el Template 
 * de GestorArchivos.
 */
void crearCliente(Tienda& tienda);

/**
 * buscarCliente
 * Solicita un ID, realiza una búsqueda secuencial o por índice y 
 * muestra la información completa del cliente si se encuentra.
 */
void buscarCliente(const Tienda& tienda);

/**
 * actualizarCliente
 * Permite modificar los datos (Teléfono, Email, Dirección) de un 
 * cliente existente y sobreescribe el registro en el archivo binario.
 */
void actualizarCliente(Tienda& tienda);

/**
 * eliminarCliente
 * Realiza un borrado lógico (cambia el atributo 'eliminado' a true)
 * para mantener la integridad referencial de las transacciones pasadas.
 */
void eliminarCliente(Tienda& tienda);

/**
 * listarClientes
 * Recorre el archivo binario de principio a fin y muestra en formato 
 * de tabla todos los clientes que no estén marcados como eliminados.
 */
void listarClientes(const Tienda& tienda);

#endif // OPERACIONES_CLIENTES_HPP