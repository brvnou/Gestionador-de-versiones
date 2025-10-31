#include <string.h>
#include <iostream>
#include "versiones.h"
#include "lineas.h"
#include "archivo.h"
using namespace std;


struct nodoDeLaVersion{
    v version;	
	Version primer_hijo;
	Version sig_hermano;
	Version padre; //separo a los hijos por por su padre
};

struct version{
    int id;
    Linea linea;
};




//**funcion delegada de CrearVersion (archivo.cpp)
TipoRet CrearVersionNueva(Version & nodo, char * version, char * error){
	if(nodo == NULL && strcmp(version, "1") == 0){
		// Si no existe el primer nodo y se quiere insertar version "1"
		// Creo el árbol vacío
		nodo = new nodoDeLaVersion;
		nodo->version = new struct version;
		nodo->version->id = 1;
		nodo->primer_hijo = NULL;
		nodo->sig_hermano = NULL;
		crearLineaVacia(nodo->version->linea);
		return OK;
	}else{
		if (!esFormatoValido(version)) {
			cout << error << "Formato invalido" << endl;
			return ERROR;
		}else{
			if (buscarVersion(nodo, version) != NULL) {
				cout << error << "La version ya existe" << endl;
				return ERROR;
			}else{
				// Crear el nodo primero
				Version nuevaVer = new nodoDeLaVersion;
				nuevaVer->version = new struct version;
				nuevaVer->version->id = obtenerUltimoNumero(version);
				nuevaVer->primer_hijo = NULL;
				nuevaVer->sig_hermano = NULL;
				nuevaVer->padre = NULL;
				
				// Verificar si tiene padre
				char* padre_id = obtenerPadre(version);
				
				if (padre_id != NULL) {
					// Si padre_id vale algo, lo inserto como primer hijo
					Version puntero_a_padre = buscarVersion(nodo, padre_id);
					
					if (puntero_a_padre != NULL){
						if (!esVersionConsecutivaValida(puntero_a_padre, nuevaVer->version->id)){
							// falta version anterior
							cout << "Falta version " << padre_id << "." << (nuevaVer->version->id - 1) << endl;
							delete[] padre_id;
							delete nuevaVer->version;
							delete nuevaVer;
							return ERROR;
						}else{
							//la inserto como primer hijo
							nuevaVer->padre = puntero_a_padre;
							//nuevaVer->sig_hermano = puntero_a_padre->primer_hijo;
							puntero_a_padre ->primer_hijo = nuevaVer;			
							delete[] padre_id;
							return OK;
						}
					}else{
						
						// Caso que no encuentre el padre, tipeo incorrecto
						cout << "Padre de: " << version << " no existe" << endl;
						delete[] padre_id;
						delete nuevaVer->version;
						delete nuevaVer;
						return ERROR;
					}
					
					
				}else{
					// No tiene padre - insertar como raíz
					nuevaVer->sig_hermano = nodo; //hermano se conecta a la lista de "nodo" con sig_hermano
					nodo = nuevaVer; //nuevaVer pasa a ser la cabeza de "nodo"
					delete[] padre_id;
					return OK;
				}
			}
		}
		return OK;
	}
}


//**Funcion delegada de MostrarVersiones
void mostrarArbol(Version nodo, int nivel){
	if (nodo == NULL) return;
	
	// Imprimir indentación según el nivel
	for (int i = 1; i < nivel; i++) {
		cout << "-> ";  // Un tabulador por nivel
	}
	
	// Mostrar la versión
	cout << obtenerfullVer(nodo) << endl;
	
	// Primero mostrar todos los hijos (aumenta nivel)
	mostrarArbol(nodo->primer_hijo, nivel + 1);
	
	// Luego mostrar hermanos (mismo nivel)
	mostrarArbol(nodo->sig_hermano, nivel);
}


// Busca la version adentro del arbol
Version buscarVersion(Version nodo, char* version_buscada) {
	//char* version_buscada = char* version, los dos son el segundo parametro
	if (nodo == NULL){
		return NULL; //arbol vacio
	}else if (strcmp(obtenerfullVer(nodo).c_str(), version_buscada) == 0){ //"c_str" char to string
		//verifico si 'nodo' es el que buscamos
		return nodo; //nodo encontrado
	}else{
		// Busca PRIMERO en los HIJOS de este nodo (profundidad)
		Version encontrado = buscarVersion(nodo->primer_hijo, version_buscada);
		if (encontrado != NULL) {
			return encontrado;  // Encontrado en los hijos
		}else{
			// Si no está en hijos, busca en los HERMANOS (mismo nivel)
			return buscarVersion(nodo->sig_hermano, version_buscada);
		}
	}	
}


