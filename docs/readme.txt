************************************************************************************
************************************************************************************
** Organización de Datos ** 1er cuatrimestre 2010 ** Grupo: "RIDERS ON THE STORM" **
** =====================    =====================    ===========================  **
**                                                                                **
** Integrantes: Arana, Andrés Gastón (86203)                                      **
** -----------  Campa, Sabrina Ayelén (85269)                                     **
**              Conte, Vanesa Gabriela (82997)                                    **
**              Piano, Sergio Matias (85191)                                      **
**                                                                                **
** Tutor: Alejandro Ferrer                                                        **
** -----                                                                          **
**                                                                                **
************************************************************************************
************************************************************************************


Para esta primera entrega se adjunta lo mencionado a continuación:
+ Documentación de Hash y Árbol B+.
+ Makefile.
+ Código fuente de Hash, Árbol B+ y Logger.

Todas las estructuras utilizan bloques de tamaño variable. Se definió un tamaño de 4 kb. Esto es modificable en el archivo "source/build/config.h".

Para la utilización de las estructuras se deberán ingresar datos por consola o por archivo y deberán respetar el siguiente formato:
(clave;valor)

A continuación se enumeran los comandos disponibles:
-B = Buscar una clave o cadena de caracteres (para el logger) en la estructura o archivo.
-I = Ingresar datos a la estructura.
-M = Modificar un dato ya existente en la estructura.
-Q = Quitar un dato de la estructura.
-S = Volcar todos los datos de la estructura en un archivo de texto plano para que pueda ser analizado en una
observación inmediata (sin datos en binario).
-H = Ayuda para la operación con la aplicación.

Para el desarrollo de pruebas unitarias se utilizó un framework denominado TUT (C++ Template Unit Test Framework). Dichas pruebas unitarias están adjuntas al código fuente y se recomienda realizar una lectura de las mismas para una mejor comprensión del código.

Para compilar el ejecutable, situarse en el directorio build y correr el target de make "all". Esto generará los ejecutables hash, btree y logger, que son los manejadores de las estructuras de Hash, Árbol B+, Logger respectivamente, y un ejecutable que corre las pruebas unitarias de TUT mencionadas anteriormente. Adicionalmente, se pueden compilar, linkear y correr las pruebas unitarias a través del target "tests", y se pueden limpiar los compilados con el target "clean".
