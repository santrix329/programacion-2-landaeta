#ifndef OPERACIONES_TRANSACCIONES_HPP
#define OPERACIONES_TRANSACCIONES_HPP

#include "../tienda/Tienda.hpp"
/*
 * realizarVenta
 * Proceso que solicita ID de cliente y producto, verifica el stock 
 * disponible, descuenta del inventario y registra la transaccion.
 */
void realizarVenta(Tienda& tienda);

/*
 * realizarCompra
 * Proceso para abastecer inventario. Solicita ID de proveedor y producto,
 * aumenta el stock fisico y registra el egreso de la transaccion.
 */
void realizarCompra(Tienda& tienda);

/*
 * consultarHistorial
 * Muestra un listado cronologico de todos los movimientos (Ventas/Compras)
 * realizados en el sistema.
 */
void consultarHistorial(const Tienda& tienda);

/*
 * anularTransaccion
 * Permite realizar un borrado logico de una operacion, revirtiendo el 
 * efecto que tuvo sobre el stock del producto involucrado.
 */
void anularTransaccion(Tienda& tienda);

/*
 * generarReporteDiario
 * Filtra las transacciones por la fecha actual y muestra el balance
 * total de ingresos y egresos.
 */
void generarReporteDiario(const Tienda& tienda);

#endif // OPERACIONES_TRANSACCIONES_HPP