// Obtiene la version completa de un int y retorna su arbol completo en String
string obtenerfullVer(Version nodo){
	if (nodo == NULL){
		return ""; //el final 
	}else if (nodo->padre != NULL){
		string versionPadre = obtenerfullVer(nodo->padre);
		return versionPadre + "." + to_string(nodo->version->id);
	}
	return to_string(nodo->version->id);
	// Caso cuando llegamos a la raiz, no va a tener padre
	// entonces retorno solamente el id
}


// Obtiene el padre de la version
char* obtenerPadre(char* version) {
	
	// Encuentra el ultimopunto en el string
    char* ultimo_punto = strrchr(version, '.');
    // ultimo_punto apunta a: ".1"
    
    if (ultimo_punto == NULL) { //si no tiene puntos es raiz
		return NULL;
    }else{
		// calculo cuantos caracteres hay hasta el último punto
		int longitud = ultimo_punto - version;
		// longitud = 4 (porque "2.15" tiene 4 caracteres)
		
		// Crea nuevo string con solo la parte del padre
		char* padre = new char[longitud + 1];
		strncpy(padre, version, longitud);  // Copia "2.15"
		padre[longitud] = '\0';             // Termina el string
		
		return padre;  // Retorna "2.15"
	}
    
}

// Retorna el ultimo numero de la version (si es que tiene)
int obtenerUltimoNumero(char* version){
	if (version == NULL){
		return 0;
		return ERROR;
	}else{
		char* ultimoPunto = strchr(version, '.');
			//strchr busca un caracter
			//obtengo lo que esta detras del utlimo punto
		if (ultimoPunto != NULL){
			return atoi(ultimoPunto + 1);
			//atoi convierte string a int
			//+1 porque quiero convertir el caracter despues del punto 
		}else{
			return atoi (version);
			//si es raiz por ej, ultimoPunto no encuentra nada, 
			//entonces uso version como viene  
		}
	}
}


// Funcion booleana que responde si la version insertada tiene un formato valido
bool esFormatoValido(char* version){
	if (version == NULL || strlen(version) == 0){ //este caso no se usa 
		cout << "caso1" << endl;
		return false;
		//Que no sea NULL o vacio
	}else if(version[0] == '.'){
		cout << "caso2" << endl;
		return false;
		//Que no empieze con punto 
	}else if (version[strlen(version) - 1] == '.'){
		cout << "caso3" << endl;
		return false;
		//Que no termine con punto
	}

	for (int i = 0; version[i] != '\0'; i++){ //recorre los digitos de version con i
		if (!isdigit(version[i]) && version[i] != '.'){//!isdigit quiere decir "si no es un digito" 
			return false;
			//Para que no contenga algo diferente a nums y puntos
		}else{
			return true;
		}
	}

	for(int i = 0; version[i] != '\0' && version[i+1] != '\0'; i++) {	//**version[i] != '\0'** verifica si llegué al final del string
		if (version[i] == '.' && version[i+1] == '.'){
			return false;
		}else{
			return true;
		}
		//Para que no tenga puntos consecutivos, voy verificando de a dos
		return false;
	}
	return false;
}


// Funcion booleana que responde a si la version toma la posta de sus parientes
bool esVersionConsecutivaValida(Version padre, int numeroNuevo){
	cout << "DEBUG validando " << numeroNuevo << " para padre " << padre->version->id << endl;
	if (padre->primer_hijo == NULL){
		if (numeroNuevo == 1){
			cout << "DEBUG cuando el primer hijo es 1" << endl;
			return true;
		}else{
			cout << "DEBUG cuando no tiene primer hijo y quisiste poner >1" << endl;
			return false;
		}
	}else{
		Version hijo = padre->primer_hijo;
		int ultimoNumero = 0;
		// Llevo la cuenta de cuantas subversiones hay 
		while (hijo != NULL){
			cout << "DEBUG cuando tiene primer hijo y quisiste poner >1" << endl;
			if (hijo->version->id > ultimoNumero) {
				
				cout << "DEBUG se inserta: " << numeroNuevo << "como siguiente hermano del primer hijo: " << padre->primer_hijo->version->id << endl;
				cout << "del padre: " << padre->version->id << endl;
				ultimoNumero = hijo->version->id;
			}else{
				return false;
			}
			hijo = hijo->sig_hermano;
		}
		if (numeroNuevo == ultimoNumero + 1){
			return true;
		}else{
			return false;
		}
	}
}


