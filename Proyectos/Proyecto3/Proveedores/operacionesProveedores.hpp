#ifndef OPERACIONES_PROVEEDORES_HPP
#define OPERACIONES_PROVEEDORES_HPP

#include "../tienda/Tienda.hpp"

/*
 * registrarProveedor
 * Solicita los datos de una nueva empresa proveedora, valida la 
 * informacion y la guarda en el archivo binario.
 */
void registrarProveedor(Tienda& tienda);

/*
 * buscarProveedor
 * Permite localizar un proveedor especifico mediante su ID o RIF
 * y muestra su informacion detallada.
 */
void buscarProveedor(const Tienda& tienda);

/*
 * actualizarProveedor
 * Localiza un proveedor existente y permite modificar sus campos
 * de contacto (Telefono, Email, Direccion).
 */
void actualizarProveedor(Tienda& tienda);

/*
 * eliminarProveedor
 * Realiza el borrado logico de un proveedor, marcandolo como inactivo
 * sin eliminar sus registros historicos.
 */
void eliminarProveedor(Tienda& tienda);

/*
 * listarProveedores
 * Genera un reporte tabular de todos los proveedores activos registrados
 * en el sistema.
 */
void listarProveedores(const Tienda& tienda);

#endif // OPERACIONES_PROVEEDORES_HPP