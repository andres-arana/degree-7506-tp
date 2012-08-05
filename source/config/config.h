/******************************************************************************
 * config.h
 * 		Definición de opciones de configuración para la compilación del
 * 		programa.
******************************************************************************/
#ifndef __BUILD_CONFIG_H_INCLUDED__
#define __BUILD_CONFIG_H_INCLUDED__

/**
 * Prende o apaga el logueo de mensajes criticos, que son errores
 * irrecuperables que evitan que la aplicacion pueda seguir ejecutando
 */
#define LOG_LEVEL_CRITICAL 0

/**
 * Prende o apaga el logueo de mensajes de error, que son errores que no
 * impiden la ejecucion de la aplicacion
 */
#define LOG_LEVEL_ERROR 0

/**
 * Prende o apaga el logueo de mensajes de advertencia, que son condiciones
 * extrañas que pueden indicar que algo puede estar andando mal
 */
#define LOG_LEVEL_WARN 0

/**
 * Prende o apaga el logueo de mensajes de informacion, que son otros
 * mensajes de menos prioridad
 */
#define LOG_LEVEL_INFO 0

/**
 * Prende o apaga el logueo de mensajes de tracing, que son mensajes
 * que indican cuando algo se ejecuta
 */
#define LOG_LEVEL_TRACE 0

/**
 * Prende o apaga el logueo de mensajes de debug, como inspeccion de ciertas
 * variables de ejecucion y otras condiciones
 */
#define LOG_LEVEL_DEBUG 0

/**
 * Establece cual de las salidas del sistema de logging deberia utilizarse
 */
#define LOG_FILE "./logs/runtime.log"

/**
 * Establece el tamaño máximo para el archivo de log. Una vez superado este
 * tamaño se archivará el log y se abrirá un archivo nuevo.
 */
#define LOG_FILE_SIZE 104857600

/**
 * Establece el tamaño de bloque del archivo de datos del hash
 */
#define HASH_BLOCK_SIZE 8192

/**
 * Establece el tamaño de bloque del archivo de datos de B+
 */
#define BPLUS_BLOCK_SIZE 8192

/**
 * Define si se compilan las assertions o no
 */
#define BUILD_OPTIONS_ASSERTIONS 1

/**
 * Define la cantidad de bits, o precisión,
 * del compresor aritmético
 */
#define ARITHMETIC_COMPRESSOR_PRECISION 16

#endif
