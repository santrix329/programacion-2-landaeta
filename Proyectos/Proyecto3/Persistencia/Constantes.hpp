#ifndef CONSTANTES_HPP
#define CONSTANTES_HPP

/* ========================================================================
    MODULO: CONSTANTES GLOBALES
    Descripción: Centraliza todas las configuraciones, rutas de archivos
                 y límites del sistema para facilitar el mantenimiento.
   ======================================================================== */

namespace Constantes {

    /* ---- Rutas de Persistencia (Carpeta Datos) ---- */
    // Nota: El sistema debe verificar la existencia de estas rutas al iniciar.
    static constexpr char ARCHIVO_PRODUCTOS[]     = "datos/productos.bin";
    static constexpr char ARCHIVO_PROVEEDORES[]   = "datos/proveedores.bin";
    static constexpr char ARCHIVO_CLIENTES[]      = "datos/clientes.bin";
    static constexpr char ARCHIVO_TRANSACCIONES[] = "datos/transacciones.bin";
    static constexpr char ARCHIVO_TIENDA[]        = "datos/tienda.bin";

    /* ---- Límites de Estructuras (Reglas de Negocio) ---- */
    // Define el tamaño de los arreglos estáticos para evitar memoria dinámica.
    static constexpr int MAX_HISTORIAL_VENTAS      = 50;
    static constexpr int MAX_COMPRAS_CLIENTE       = 50; // Sincronizado con Cliente.cpp
    static constexpr int MAX_PRODUCTOS_PROVEEDOR   = 100;

    /* ---- Tamaños de Campos de Texto (Strings) ---- */
    // Garantiza que los strncpy tengan un límite seguro y eviten desbordamientos.
    static constexpr int TAM_CODIGO      = 20;
    static constexpr int TAM_NOMBRE      = 100;
    static constexpr int TAM_DESCRIPCION = 200;
    static constexpr int TAM_FECHA       = 30;
    static constexpr int TAM_TELEFONO    = 20;
    static constexpr int TAM_EMAIL       = 60;
    static constexpr int TAM_DIRECCION   = 150;

    /* ---- Configuración Económica y del Sistema ---- */
    static constexpr int VERSION_SISTEMA    = 3;    // Representa la Fase 3 del Proyecto
    static constexpr float IVA_PORCENTAJE   = 0.16f; // Estándar impositivo
    static constexpr char MONEDA_SIMBOLO[] = "USD"; 

    /* ---- Códigos de Colores ANSI (Para una interfaz robusta) ---- */
    static constexpr char COLOR_RST[]   = "\033[0m";
    static constexpr char COLOR_NEG[]   = "\033[1m";
    static constexpr char COLOR_ROJO[]  = "\033[31m";
    static constexpr char COLOR_VERDE[] = "\033[32m";
    static constexpr char COLOR_AMARI[] = "\033[33m";
    static constexpr char COLOR_CIAN[]  = "\033[36m";

}

#endif /* CONSTANTES_HPP */