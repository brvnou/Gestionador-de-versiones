#include "versiones.h"
#include "archivo.h"
#include <string.h>
#include <iostream>
#include <cctype> //para isDigit
using namespace std;





bool esFormatoValido(char* version){
	if (version == NULL || strlen(version) == 0){
		return false;
		//Que no sea NULL o vacio
	}else if(version[0] == '.'){
		return false;
		//Que no empieze con punto 
	}else if (version[strlen(version) - 1] == '.'){
		return false;
		//Que no termine con punto
	}

	for (int i = 0; version[i] != '\0'; i++){ //recorre los digitos de version con i
		if (!isdigit(version[i]) && version[i] != '.')//!isdigit quiere decir "si no es un digito" 
		return false;
		//Para que no contenga algo diferente a nums y puntos
	}

	for(int i = 0; version[i] != '\0' && version[i+1] != '\0'; i++) {	//**version[i] != '\0'** verifica si llegue al final del string
		if (version[i] == '.' && version[i+1] == '.'){
			return false;
		}
		//Para que no tenga puntos consecutivos, voy verificando de a dos
	}
}

nodo_version* buscarVersion(nodo_version* raiz, char* version_buscada) {
	if (raiz == NULL) {
		return NULL;  // Árbol vacío
    }else if(raiz->version->id = version_buscada){
		// Verifica si ESTE nodo es el que buscamos
		return raiz;  // ¡Encontrado!
	}
    
    // Busca PRIMERO en los HIJOS de este nodo (profundidad)
    nodo_version* encontrado = buscarVersion(raiz->primer_hijo, version_buscada);
    if (encontrado != NULL) {
		return encontrado;  // Encontrado en los hijos
    }else{
		// Si no está en hijos, busca en los HERMANOS (mismo nivel)
		return buscarVersion(raiz->sig_hermano, version_buscada);
	}
    
}


char* obtenerPadre(char* version) {
	// Ejemplo: version = "2.15.1"
    
    // Encuentra el ÚLTIMO punto en el string
    char* ultimo_punto = strrchr(version, '.');
    // ultimo_punto apunta a: ".1"
    
    if (ultimo_punto == NULL) {
		return NULL;  // No tiene puntos = es raíz (ej: "1")
    }
    
    // Calcula cuántos caracteres hay hasta el último punto
    int longitud = ultimo_punto - version;
    // longitud = 4 (porque "2.15" tiene 4 caracteres)
    
    // Crea nuevo string con solo la parte del padre
    char* padre = new char[longitud + 1];
    strncpy(padre, version, longitud);  // Copia "2.15"
    padre[longitud] = '\0';             // Termina el string
    
    return padre;  // Retorna "2.15"
}

bool existePadreDirecto(Archivo a, char* version) {
	char* padre = obtenerPadre(version);
	
	if (padre == NULL) {
		return true; // Es una versión raíz, no necesita padre
	}else{
		bool existe = (buscarVersion(a->raiz, padre) != NULL);
		delete[] padre;
		return existe;
	}
	
}


// Función auxiliar recursiva para mostrar el árbol
void mostrarArbolRecursivo(NVersion &v, int nivel) {
	if (v == NULL) return;
	
	// Imprimir indentación según el nivel
	for (int i = 0; i < nivel; i++) {
		cout << "\t";  // Un tabulador por nivel
	}
	
	// Mostrar la versión
	cout << v->version << endl;
	
	// Primero mostrar todos los hijos (aumenta nivel)
	mostrarArbolRecursivo(v->primer_hijo, nivel + 1);
	
	// Luego mostrar hermanos (mismo nivel)
	mostrarArbolRecursivo(v->sig_hermano, nivel);
}


TipoRet CrearVersion(Archivo &a, char * version, char * error){
	if (buscarVersion(a->raiz, version) != NULL) {
		cout << error << "La version ya existe" << endl;
        return ERROR;
    }else{
		// Crear el nodo primero (siempre lo necesitamos)
		nodo_version* nuevaVer = new nodo_version;
		nuevaVer->version->id = new char[strlen(version) + 1];
		nuevaVer->primer_hijo = NULL;
		nuevaVer->sig_hermano = NULL;
		
		// Verificar si tiene padre
		char* padre = obtenerPadre(nuevaVer->version->id);
		
		if (padre != NULL) {
			// Tiene padre - buscar e insertar como hijo
			nodo_version* nodo_padre = buscarVersion(a->raiz, padre);        
			nuevaVer->sig_hermano = nodo_padre->primer_hijo;
			nodo_padre->primer_hijo = nuevaVer;
			delete[] padre;
		} else {
			// No tiene padre - insertar como raíz
			nuevaVer->sig_hermano = a->raiz;
			a->raiz = nuevaVer;
		}
		return OK;
	}
}


TipoRet MostrarVersiones(Archivo a){
	if (a == NULL) {
		cout << "Archivo no existe" << endl;
        return ERROR;
    }else{
		// Mostrar nombre del archivo
		cout << "ARCHIVO: " << a->nombre << endl;
		cout << endl;  // Línea en blanco
		
		// Mostrar árbol de versiones recursivamente
		if (a->raiz != NULL) {
			mostrarArbolRecursivo(a->raiz, 0);
		} else {
			cout << "No hay versiones disponibles" << endl;
		}
		
	}
    
    
    return OK;
}


TipoRet BorrarVersion(Archivo &a, char * version){
// Elimina una versión del archivo si la version pasada por parámetro existe. En otro caso la operación quedará sin efecto.
// Si la versión a eliminar posee subversiones, éstas deberán ser eliminadas también, así como el texto asociado a cada una de las versiones.
// No deben quedar números de versiones libres sin usar. Por lo tanto cuando se elimina una versión, las versiones hermanas que le siguen deben decrementar su numeración (así también sus subversiones dependientes). Por ejemplo, si existen las versiones 2.15.1, 2.15.2 y 2.15.3, y elimino la 2.15.1, la versión 2.15.2 y la 2.15.3 pasan a ser 2.15.1 y 2.15.2 respectivamente, esto incluye a todas las subversiones de estas versiones.

return NO_IMPLEMENTADA;
}