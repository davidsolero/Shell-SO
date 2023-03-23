/*
   Redirección de la salida de un "programa" en un fichero.

   Compilar:
	  gcc redirec.c
   Ejemplo de uso:
	  ./a.out entrada.txt salida.txt sort

   Usa el fichero del primer argumento como entrada del programa y el
   segundo fichero como salida del programa en sustitucion de las
   entradas y salidas estandar. No hace ningún control de errores
   (no comprueba que exista el programa ni los permisos de ejecución)
*/

#include <unistd.h>
#include <stdio.h>

int main(int argc, char *argv[])
{
	FILE *infile, *outfile;
	int fnum1, fnum2;

	// Requiere 4 argumentos incluido el nombre del propio ejecutable
	if (4 > argc)
	{
		printf("\tError: Uso: %s ls /tmp/salida.txt\n", argv[0]);
		return (-1);
	}

	argv++;	// saltar el nombre del ejecutable

	// abrimos el fichero de entrada
	if (NULL == (infile = fopen(argv[0], "r")))
	{
		// si hay erorr, informamos y salimos
		printf("\tError: abriendo: %s\n", argv[0]);
		return (-1);
	}

	// redireccionamos la entrada estandar (stdin) al fichero que acabamos de abrir (infile)
	fnum1 = fileno(infile);
	fnum2 = fileno(stdin);
	if (dup2(fnum1, fnum2) == -1)
	{
		// si hay error, informamos y salimos
		printf("\tError: redireccionando entrada\n");
		return (-1);
	}

	argv++; // saltar el nombre del fichero de entrada

	// abrimos el fichero de salida
	if (NULL == (outfile = fopen(argv[0], "w")))
	{
		// si hay erorr, informamos y salimos
		printf("\tError: abriendo: %s\n", argv[0]);
		return (-1);
	}

	// redireccionamos la salida estandar (stdout) al fichero que acabamos de abrir (outfile)
	fnum1 = fileno(outfile);
	fnum2 = fileno(stdout);
	if (dup2(fnum1, fnum2) == -1)
	{
		// si hay error, informamos y salimos
		printf("\tError: redireccionando salida\n");
		return (-1);
	}

	argv++; // saltar el nombre del fichero de salida

	// ejecutamos el programa con la entrada y salida redireccionadas
	execvp(argv[0], argv);

	// si llegamos aqui es que exec ha fallado
	fclose(infile); 
	fclose(outfile);
	return -1;
}