// Funcion booleana que responde a si la version es vacía o no
bool nodoEsVacio(Version nodo){
	if(!nodo){
		return true;
	}else{
		return false;
	}
}


//--Puente de InsertarLinea, busca la version de la linea a insertar
TipoRet versionesInsertarLinea(Version raiz, char* versionID, char *linea, unsigned int nroLinea, char *error){
	// Busco la version
	Version nodoVer = buscarVersion(raiz, versionID);
	
	if(nodoVer == NULL){
		strcpy(error, "Version no encontrada");
		return ERROR;
	}else{
		// Verifico las subversiones
		if(nodoVer->primer_hijo != NULL){
			strcpy(error, "No se pueden modificar versiones con subversiones");
			return ERROR;
		}else{
			return lineasInsertarLinea(nodoVer->version->linea, linea, nroLinea, error);
		}
	}
	
}


TipoRet versionesBorrarLinea(Version raiz, char *versionID, unsigned int nroLinea, char *error){
	// Busco la version
	Version nodoVer = buscarVersion(raiz, versionID);
	
	if(nodoVer == NULL){
		strcpy(error, "Version no encontrada");
		return ERROR;
	}else{
		// Verifico las subversiones
		if(nodoVer->primer_hijo != NULL){
			strcpy(error, "No se pueden modificar versiones con subversiones");
			return ERROR;
		}else{
			return lineasBorrarLinea(nodoVer->version->linea, nroLinea, error);
		}
	}
}



TipoRet buscarVersionMostrarTexto (Version nodo, char* version, char* nombreArchivo){
	// Busco la version
	Version nodoVer = buscarVersion(nodo, version);

	if(nodoVer == NULL){
		cout << "Version no encontrada" << endl;
		return ERROR;
	}else{
		cout << nombreArchivo << " - " << version << endl;
		lineasMostrarTexto(nodoVer->version->linea);
		return OK;
	}
}


TipoRet buscarVersionMostrarCambios(Version nodo, char* version, char* nombreArchivo){
	// Busco la version
	Version nodoVer = buscarVersion(nodo, version);

	if(nodoVer == NULL){
		cout << "Version no encontrada" << endl;
		return ERROR;
	}else{
		char* padre_id = obtenerPadre(version);
		
		if(padre_id != NULL){
			Version versionPadre = buscarVersion(nodo, padre_id);
			
			cout << nombreArchivo << " - " << version << endl;
			lineasMostrarCambios(nodoVer->version->linea, versionPadre->version->linea);
			return OK;
		
		}else{
			// Si no tiene padre, es raiz
			cout << nombreArchivo << " - " << version << endl;
			lineasMostrarCambios(nodoVer->version->linea, NULL);
			return OK;
		}
	}
}


// Comprueba si la versión es NULL o su lista de líneas está vacía
bool esVersionVacia(Version v){
	return (v == NULL || v->version == NULL || v->version->linea == NULL);
}


// Función que compara dos versiones completas
TipoRet versionesIguales(Version nodoinicio, char *version1, char *version2, bool &iguales){
	// Busco las versiones en el árbol
	Version v1 = buscarVersion(nodoinicio, version1);
	Version v2 = buscarVersion(nodoinicio, version2);

	// Pregunto si están vacías usando función tercerizada
	if (v1 == NULL || v2 == NULL){
		cout << "Una o ambas versiones no existen" << endl;
		iguales = false;
		return ERROR;
	}else{
		// Chequeo si tienen el mismo número de líneas
		if (!cantidadLineas(v1->version->linea, v2->version->linea)){
			iguales = false;
			cout << "Las versiones son diferentes" << endl; //*******no imprime esto */
			return ERROR;
		}else{
			// Comparo línea por línea
			iguales = compararLineas(v1->version->linea, v2->version->linea);
			return OK;
		}
	}
}
