tcc -c bmp.c && tcc -c %1.c && tcc -o %1.exe %1.o bmp.o
@del %1.o