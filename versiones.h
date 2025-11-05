#ifndef VERSIONES_H
#define VERSIONES_H
#include "definiciones.h"
#include <string>
using namespace std;


typedef struct nodoDeLaVersion* Version;
typedef struct version* v;
typedef struct nodo_linea* Linea;


TipoRet CrearVersionNueva(Version& nodo, char * version, char * error);
//**funcion delegada de CrearVersion
// Crea una version nueva

TipoRet buscarBorrarVersion(Version& nodo, char * version);
//**funcion delegada de BorrarVersion
// Busca la version para poder borrarla

void recorrerArbol(Version nodo, int nivel);
//**funcion delegada de MostrarVersiones
// Recorre el arbol 

TipoRet versionesInsertarLinea(Version raiz, char *versionId, char *linea, unsigned int nroLinea, char *error);
//--puente de InsertarLinea
// Busca la version para poder insertar la linea

TipoRet versionesBorrarLinea(Version raiz, char *versionId, unsigned int nroLinea, char *error);
//--puente de BorrarLinea
// Busca la version para poder borrar la linea

TipoRet buscarVersionMostrarTexto(Version nodo, char* version, char* nombreArchivo);
//--puente de MostrarTexto
// Busca la version para poder mostrar el texto

TipoRet buscarVersionMostrarCambios(Version nodo, char* version, char* nombreArchivo);
//--puente de MostrarCambios
// Busca la version para poder mostrar el texto

TipoRet versionesIguales(Version nodoinicio, char *version1, char *version2, bool &iguales);
//**funcion delegada de Iguales
// Compara dos versiones completas

TipoRet crearVersionIndependiente(Version nodoinicio, char *version);
//**funcion delegada de VersionIndependiente
// Crea una version independiente

 
bool nodoEsVacio(Version nodo);
// Funcion booleana que responde a si la version es vacía o no
    
#endif