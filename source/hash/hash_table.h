/******************************************************************************
 * hash_table.h
 * 		Declaraciones de la clase hash::hash_table
******************************************************************************/
#ifndef __HASH_HASH_TABLE_H_INCLUDED__
#define __HASH_HASH_TABLE_H_INCLUDED__

#include "../commons/io/block_file.h"
#include "../commons/io/serializators.h"
#include <utility>
namespace hash {

/**
 * Mantiene una asociación entre claves y posiciones de buckets.
 * Contiene además un bloque de control que mantiene el tamaño actual
 * de la tabla.
 */
template<typename K>
class hash_table {
private:
	commons::io::block_file file;

	int last_used_position;
	commons::io::block last_used_block;
	commons::io::block control_block;

	struct block_file_initializer : public commons::io::block_file::initializer {
		virtual void initialize(commons::io::block_file *file) const;
	};

	int do_hash(const K &key) const;
	void set_size(int size);
	int get_position_in_entry(int entry);
	void set_position_in_entry(int entry, int position);
	void shrink_if_possible();
	int normalize_position(int position);
public:
	/**
	 * Crea una nueva instancia de hash_table a partir
	 * del archivo dado por filename. Crea e inicializa
	 * el archivo si este no existe, en caso contrario
	 * carga los contenidos de dicho archivo
	 */
	explicit hash_table(const char *filename);

	/**
	 * Devuelve el tamaño actual de la tabla.
	 */
	int get_size() const;

	/**
	 * Obtiene la posición asociada a la clave key.
	 */
	int get_key_position(const K &key);

	/**
	 * Obtiene la posición a la que apunta un registro
	 * dado de la tabla
	 */
	int get_table_position(int position);

	/**
	 * Duplica la tabla, creciendo al doble su tamaño y copiando
	 * los registros en la mitad superior a la mitad inferior.
	 */
	void grow();

	/**
	 * Remapea la posición de la última entrada a la que
	 * se accedio, cuyo hash factor era previous_hash_factor,
	 * para que ella y la mitad de las entradas que apuntaban al
	 * mismo bloque paraa que apunten a new_position
	 */
	void remap_last_used_entry(int new_position, int previous_hash_factor);

	/**
	 * Intenta achicar la tabla de dispersión y reapuntar todos
	 * los nodos que apuntaban a la misma posición que la última accedida
	 * de manera que la posición a la que se apuntaba pueda ser eliminada
	 * de forma segura. Después de reapuntar a un nuevo nodo, intenta achicar
	 * la tabla de dispersión para reutilizar espacio en próximas inserciones.
	 * Devuelve en result.first true si se pudo reapuntar, false en caso contrario.
	 * Si result.first es true, devuelve en result.second la nueva posición a la
	 * que se reapuntaron las entradas.
	 */
	std::pair<bool, int> try_position_erasure(int last_position_hash_factor);

