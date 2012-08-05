/******************************************************************************
 * symbol_distribution.h
 * 		Declaraciones de la clase arithmetic::symbol_distribution
******************************************************************************/
#ifndef __ARITHMETIC_SYMBOL_DISTRIBUTION_H_INCLUDED__
#define __ARITHMETIC_SYMBOL_DISTRIBUTION_H_INCLUDED__

#include "symbol.h"
#include "../commons/io/block.h"
#include "../commons/io/serializators.h"
#include <set>


namespace arithmetic {

/**
 * Representa una distribución de probabilidades de símbolos
 */
class symbol_distribution {
private:
	unsigned int frequencies[ARITHMETIC_SYMBOL_COUNT];
	unsigned int total_frequencies;
public:
	/**
	 * Crea una nueva instancia de symbol_distribution
	 * que contiene sólo el símbolo esc
	 */
	symbol_distribution(bool default_esc = true);

	/**
	 * Crea una nueva instancia de symbol_distribution
	 * vacia que contiene sólamente el símbolo dado.
	 */
	symbol_distribution(const symbol &s);

	/**
	 * Registra la emisión de un símbolo dado
	 */
	void register_symbol_emision(const symbol &s);

	/**
	 * Determina si existe un símbolo en la distribución
	 */
	bool has_symbol(const symbol &s) const;

	/**
	 * Determina la probabilidad acumulada de todos los
	 * simbolos anteriores a un simbolo dado que DEBE estar
	 * en la distribución de probabilidades
	 */
	double get_accumulated_probability(const symbol &s) const;

	/**
	 * Determina la probabilidad que de un símbolo dado
	 * ocurra.
	 */
	double get_probability_of(const symbol &s) const;

	/**
	 * Devuelve true si sólamente hay un símbolo en la distribución
	 */
	bool has_only_the_symbol(const symbol &s) const;

	/**
	 * Devuelve la cantidad de veces que un símbolo fue emitido
	 */
	unsigned int get_emissions(const symbol &s) const;

	/**
	 * Tipo que representa un set de exclusión
	 */
	typedef std::set<symbol> exclusion_set;

	/**
	 * Devuelve un set que contiene todos los chars que
	 * contiene esta distribución, sin incluir el ESC ni
	 * el EOF
	 */
	void append_to_exclusion_set(exclusion_set &exclusion) const;

	/**
	 * Crea una distribución copia de esta que toma todos los
	 * de esta menos aquellos que están incluidos en el set de
	 * exclusion dado.
	 */
	symbol_distribution exclude(const exclusion_set &exclusion) const;

	/**
	 * Representa un iterador para recorrer los símbolos
	 * registrados en una distribución
	 */
	class iterator {
	private:
		unsigned int current_position;
		const unsigned int *frequencies;
	public:
		/**
		 * Crea un nuevo iterator apuntando a la posición
		 * current_position de frequencies.
		 */
		iterator(unsigned int current_position, const unsigned int *frequencies);

		/**
		 * Devuelve el símbolo actual
		 */
		symbol operator*() const;

		/**
		 * Compara dos iteradores y devuelve true si apuntan
		 * al mismo elemento
		 */
		bool operator==(const iterator &other) const;

		/**
		 * Compara dos iteradores y devuelve false si apuntan
		 * al mismo elemento
		 */
		bool operator!=(const iterator &other) const;

		/**
		 * Apunta al siguiente elemento
		 */
		void operator++(int);
	};

	/**
	 * Devuelve un iterador apuntando al primer símbolo
	 */
	iterator begin() const;

	/**
	 * Devuelve un iterador inválido apuntando a una
	 * posición después del último símbolo
	 */
	iterator end() const;

	/**
	 * Operadores de serialización
	 */
	friend int commons::io::serialization_length<symbol_distribution>(const symbol_distribution &instance);
	friend int commons::io::serialization_length<symbol_distribution>(const commons::io::block &b, int index);
	friend void commons::io::serialize<symbol_distribution>(const symbol_distribution &instance, commons::io::block *b, int index);
	friend symbol_distribution commons::io::deserialize<symbol_distribution>(const commons::io::block &b, int index);
};

std::ostream &operator<<(std::ostream &output, const symbol_distribution &symbol);

};

// Implementación especial del serializador para symbol_distribution
namespace commons {
namespace io {

template<>
int serialization_length<arithmetic::symbol_distribution>(const arithmetic::symbol_distribution &instance);

template<>
int serialization_length<arithmetic::symbol_distribution>(const commons::io::block &b, int index);

template<>
void serialize<arithmetic::symbol_distribution>(const arithmetic::symbol_distribution &instance, commons::io::block *b, int index);

template<>
arithmetic::symbol_distribution deserialize<arithmetic::symbol_distribution>(const commons::io::block &b, int index);

};
};

#endif
