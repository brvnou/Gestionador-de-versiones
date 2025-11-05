#ifndef LINEAS_H
#define LINEAS_H
#include "definiciones.h"


typedef struct nodo_linea* Linea;


TipoRet lineasInsertarLinea(Linea& lineas, char *texto, unsigned int nroLinea, char *error);
//**funcion delegada de InsertarLinea
// Inserta una linea en una version

TipoRet lineasBorrarLinea(Linea& lineas, unsigned int nroLinea, char *error);
//**funcion delegada de BorrarLinea
// Borra una linea en una version 

TipoRet lineasMostrarTexto(Linea lineas);
//**funcion delegada de MostrarTexto
// Muestra el contenido de las lineas de una version en especifica

TipoRet lineasMostrarCambios(Linea lineas, Linea lineasPadre);
//**funcion delegada de MostrarCambios
// Muestra los cambios realizados en una version


Linea crearLineaVacia(Linea lineas);
// Crea una linea vacia

Linea copiarLineas(Linea original);
// Compara dos listas de líneas

bool mismaCantidadLineas(Linea lineas1, Linea lineas2);
// Verifica si dos versiones tienen el mismo número de líneas

bool compararLineas(Linea l1, Linea l2);
// Compara dos listas de líneas

Linea aplicarCambios(Linea lineasBase, Linea cambios);
// Combina las lineas del padre con las de la version actual

void borrarLineas(Linea linea);

#endif