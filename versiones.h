#ifndef VERSIONES_H
#define VERSIONES_H
#include "archivo.h"
#include "version.h"

struct nodoDeLaVersion{
    version* version;	
	nodoDeLaVersion* primer_hijo;
	nodoDeLaVersion* sig_hermano;
};
typedef struct nodoDeLaVersion* NVersion;
typedef struct nodo_archivo * Archivo;

nodoDeLaVersion* buscarVersion(nodoDeLaVersion* raiz, char* version_buscada);
char* obtenerPadre(char* version);
void mostrarArbolRecursivo(NVersion &v, int nivel);
bool existePadreDirecto(Archivo a, char* version);
bool esFormatoValido(char* version);
TipoRet CrearVersion(Archivo &a, char * version, char * error);
TipoRet MostrarVersiones(Archivo a);
TipoRet BorrarVersion(Archivo &a, char * version);



#endif