#ifndef VERSIONES_H
#define VERSIONES_H
#include "definiciones.h"
#include <string>
using namespace std;



typedef struct nodoDeLaVersion* Version;   //Puntero a un nodoDeLaversion
typedef struct version* v;                 //Puntero a una version
typedef struct nodo_linea* Linea;          //Puntero a un nodo_linea

//**funcion delegada de CrearVersion
TipoRet CrearVersionNueva(Version & nodo, char * version, char * error);

//**funcion delegada de MostrarVersiones
void mostrarArbol(Version nodo, int nivel);

// Busca la version adentro del arbol
Version buscarVersion(Version nodo, char* version_buscada);

// Obtiene la version completa de un int y retorna su arbol completo en String
string obtenerfullVer(Version nodo);

// Obtiene el padre de la version
char* obtenerPadre(char* version);

// Retorna el ultimo numero de la version (si es que tiene)
int obtenerUltimoNumero(char* version);

// Funcion booleana que responde si la version insertada tiene un formato valido
bool esFormatoValido(char* version);

// Funcion booleana que responde a si la version toma la posta de sus parientes
bool esVersionConsecutivaValida(Version padre, int numeroNuevo);

// Funcion booleana que responde a si la version es vacía o no
bool nodoEsVacio(Version nodo);

Linea obtenerVersionObjetivo(Version versionObjetivo);


//**funcion delegada de BorrarVersion
//**funcion delegada de Iguales
//**funcion delegada de VersionIndependiente

//--puente de BorrarLinea
TipoRet versionesBorrarLinea(Version raiz, char *versionId, unsigned int nroLinea, char *error);


//--puente de MostrarTexto
TipoRet buscarVersionMostrarTexto(Version nodo, char* version, char* nombreArchivo);

//--puente de MostrarCambios
TipoRet buscarVersionMostrarCambios(Version nodo, char* version, char* nombreArchivo);

//--puente de InsertarLinea
TipoRet versionesInsertarLinea(Version raiz, char *versionId, char *linea, unsigned int nroLinea, char *error);

#endif