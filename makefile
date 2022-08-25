GCC_OPTIONS=-Wall -pedantic -I ../../../Code/include
GL_OPTIONS=-framework OpenGL -framework GLUT 
COPTIONS=$(GCC_OPTIONS) $(GL_OPTIONS)

all: prog
 
prog: initShader.o skeleton.o
	g++ $(GL_OPTIONS) -o prog initShader.o skeleton.o 
	
initShader.o: ../../../Code/include/Angel.h initShader.cpp
	g++ $(GCC_OPTIONS) -c initShader.cpp
	
skeleton.o: ../../../Code/include/Angel.h skeleton.cpp
	g++ $(GCC_OPTIONS) -c skeleton.cpp	
	
clean:	
	rm initShader.o
	rm skeleton.o
	rm prog
	 

