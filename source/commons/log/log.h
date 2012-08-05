/******************************************************************************
 * log.h
 * 		Archivo maestro a incluir cuando se quieren utilizar los servicios de
 * 		log de la aplicacion
******************************************************************************/
#ifndef __COMMONS_LOG_LOG_H_INCLUDED__
#define __COMMONS_LOG_LOG_H_INCLUDED__

#include "logger.h"
#include "../../config/config.h"
#include "../utils/string_utils.h"

#if LOG_LEVEL_CRITICAL

/**
 * Loguea un error crítico
 */
#define LOG_CRITICAL(WHAT) commons::log::logger::instance.critical(WHAT, __FILE__, __LINE__)

#else
#define LOG_CRITICAL(WHAT)
#endif

#if LOG_LEVEL_ERROR

/**
 * Loguea un error
 */
#define LOG_ERROR(WHAT) commons::log::logger::instance.error(WHAT, __FILE__, __LINE__)

#else
#define LOG_ERROR(WHAT)
#endif

#if LOG_LEVEL_WARN

/**
 * Loguea una advertencia
 */
#define LOG_WARN(WHAT) commons::log::logger::instance.warn(WHAT, __FILE__, __LINE__)

#else
#define LOG_WARN(WHAT)
#endif

#if LOG_LEVEL_INFO

/**
 * Loguea un mensaje informativo
 */
#define LOG_INFO(WHAT) commons::log::logger::instance.info(WHAT, __FILE__, __LINE__)

#else
#define LOG_INFO(WHAT)
#endif

#if LOG_LEVEL_TRACE

/**
 * Loguea un la ejecución de un punto de control
 */
#define LOG_TRACE(WHAT) commons::log::logger::instance.trace(WHAT, __FILE__, __LINE__)

#else
#define LOG_TRACE(WHAT)
#endif

#if LOG_LEVEL_DEBUG

/**
 * Loguea una condición de depuración
 */
#define LOG_DEBUG(WHAT) commons::log::logger::instance.debug(WHAT, __FILE__, __LINE__)
/**
 * Loguea el valor de una variable
 */
#define LOG_DEBUG_VAR(WHAT) commons::log::logger::instance.debug_var(#WHAT, commons::utils::strings::anything_to_string(WHAT), __FILE__, __LINE__)

#else
#define LOG_DEBUG(WHAT)
#define LOG_DEBUG_VAR(WHAT)
#endif

#endif
