#include <string.h>
#include <iostream>
#include "lineas.h"
#include "versiones.h"
using namespace std;

struct nodo_linea{
    char * texto;
    Linea sig_linea;
};


TipoRet lineasInsertarLinea(Linea& lineas, char * texto, unsigned int nroLinea, char *error){
    cout << "DEBUG lineasInsertarLinea: Inicio" << endl;
    cout << "DEBUG: nroLinea = " << nroLinea << endl;
    cout << "DEBUG: texto = " << texto << endl;
    
    // Cuento el total de lineas
    int totalLineas = 0;
    nodo_linea * actual = lineas;
    while (actual != NULL) {
        totalLineas++;
        actual = actual->sig_linea;
    }

    cout << "DEBUG: totalLineas = " << totalLineas << endl;
    
    // Valido nro ingresado
    if (nroLinea <= (unsigned int)(totalLineas + 1)){
        cout << "DEBUG: Validación OK, creando línea..." << endl;
        // Creo nueva linea
        Linea nuevaLinea = new nodo_linea;
        nuevaLinea->texto = new char[strlen(texto) + 1];
        strcpy(nuevaLinea->texto, texto);
        nuevaLinea->sig_linea = NULL;
        
    // Inserto en la posicion especificada
        if(nroLinea == 1){
            cout << "DEBUG: Insertando al inicio" << endl;
            // Inserto al inicio
            nuevaLinea->sig_linea = lineas;
            lineas = nuevaLinea;
        }else{
            cout << "DEBUG: Insertando en posición " << nroLinea << endl;
            // Inserto entre lineas o al final 
            Linea anterior = lineas;
            for (unsigned int i = 1; i < nroLinea - 1; i++){
                anterior = anterior->sig_linea;
            }
            nuevaLinea->sig_linea = anterior->sig_linea;
            anterior->sig_linea = nuevaLinea;
        }

        cout << "DEBUG: Línea insertada exitosamente" << endl;
    
    
    }else{
    cout << "Numero de linea invalido" << endl;
    return ERROR;
    }
    return OK;
}


TipoRet lineasBorrarLinea(Linea& lineas, unsigned int nroLinea, char *error){
    
    // Cuento el total de lineas
    int totalLineas = 0;
    Linea actual = lineas;
    while (actual != NULL) {
        totalLineas++;
        actual = actual->sig_linea;
    }

    if(lineas == NULL){
        cout << "No hay lineas para borrar" << endl;
        return ERROR;
    }else{

        // Valido nro ingresado
        if (nroLinea <= (unsigned int)(totalLineas + 1)){
            Linea auxBorrar;
            
            //Borro en la posicion especificada
            if(nroLinea == 1){
                //Borro al inicio
                auxBorrar = lineas;
                lineas = lineas->sig_linea;

                delete[] auxBorrar->texto;
                delete auxBorrar;
            }else{
                //Borro entre lineas o al final
                Linea anterior = lineas;
                for (unsigned int i = 1; i < nroLinea - 1; i++){
                    anterior = anterior->sig_linea;
                }
                auxBorrar = anterior->sig_linea; 
                //aux pasa a ser la linea que queremos borrar
                anterior->sig_linea = auxBorrar->sig_linea;
                //anterior ahora apunta a la linea despues de la que vamos a borrar
                delete[] auxBorrar->texto;
                delete auxBorrar;
            }
        }else{
        cout << "Numero de linea invalido" << endl;
        return ERROR;
        }
    }
    return OK;
}


TipoRet lineasMostrarTexto(Linea lineas){
    if (lineas == NULL){
        cout << "No contiene lineas" << endl;
        return ERROR;
    }else{
        int nroLinea = 1;
        while(lineas != NULL){
            cout << nroLinea << "-  " << lineas->texto << endl;
            nroLinea++;
            lineas = lineas->sig_linea;
        }
    }
    return OK;
}


TipoRet lineasMostrarCambios(Linea lineas, Linea lineasPadre){
    if(lineas == NULL && lineasPadre == NULL){
    // Ambas vacias, no hay cambios
        cout << "No se realizaron cambios" << endl;
        return OK;
    }else if(lineas == NULL && lineasPadre != NULL){
    // Solo padre tiene lineas: todas son borradas (BL)
    Linea actual = lineasPadre;
    int numLinea = 1;
        while(actual != NULL){
            cout << "BL " << numLinea << endl;
            numLinea++;
            actual = actual->sig_linea;
        }
        return OK;
    }else if(lineas != NULL && lineasPadre == NULL){
    // lineas es raiz: todas son insertadas (IL)
    Linea actual = lineas;
    int numLinea = 1;
        while(actual != NULL){
            cout << "IL " << numLinea << "   " << actual->texto << endl;
            numLinea++;
            actual = actual->sig_linea;
        }
        return OK; 
    }else{
    // Comparar linea por linea
    Linea actualVersion = lineas;
    Linea actualPadre = lineasPadre;
    int numLinea = 1;
    bool hayModificaciones = false;

    while(actualVersion != NULL || actualPadre != NULL){
        // minetras cualquiera de los dos tenga algo 
        
        if(actualVersion != NULL && actualPadre != NULL){
        // cuando las dos tienen algo me fijo esto:
            
            if(strcmp(actualVersion->texto, actualPadre->texto) != 0){
                //Cuando son diferentes es porque hay una insertada (IL)
                cout << "IL " << numLinea << "   " << actualVersion->texto << endl;
                hayModificaciones = true;
            }
    
            actualVersion = actualVersion->sig_linea;
            actualPadre = actualPadre->sig_linea;
        
        }else if(actualVersion != NULL || actualPadre == NULL){
        //Cuando actual version tiene algo y padre no, me fijo esto:
        
        //Hay nuevas lineas al final
        cout << "IL " << numLinea << "   " << actualVersion->texto << endl;
        hayModificaciones = true;
        actualVersion = actualVersion->sig_linea;
        }else if(actualVersion == NULL || actualPadre != NULL){
        //Cuando actual version deja de tener y padre sigue teniendo, me fijo esto:
        
        cout << "BL " << numLinea << endl;
        hayModificaciones = true;
        actualPadre = actualPadre->sig_linea;
        }

        numLinea ++;
    }

    if(!hayModificaciones){
        cout << "No se realizaron modificacines" << endl;
    }
    return OK;
    }
}


Linea crearLineaVacia(Linea lineas){
    lineas = NULL;
    return lineas;
}



// Cuenta líneas de una versión
int contarLineas(Linea l){
    int total = 0;
    while (l != NULL){
        total++;
        l = l->sig_linea;
    }
    return total;
}

// Verifica si dos versiones tienen el mismo número de líneas
bool versionesTienenMismoNumeroDeLineas(Linea lineas1, Linea lineas2){
    int total1 = contarLineas(lineas1);
    int total2 = contarLineas(lineas2);
    return (total1 == total2);
}

// Compara dos listas de líneas
bool compararLineas(Linea l1, Linea l2){
    while (l1 != NULL && l2 != NULL){
        // puede que sea ==0 y que retorne a true
        if (strcmp(l1->texto, l2->texto) != 0){
            return false; // línea distinta
        }else{
            l1 = l1->sig_linea;
            l2 = l2->sig_linea;
        }
    }
    // Si ambas terminaron al mismo tiempo, son iguales
    return (l1 == NULL && l2 == NULL);
}