	virtual ~hash_table();
};

template<typename K>
hash_table<K>::hash_table(const char *filename)
: file(filename, sizeof(int), block_file_initializer()),
  last_used_block(sizeof(int)),
  control_block(sizeof(int)) {
	file.read_block_into(0, &control_block);
	file.read_block_into(1, &last_used_block);
	last_used_position = 1;
}

template<typename K>
int hash_table<K>::get_size() const {
	return commons::io::deserialize<int>(control_block, 0);
}

template<typename K>
int hash_table<K>::get_key_position(const K &key) {
	// Calculo el hash de la clave. Eso me da el bloque en el que está
	// la posición de la clave.
	int position = do_hash(key);
	// Leo desde el archivo la posición
	last_used_position = position;
	file.read_block_into(position, &last_used_block);
	return commons::io::deserialize<int>(last_used_block, 0);
}

template<typename K>
int hash_table<K>::get_table_position(int position) {
	position++;
	last_used_position = position;
	file.read_block_into(position, &last_used_block);
	return commons::io::deserialize<int>(last_used_block, 0);
}

template<typename K>
void hash_table<K>::grow() {
	int active_block_count = get_size();
	commons::io::block temp_block(file.get_block_size());
	// Recorro todos los bloque que tengo en este momento
	for (int i = 1; i <= active_block_count; i++) {
		// Leo cada bloque
		file.read_block_into(i, &temp_block);
		// Si hay una posición disponible en el archivo
		// escribo en esa posición.
		if (i + active_block_count < file.get_block_count()) {
			file.write_block(i + active_block_count, temp_block);
		// Sino, agrego el bloque al final del archivo
		} else {
			file.append_block(temp_block);
		}
	}
	// Actualizo la información de control
	set_size(get_size() * 2);
}

template<typename K>
void hash_table<K>::remap_last_used_entry(int new_position, int previous_hash_factor) {
	// Actualizo todas las entradas en disco, salteando de a una
	int new_hash_factor = 2 * previous_hash_factor;
	for (int i = 0; i < (get_size() / new_hash_factor); i++) {
		set_position_in_entry(normalize_position((last_used_position - 1) + (i * new_hash_factor)), new_position);
	}
	// Actualizo la copia en memoria de la posición actual
	// para evitar otro acceso a disco
	commons::io::serialize(new_position, &last_used_block, 0);
}

template<typename K>
std::pair<bool, int> hash_table<K>::try_position_erasure(int last_position_hash_factor) {
	// Si la tabla tiene una sola entrada, no se puede achicar más
	if (get_size() == 1)
		return std::make_pair(false, 0);

	// Chequeo si en las posiciones medias entre las repeticiones
	// de la ultima posición está el mismo puntero
	int upper_halfway = normalize_position((last_used_position - 1) - (last_position_hash_factor / 2));
	int upper_halfway_position = get_position_in_entry(upper_halfway);
	int lower_halfway = normalize_position((last_used_position - 1) + (last_position_hash_factor / 2));
	int lower_halfway_position = get_position_in_entry(lower_halfway);

	// Si está el mismo número, cosas mágicas ocurren
	if (lower_halfway_position == upper_halfway_position) {
		// Actualizo todas las entradas que apuntaban a la misma entrada
		// a la que apuntaba la última accedida para que apunten a lo que
		// apuntan las dos mitades hacia arriba y hacia abajo
		for (int i = 0; i < get_size() / last_position_hash_factor; i++)
			set_position_in_entry((last_used_position - 1) + last_position_hash_factor * i, lower_halfway_position);

		// Intentamos achicar la tabla si las mitades superior e inferior
		// son iguales
		shrink_if_possible();

		// Como estaba el mismo número, avisamos que la posición anterior
		// ya no está apuntada por nadie, y que ahora todos los que apuntaban
		// a esa posición están apuntando a otra.
		return std::make_pair(true, lower_halfway_position);
	}

	// No estaba el mismo número, no puedo eliminar la posición
	return std::make_pair(false, 0);
}

template<typename K>
hash_table<K>::~hash_table() {
	file.write_block(0, control_block);
}

template<typename K>
void hash_table<K>::block_file_initializer::initialize(commons::io::block_file *file) const {
	commons::io::block b(file->get_block_size());

	// Guardo el bloque de control
	commons::io::serialize(1, &b, 0);
	file->append_block(b);

	// Guardo un primer bloque para que todas las claves apunten
	// a la posición cero
	commons::io::serialize(0, &b, 0);
	file->append_block(b);
}

template<typename K>
int hash_table<K>::do_hash(const K &key) const{
	return (key % get_size()) + 1;
}

template<typename K>
void hash_table<K>::set_size(int size) {
	commons::io::serialize(size, &control_block, 0);
}

template<typename K>
int hash_table<K>::get_position_in_entry(int entry) {
	commons::io::block b = file.read_block(entry + 1);
	return commons::io::deserialize<int>(b, 0);
}

template<typename K>
void hash_table<K>::set_position_in_entry(int entry, int position) {
	commons::io::block b(file.get_block_size());
	commons::io::serialize(position, &b, 0);
	file.write_block(entry + 1, b);
}

template<typename K>
void hash_table<K>::shrink_if_possible() {
	// Chequeo la mitad inferior contra la mitad superior.
	int half_size = get_size() / 2;
	for (int i = 0; i < half_size; i++) {
		// Si alguna de las posiciones no corresponde, salimos
		if (get_position_in_entry(i) != get_position_in_entry(i + half_size)) {
			return;
		}
	}

	// Si llegamos acá, todas las posiciónes de la mitad inferior eran
	// iguales a las de la mitad superior. Achicamos la tabla a la mitad
	set_size(half_size);
}

template<typename K>
int hash_table<K>::normalize_position(int position) {
	if (position < 0)
		return position + get_size();

	if (position >= get_size())
		return position - get_size();

	return position;
}

inline int operator %(const std::string &key, int max) {
     unsigned int h = 0;
     for(std::string::size_type i = 0; i < key.size(); i++) {
		 h = 31 * h + key[i];
     }
     return h % max;
}

};
#endif // __HASH_HASH_TABLE_H_INCLUDED__
