#ifndef LINEAS_H
#define LINEAS_H
#include "definiciones.h"


typedef struct nodo_linea* Linea;          //Puntero a un nodo_linea


//funcion delegada de InsertarLinea (archivo.cpp)
TipoRet lineasInsertarLinea(Linea& lineas, char *texto, unsigned int nroLinea, char *error);

//funcion delegada de MostrarTexto (archivo.cpp)
//Muestra el contenido de las lineas de una version en especifica
TipoRet lineasMostrarTexto(Linea lineas);

//funcion delegada de BorrarLinea (archivo.cpp)
TipoRet lineasBorrarLinea(Linea& lineas, unsigned int nroLinea, char *error);


//funcion delegada de MostrarCambios (archivo.cpp)
TipoRet lineasMostrarCambios(Linea lineas, Linea lineasPadre);


Linea crearLineaVacia(Linea lineas);

#endif