todo: main.o archivo.o versiones.o lineas.o
	g++ -Wall -o main main.o archivo.o versiones.o lineas.o
main.o: main.cpp
	g++ -Wall -c main.cpp
archivo.o: archivo.h archivo.cpp
	g++ -Wall -c archivo.cpp
versiones.o: versiones.h versiones.cpp
	g++ -Wall -c versiones.cpp
lineas.o: lineas.h lineas.cpp
	g++ -Wall -c lineas.cpp

limpiar:
	rm -f main.exe
	rm -f *.o