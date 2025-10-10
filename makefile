todo: main.o archivo.o
	g++ -Wall -o main main.o archivo.o
main.o: main.cpp
	g++ -Wall -c main.cpp
archivo.o: archivo.h archivo.cpp
	g++ -Wall -c archivo.cpp
versiones.o: versiones.h versiones.cpp
	g++ -Wall -c versiones.cpp
version.o: version.h version.cpp
	g++ -Wall -c version.cpp
limpiar:
	rm -f main
	rm -f *.o

