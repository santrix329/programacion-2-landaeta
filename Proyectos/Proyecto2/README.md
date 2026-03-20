# 🏥 Sistema de Gestión Hospitalaria - V2

## 📝 Descripción del Proyecto
### El sistema es una aplicación de consola robusta escrita en C++ diseñada para la gestión
### eficiente de inventarios, pacientes y proveedores.
### Utiliza persistencia de datos mediante archivos binarios de acceso aleatorio para
### garantizar velocidad y eficiencia en el manejo de información.

## 🚀 Instrucciones de Compilación
### Para compilar el proyecto, se requiere un compilador compatible con C++11 o superior
### (como G++). Utilice el siguiente comando en su terminal:
### Bash
### g++ -o sistema_hospital main.cpp

## 💻 Instrucciones de Ejecución
### Una vez generado el ejecutable, puede iniciar el programa con:
### Bash
### ./sistema_hospital

## 🏗️ Estructura del Código
### El código sigue un enfoque de programación modular, separando la lógica de negocio
### en funciones específicas y utilizando estructuras de datos robustas:
### Estructuras (Structs): Producto, Cliente, Proveedor y Transaccion.
### Gestión de Archivos: Uso de fstream para lectura y escritura binaria con
### posicionamiento aleatorio mediante seekg y seekp.

## 📐 Diagrama de Estructuras y Tamaños (sizeof)
### Para que el acceso aleatorio funcione, cada estructura tiene un tamaño fijo en bytes.
### Aquí se detalla la ocupación de memoria de cada molde de datos:
### ArchivoHeader: 16 bytes. Control de metadatos y punteros de ID.
### Producto: 560 bytes. Almacena datos de artículos e historial de ventas.
### Cliente: 660 bytes. Registro detallado de pacientes.
### Proveedor: 452 bytes. Datos de contacto de empresas suministradoras.
### Transaccion: 356 bytes. Registro de movimientos financieros.
### Tienda: 132 bytes. Configuración y datos del establecimiento.

### Visualización de Memoria (Producto):
### [ID (4B)] + [Nombre (100B)] + [Precio (4B)] + [Eliminado (1B)] + [Otros (447B)]
### Total = 560 Bytes por registro.

## ✨ Funcionalidades Implementadas
### Gestión de Inventario: Registro de productos con ID autoincremental persistente.
### Búsquedas Inteligentes: Filtros por ID exacto y coincidencias parciales por nombre.
### Borrado Lógico: Los registros se marcan como inactivos sin alterar el archivo físico.
### Validación de Entradas: Limpieza de buffer para evitar errores de lectura con cin.

## 🧪 Casos de Prueba Ejecutados
### Prueba de Persistencia: Se verificó que el Header mantiene el ID tras reiniciar.
### Validación de Duplicados: El sistema impide registrar códigos de producto repetidos.
### Acceso Directo: Localización instantánea de registros mediante cálculo de offsets.

### Tamano de ArchivoHeader: 16 bytes
### Tamano de Producto: 560 bytes
### Tamano de Proveedor: 452 bytes
### Tamano de Cliente: 660 bytes
### Tamano de Transaccion: 356 bytes
### Tamano de Tienda: 132 bytes