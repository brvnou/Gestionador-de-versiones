#include <string.h>
#include <iostream>
#include "archivo.h"
#include "versiones.h"
using namespace std;


struct nodo_archivo {
    char * nombre;
    Version nodo; // Flecha para poder navegar adentro del arbol
};


// Crea el archivo sin contenido
Archivo CrearArchivo(char * nombre){
	Archivo a = new nodo_archivo;
	a->nombre = new(char[MAX_NOMBRE]);
	strcpy(a->nombre, nombre);
    a->nodo = NULL; // CrearVersionVacia()
    
    char version[] = "1";
    char* error = new char[100];
	
    TipoRet resultado = CrearVersionNueva(a->nodo, version, error);
    if(resultado != OK){
        strcpy(error, "No se pudo crear version inicial");
        delete [] a->nombre;
        delete a;
        return NULL;
    }else{
        return a;
    }
}

// Borra el archivo del programa
TipoRet BorrarArchivo(Archivo &a){
    delete a;
	cout << "ARCHIVO BORRADO" << endl;
	return OK;
}

// Verifica que los valores ingresados no sean vacios y se los retorna a "CrearVersionNueva"
TipoRet CrearVersion(Archivo &a, char * version, char * error){
    if(strcmp(version, "") == 0){
        strcpy(error, "Parametros invalidos"); //escribe el mensaje en el buffer
        return ERROR;
    }else{
        return CrearVersionNueva(a->nodo, version, error);
        return OK;
    }
}

// Verifica los parametros y se los retorna a "buscarBorrarVersion" 
TipoRet BorrarVersion(Archivo &a, char * version){
    if (strcmp(version, "") == 0){
        cout << "Parametros invalidos" << endl;
        return ERROR;
    }else{
        return buscarBorrarVersion(a->nodo, version);   
        return OK;
    }
}

// Verifica que existan versiones para mostrar y delega a "recorrerArbol"
TipoRet MostrarVersiones(Archivo a){
    cout << "ARCHIVO: " << a->nombre << endl;
    
    // Mostrar árbol de versiones recursivamente
    if (nodoEsVacio(a->nodo)) {
        cout << "No hay versiones disponibles" << endl;
    } else {
        recorrerArbol(a->nodo, 1);
    }
    return OK;
}

// Verifica los parametros y se los retorna a "versionesInsertarLinea" 
TipoRet InsertarLinea(Archivo &a, char * version, char * linea, unsigned int nroLinea, char * error){
    if (strcmp(version, "") == 0 || strcmp(linea, "") == 0){
        strcpy(error, "Parametros invalidos"); //escribe el mensaje en el buffer
        return ERROR;
    }else{
        // Delegar a versiones.cpp
        return versionesInsertarLinea(a->nodo, version, linea, nroLinea, error);
    }
}

// Verifica los parametros y se los retorna a "versionesBorrarLinea" 
TipoRet BorrarLinea(Archivo &a, char * version, unsigned int nroLinea, char * error){
    if(strcmp(version, "") == 0){
        strcpy(error, "Parametros invalidos"); //escribe el mensaje en el buffer
        return ERROR;
    }else{
        // Delegar a versiones.cpp
        return versionesBorrarLinea(a->nodo, version, nroLinea, error);
    }
    
}

// Verifica los parametros y se los retorna a "buscarVersionMostrarTexto" 
TipoRet MostrarTexto(Archivo a, char* version){
    if (strcmp(version, "") == 0){
        cout << "Parametros invalidos" << endl;
        return ERROR;
    }else{
        return buscarVersionMostrarTexto(a->nodo, version, a->nombre);
    }
}

// Verifica los parametros y se los retorna a "buscarVersionMostrarCambios" 
TipoRet MostrarCambios(Archivo a, char * version){
    if (strcmp(version, "") == 0){
        cout << "Parametros invalidos" << endl;
        return ERROR;
    }else{
        return buscarVersionMostrarCambios(a->nodo, version, a->nombre);
    }
}

// Verifica los parametros y se los retorna a "versionesIguales" 
TipoRet Iguales(Archivo a, char *version1, char *version2, bool &iguales){
    // Solo chequeamos que las cadenas de versiones no sean NULL
    if (strcmp(version1, "") == 0 || strcmp(version2, "") == 0){
        cout << "Parametros invalidos" << endl;
        iguales = false;
        return ERROR;
    }else{
        return versionesIguales(a->nodo, version1, version2, iguales);
    }
}

// Verifica los parametros y se los retorna a "crearVersionIndependiente" 
TipoRet VersionIndependiente(Archivo &a, char * version){
    if (strcmp(version, "") == 0){
        cout << "Parametros invalidos" << endl;
        return ERROR;
    }else{
        return crearVersionIndependiente(a->nodo, version);
    }
}