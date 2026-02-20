#include <iostream>
#include <cstring>
#include <string>
#include <iomanip>
#include <ctime>
#include <cstdlib>
#include <cctype>

using namespace std;

struct Producto {
    int id;
    char codigo[20];
    char nombre[100];
    char descripcion[200];
    int idProveedor;
    float precio;
    int stock;
    char fechaRegistro[11];
};

struct Proveedor {
    int id;
    char nombre[100];
    char rif[20];
    char telefono[20];
    char email[100];
    char direccion[200];
    char fechaRegistro[11];
};

struct Cliente {
    int id;
    char nombre[100];
    char cedula[20];
    char telefono[20];
    char email[100];
    char direccion[200];
    char fechaRegistro[11];
};

struct Transaccion {
    int id;                    
    char tipo[10];            
    int idProducto;           
    int idRelacionado;        
    int cantidad;              
    float precioUnitario;      
    float total;               
    char fecha[11];            
    char descripcion[200];     
};


struct Tienda {
    char nombre[100];          // Nombre de la tienda
    char rif[20];              // RIF de la tienda
    
    // === Arrays dinámicos de entidades (Punteros) ===
    // Se inicializan con capacidad 5 según el punto 2.1
    Producto* productos;
    int numProductos;          // Cuántos hay actualmente
    int capacidadProductos;    // Tamaño máximo actual del array
    
    Proveedor* proveedores;
    int numProveedores;
    int capacidadProveedores;
    
    Cliente* clientes;
    int numClientes;
    int capacidadClientes;
    
    Transaccion* transacciones;
    int numTransacciones;
    int capacidadTransacciones;
    
    // === Contadores para IDs autoincrementales ===
    // Deben empezar en 1 según el punto 2.1
    int siguienteIdProducto;
    int siguienteIdProveedor;
    int siguienteIdCliente;
    int siguienteIdTransaccion;
};

void inicializarTienda(Tienda* tienda, const char* nombre, const char* rif) {
    strcpy(tienda->nombre, nombre); 
    strcpy(tienda->rif, rif);
    
    // Inicializar arrays dinámicos con capacidad 5
    tienda->productos = new Producto[5];
    tienda->numProductos = 0;
    tienda->capacidadProductos = 5;
    
    tienda->proveedores = new Proveedor[5];
    tienda->numProveedores = 0;
    tienda->capacidadProveedores = 5;
    
    tienda->clientes = new Cliente[5];
    tienda->numClientes = 0;
    tienda->capacidadClientes = 5;
    
    tienda->transacciones = new Transaccion[5];
    tienda->numTransacciones = 0;
    tienda->capacidadTransacciones = 5;
    
    // Inicializar contadores de IDs
    tienda->siguienteIdProducto = 1;
    tienda->siguienteIdProveedor = 1;
    tienda->siguienteIdCliente = 1;
    tienda->siguienteIdTransaccion = 1;
}


void liberarTienda(Tienda* tienda) {
    delete[] tienda->productos;
    delete[] tienda->proveedores;
    delete[] tienda->clientes;
    delete[] tienda->transacciones;

    tienda->productos = NULL;
    tienda->proveedores = NULL;
    tienda->clientes = NULL;
    tienda->transacciones = NULL;
}

void crearProducto(Tienda* tienda) {
    char confirmar;
    cout <<"Desea registrar un nuevo producto? (S/N): ";
    cin >> confirmar;
    if(confirmar == 'N' || confirmar == 'n') {
        cout << "Registro de producto cancelado." << endl;
        return;
    }

    char entradaProducto[100];
    cout << " Ingrese el nombre del producto o escriba(cancelar) para cancelar "<< endl;
    cin >> entradaProducto;
    if(strcmp(entradaProducto, "cancelar") == 0) {
        cout << "Registro de producto cancelado." << endl;
        return;
    }


    float precioProducto;
    cout << " Ingrese el precio ddel producto o escriba (0) para cancelr "<< endl;
    cin >> precioProducto;
    if(precioProducto == 0) {
        cout << "Registro de producto cancelado." << endl;
        return;
    }

    if(precioProducto < 0) {
        cout << "El precio del producto no puede ser negativo, se cancela el registro" << endl;
        return;
    }


    int stockProducto;
    cout << " Ingrese el stock del producto o escriba (0) para cancelar "<< endl;
    cin >> stockProducto;
    if(stockProducto == 0) {
        cout << "Registro de producto cancelado." << endl;
        return;
    }

    if(stockProducto < 0) {
        cout << "El stock del producto no puede ser negativo, se cancela el registro" << endl;
        return;
    }

     // El paso que sigue es lo de solicitar el id del proveedor
}



int main() {
    // 1 Creamos la variable de la tienda
    Tienda supertienda;

    // 2 La inicializamos 
    inicializarTienda(&supertienda, "Mi Bodeguita 2.0", "J-98765432-1");

    // 3 Mostramos que si funciono
    cout << "========================================" << endl;
    cout << "TIENDA: " << supertienda.nombre << endl; // Esto imprime lo que copió strcpy
    cout << "RIF: " << supertienda.rif << endl;
    cout << "Capacidad inicial de productos: " << supertienda.capacidadProductos << endl;
    cout << "========================================" << endl;

    // 4. Pausa para que no se cierre
    cout << "\nPresiona Enter para salir...";
    cin.get(); 

    // 5. Limpiamos la memoria antes de irnos
    liberarTienda(&supertienda);

    return 0;
}
