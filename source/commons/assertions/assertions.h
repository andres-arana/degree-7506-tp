/******************************************************************************
 * assertions.h
 * 		Archivo maestro de inclusión cuando se quiere utilizar el sistema
 * 		de assertions
******************************************************************************/
#ifndef __COMMONS_ASSERTIONS_ASSERTIONS_H_INCLUDED__
#define __COMMONS_ASSERTIONS_ASSERTIONS_H_INCLUDED__

#include "../../config/config.h"
#include "assertion_failure.h"

#if BUILD_OPTIONS_ASSERTIONS

/**
 * Define un chequeo sobre una condicion dada. Dicho chequeo se realiza
 * sólamente si el build está configurado para incluir los assertions,
 * y eleva un assertion_failure con los detalles de la assertion que falló
 * si la condición no se da.
 */
#define ASSERTION(CONDITION) { if (!(CONDITION)) throw commons::assertions::assertion_failure("'" #CONDITION "' was false", __FILE__, __LINE__); }

/**
 * Define un chequeo sobre una condicion dada. Dicho chequeo se realiza
 * sólamente si el build está configurado para incluir los assertions,
 * y eleva un assertion_failure con los detalles de la assertion que falló
 * si la condición no se da.
 */
#define ASSERTION_WITH_MESSAGE(CONDITION, MESSAGE) { if (!(CONDITION)) throw commons::assertions::assertion_failure("Assertion failure: " MESSAGE, __FILE__, __LINE__); }

/**
 * Define un chequeo que falla siempre. Dicho chequeo se realiza
 * sólamente si el build está configurado para incluir los assertions,
 * y eleva un assertion_failure con los detalles de la assertion que falló.
 */
#define ASSERTION_FAILURE(MESSAGE) throw commons::assertions::assertion_failure("Assertion failure: " MESSAGE, __FILE__, __LINE__)

#else
#define ASSERTION(CONDITION)
#define ASSERTION_WITH_MESSAGE(CONDITION, MESSAGE)
#endif

#endif
