#include <string.h>
#include <iostream>
#include "archivo.h"
#include "versiones.h"
using namespace std;

struct nodo_archivo {
    char * nombre;
    Version nodo; // Flecha para poder navegar adentro del arbol
};

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


TipoRet BorrarArchivo(Archivo &a){
    delete a;
	cout << "ARCHIVO BORRADO" << endl;
	return OK;
}


TipoRet CrearVersion(Archivo &a, char * version, char * error){
    return CrearVersionNueva(a->nodo, version, error);
}


TipoRet MostrarVersiones(Archivo a){
    if (a == NULL) {
        cout << "Archivo no existe" << endl;
        return ERROR;
    }else{
        cout << "ARCHIVO: " << a->nombre << endl;
		
		// Mostrar árbol de versiones recursivamente
		if (nodoEsVacio(a->nodo)) {
            cout << "No hay versiones disponibles" << endl;
		} else {
            mostrarArbol(a->nodo, 1);
		}
	}
    return OK;
}


TipoRet InsertarLinea(Archivo &a, char * version, char * linea, unsigned int nroLinea, char * error){
    if (strcmp(version, "") == 0 || strcmp(linea, "") == 0){
        strcpy(error, "Parametros invalidos"); //escribe el mensaje en el buffer
        return ERROR;
    }else{
        // Delegar a versiones.cpp
        return versionesInsertarLinea(a->nodo, version, linea, nroLinea, error);
    }
}

TipoRet BorrarLinea(Archivo &a, char * version, unsigned int nroLinea, char * error){
    if(strcmp(version, "") == 0){
        strcpy(error, "Parametros invalidos"); //escribe el mensaje en el buffer
        return ERROR;
    }else{
        // Delegar a versiones.cpp
        return versionesBorrarLinea(a->nodo, version, nroLinea, error);
    }

}


// Esta función muestra el texto completo de la version, teniendo en cuenta los cambios realizados en dicha versión y en las versiones ancestras, de la cual ella depende.
TipoRet MostrarTexto(Archivo a, char* version){
    if (strcmp(version, "") == 0){
        cout << "Parametros invalidos" << endl;
        return ERROR;
    }else{
        return buscarVersionMostrarTexto(a->nodo, version, a->nombre);
    }
}




TipoRet MostrarCambios(Archivo a, char * version){
    if (strcmp(version, "") == 0){
        cout << "Parametros invalidos" << endl;
        return ERROR;
    }else{
        return buscarVersionMostrarCambios(a->nodo, version, a->nombre);
    }
}



TipoRet BorrarVersion(Archivo &a, char * version){
    return NO_IMPLEMENTADA;
}


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


TipoRet VersionIndependiente(Archivo &a, char * version){
    if (strcmp(version, "") == 0){
        cout << "Parametros invalidos" << endl;
        return ERROR;
    }else{
        return buscarVersionIndependiente(a->nodo, version);
    }
}

