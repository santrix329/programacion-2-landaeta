# 📝 Descripción del Proyecto

### Este proyecto consiste en una aplicación de consola desarrollada en C++ diseñada para la gestión integral de una tienda comercial. El sistema permite el control de inventarios, administración de clientes y proveedores, y el registro automatizado de transacciones (compras y ventas).

### El núcleo del programa se basa en la gestión de memoria dinámica (Heap), permitiendo que las estructuras crezcan según las necesidades del usuario mediante algoritmos de redimensionamiento.

## 🚀 Instrucciones de Compilación

### Para compilar el proyecto, se requiere un compilador compatible con C++11 o superior (como G++). Utilice el siguiente comando en su terminal:
### Bash
### g++ -o sistema_tienda main.cpp
### 💻 Instrucciones de Ejecución
### Una vez generado el ejecutable, puede iniciar el programa con:

### Bash
### ./sistema_tienda
### 🏗️ Estructura del Código
### El código sigue un enfoque de programación modular, separando la lógica de negocio en funciones específicas y utilizando estructuras de datos robustas:

## Estructuras (Structs): Producto, Cliente, Proveedor y Transaccion.

### Gestión de Memoria: Uso de new y delete para el manejo de arreglos dinámicos en el Heap.

### Funciones de Utilidad:

### obtenerFechaActual: Captura la fecha del sistema mediante la librería <ctime>.

### convertirAMinusculas: Estandariza cadenas de caracteres para comparaciones seguras.

### contieneSubstring: Motor de búsqueda para coincidencias parciales.

### validarFecha: Filtro de seguridad para entradas en formato YYYY/MM/DD.

### ✨ Funcionalidades Implementadas
### Gestión de Inventario: Registro de productos con ID autoincremental y actualización automática de stock.

### Módulo de Ventas/Compras: Registro de transacciones vinculadas a clientes y proveedores.
### Búsquedas Inteligentes: Filtros de búsqueda insensibles a mayúsculas y minúsculas para productos y personas.
### Historial de Transacciones: Consulta de movimientos filtrada por tipo, ID o fecha específica.
### Validación de Entradas: Ciclos de validación robusta para opciones S/N y formatos de fecha.

### 🧪 Casos de Prueba Ejecutados
### Prueba de Redimensionamiento: Se superó la capacidad inicial de 5 productos; el sistema redimensionó el arreglo en el Heap exitosamente duplicando su capacidad sin pérdida de datos.

### Búsqueda Case-Insensitive: Se registró el producto "HARINA" y se encontró exitosamente buscando "harina".

### Validación de Formato de Fecha: El sistema rechazó la entrada "2026-02-27" y aceptó "2026/02/27" según la máscara definida.

### Persistencia en Transacciones: Al registrar una venta, la fecha se capturó automáticamente del sistema y se asignó correctamente a la transacción.