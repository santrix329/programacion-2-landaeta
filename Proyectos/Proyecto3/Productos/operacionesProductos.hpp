#ifndef OPERACIONES_PRODUCTOS_HPP
#define OPERACIONES_PRODUCTOS_HPP

#include "../tienda/Tienda.hpp" // Necesario para reconocer el tipo Tienda

/* ========================================================================
    MODULO: OPERACIONES DE PRODUCTOS
    Descripcion: Lógica de negocio para la gestión del inventario.
   ======================================================================== */

/**
 * registrarProducto
 * Crea un nuevo objeto Producto, valida sus datos y lo guarda en el archivo.
 * Usa la tienda para obtener y actualizar el contador de IDs.
 */
void registrarProducto(Tienda& tienda);

/**
 * listarProductos
 * Recupera todos los registros del archivo y los muestra en formato
 * de tabla, filtrando aquellos con borrado logico activo.
 */
void listarProductos(Tienda& tienda);

/**
 * buscarProducto
 * Solicita un ID, lo busca en el archivo y muestra la ficha detallada
 * usando Producto::mostrarInformacionCompleta().
 */
void buscarProducto(Tienda& tienda);

/**
 * actualizarProducto
 * Permite modificar campos especificos (Precio, Stock) sin alterar el ID.
 */
void actualizarProducto(Tienda& tienda);

/**
 * eliminarProducto
 * Realiza el marcado de 'eliminado = true' (borrado lógico).
 */
void eliminarProducto(Tienda& tienda);

/**
 * reporteStockCritico
 * Filtra y muestra productos cuyo stock es menor al minimo configurado.
 */
void reporteStockCritico(Tienda& tienda);

void menuProductos(Tienda& tienda);

void actualizarProducto(Tienda& tienda);

#endif /* OPERACIONES_PRODUCTOS_HPP */