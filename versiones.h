#ifndef VERSIONES_H
#define VERSIONES_H
#include "archivo.h"
#include "version.h"

struct nodo_version{
    version* version;	
	nodo_version* primer_hijo;
	nodo_version* sig_hermano;
};
typedef struct nodo_version * NVersion;

bool esFormatoValido(char* version);
nodo_version* buscarVersion(nodo_version* raiz, char* version_buscada);
char* obtenerPadre(char* version);
bool existePadreDirecto(Archivo a, char* version);
TipoRet CrearVersion(Archivo &a, char * version, char * error);
void mostrarArbolRecursivo(NVersion &v, int nivel);
TipoRet MostrarVersiones(Archivo a);
void mostrarArbolRecursivo(NVersion &v, int nivel);
TipoRet BorrarVersion(Archivo &a, char * version);



#endif