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


// Funcion booleana que responde a si la version es vacía o no
bool nodoEsVacio(Version nodo){
	if(!nodo){
		return true;
	}else{
		return false;
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

// Busca la version adentro del arbol
Version buscarVersion(Version nodo, char* version_buscada) {
	if (nodo == NULL){
		return NULL; //arbol vacio
	}else if (strcmp(obtenerfullVer(nodo).c_str(), version_buscada) == 0){ //"c_str" char to string
		//verifico si 'nodo' es el que buscamos
		return nodo; //nodo encontrado
	}else{
		// Busca primero en los hijos del nodo
		Version encontrado = buscarVersion(nodo->primer_hijo, version_buscada);
		if (encontrado != NULL) {
			return encontrado;  // Encontrado en los hijos
		}else{
			// Si no, busco en hermanos
			return buscarVersion(nodo->sig_hermano, version_buscada);
		}
	}	
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
		char* ultimoPunto = strrchr(version, '.');
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

void borrarNodoYSubversiones (Version nodo){
	if (nodo == NULL){
		return;
	}else{
		// Borro todos los hijos
		Version hijo = nodo->primer_hijo;
		while (hijo != NULL){
			Version siguiente = hijo->sig_hermano;
			borrarNodoYSubversiones(hijo);
			hijo = siguiente;
		}

		// B
		borrarLineas(nodo->version->linea);
		delete nodo->version;
		delete nodo;
	}
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

// Obtiene las lineas acumuladas 
Linea obtenerLineasAcumuladas(Version nodo){
	if (nodo == NULL){
		return NULL;
	}else{
		Linea lineasPadre = NULL;
		if (nodo->padre != NULL){
			lineasPadre = obtenerLineasAcumuladas(nodo->padre);
		}
		return aplicarCambios(lineasPadre, nodo->version->linea);
	}
}

// Decrementa un valor menos el id del nodo y de sus hijos
void decrementarIDs (Version nodo){
	if(nodo == NULL){
		return;
	}else{
		// Decremento este nodo
		nodo->version->id--;

		// Decremento todos los hijos
		Version hijo = nodo->primer_hijo;
		while(hijo != NULL){
			decrementarIDs(hijo);
			hijo = hijo->sig_hermano;
		}
		
	}
}

//**funcion delegada de CrearVersion
// Crea una version nueva
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
					
					Version puntero_a_padre = buscarVersion(nodo, padre_id);
					cout << "DEBUG: Insertando " << version << " como hijo de " << padre_id << endl;
					cout << "DEBUG: Padre tiene primer_hijo? " << (puntero_a_padre->primer_hijo == NULL ? "NO" : "SI") << endl;
					if(puntero_a_padre->primer_hijo != NULL){
    				cout << "DEBUG: primer_hijo actual: " << obtenerfullVer(puntero_a_padre->primer_hijo) << endl;
					}
					
					// Si padre_id vale algo, lo inserto como primer hijo
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
							nuevaVer->sig_hermano = puntero_a_padre->primer_hijo;
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

// Busca la version para poder borrarla
TipoRet buscarBorrarVersion (Version& raiz, char * version){
	if(raiz == NULL){
		cout << "No hay versiones disponibles" << endl;
		return ERROR;
	}else{
		Version versionABorrar = buscarVersion(raiz, version);
		
		if(versionABorrar == NULL){
			cout << "Version no encontrada" << endl;
			return ERROR;
		}else{
			// Caso donde es raiz
			if(versionABorrar->padre == NULL){
				// Desconecto de la lista si es el primer nodo
				if(raiz == versionABorrar){
					raiz = versionABorrar->sig_hermano;
				}else{
				// Lo busco entre las raices
					Version anterior = raiz;
					while (anterior != NULL && anterior->sig_hermano != versionABorrar) {
						anterior = anterior->sig_hermano;
					}
					// Salteo a versionABorrar	
					anterior->sig_hermano = versionABorrar->sig_hermano;
				}
				// Decrementar IDs de los hermanos siguientes y sus respectivas subvers
				Version inicio = raiz;
				while(inicio != versionABorrar){
					decrementarIDs(inicio);
					inicio = inicio->sig_hermano;
				}
				// Le desconecto su flecha "sig_hermano"
				versionABorrar->sig_hermano = NULL;
				
				// Borro el nodo y sus subversiones
				borrarNodoYSubversiones(versionABorrar);
			}else{
				// Caso donde tiene padre, es una subversion
				Version padre = versionABorrar->padre;
				
				// Desconecto de la lista de hijos de su padre
				if(padre->primer_hijo == versionABorrar){
					// Cuando es el primer hijo
					padre->primer_hijo = versionABorrar->sig_hermano;
				}else{
					// Cuando está entre los hijos
					Version anterior = padre->primer_hijo;
					while(anterior->sig_hermano != versionABorrar){
						anterior = anterior->sig_hermano;
					}
					anterior->sig_hermano = versionABorrar->sig_hermano;
				}

				// Decremento IDs de los hermanos siguientes y sus respectivas subvers
				Version hermano = versionABorrar->sig_hermano;
				while(hermano != NULL){
					decrementarIDs(hermano);
					hermano = hermano->sig_hermano;
				}
				// Desconecto de hermanos
				versionABorrar->sig_hermano = NULL;
				borrarNodoYSubversiones(versionABorrar);
			}
		}
	}
	return OK;
}

//**Funcion delegada de MostrarVersiones
// Recorre el arbol 
void recorrerArbol(Version nodo, int nivel){
	if (nodo == NULL) return;
	
	// Imprimir indentación según el nivel
	for (int i = 1; i < nivel; i++) {
		cout << "-> ";  // Un tabulador por nivel
	}
	
	// Mostrar la versión
	cout << obtenerfullVer(nodo) << endl;
	
	// Primero mostrar todos los hijos (aumenta nivel)
	recorrerArbol(nodo->primer_hijo, nivel + 1);
	
	// Luego mostrar hermanos (mismo nivel)
	recorrerArbol(nodo->sig_hermano, nivel);
}

//--Puente de InsertarLinea, busca la version de la linea a insertar
// Busca la version para poder insertar la linea
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

// Busca la version para poder borrar la linea
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

// Busca la version para poder mostrar el texto
TipoRet buscarVersionMostrarTexto(Version nodo, char* version, char* nombreArchivo){
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

// Busca la version para poder mostrar el texto
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

// Función que compara dos versiones completas
TipoRet versionesIguales(Version nodoinicio, char *version1, char *version2, bool &iguales){
	// Busco las versiones en el árbol
	Version v1 = buscarVersion(nodoinicio, version1);
	Version v2 = buscarVersion(nodoinicio, version2);

	// Chequeo si tienen el mismo número de líneas
	if (!mismaCantidadLineas(v1->version->linea, v2->version->linea)){
		iguales = false;
		cout << "Las versiones son diferentes" << endl; //*******no imprime esto */
		return ERROR;
	}else{
		// Comparo línea por línea
		iguales = compararLineas(v1->version->linea, v2->version->linea);
		return OK;
	}
}

// Crea una version independiente
TipoRet crearVersionIndependiente(Version nodoinicio, char *version){
	// Busco la version
	Version nodoVer = buscarVersion(nodoinicio, version);

	if (nodoVer == NULL){
		cout << "Version no encontrada" << endl;
		return ERROR;
	}else{
		// Obtengo las lineas acumuladas 
		Linea lineasAcumuladas = obtenerLineasAcumuladas(nodoVer);

		// Encuentro el nuemero de la ultima raiz
		int ultimaRaiz = 0;
		Version actualNodo = nodoinicio;
		while(actualNodo != NULL){
			if(actualNodo->version->id > ultimaRaiz){
				ultimaRaiz = actualNodo->version->id;
			}
			actualNodo = actualNodo->sig_hermano;
		}
	
		// Creo nueva raiz
		Version nuevaRaiz = new nodoDeLaVersion;
		nuevaRaiz->version = new struct version;
		nuevaRaiz->version->id = ultimaRaiz + 1;
		nuevaRaiz->version->linea = NULL;
		nuevaRaiz->primer_hijo = NULL;
		nuevaRaiz->sig_hermano = NULL;
		nuevaRaiz->padre = NULL;
		
		nuevaRaiz->version->linea = copiarLineas(lineasAcumuladas);
		
		if(nodoinicio == NULL){
			nodoinicio = nuevaRaiz;
		}else{
			Version ultimo = nodoinicio;
			while(ultimo->sig_hermano != NULL){
				ultimo = ultimo->sig_hermano;
			}
			ultimo->sig_hermano = nuevaRaiz;
		}
	return OK;
	}
}


