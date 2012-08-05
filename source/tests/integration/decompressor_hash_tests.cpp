#include "../../dependencies/tut/tut.hpp"
#include "../../ppmc/decompressor.h"
#include "../../ppmc/compressor.h"
#include "../../hash/hash_container.h"
#include "../../commons/io/stream_binary_source.h"
#include "../../commons/io/stream_binary_destination.h"
#include "../../commons/io/stream_char_destination.h"
#include "../../commons/io/stream_char_source.h"
#include "../../commons/utils/bit_utils.h"
#include <sstream>

using namespace ppmc;

namespace {

struct test_data {
	typedef hash::hash_container<std::string, arithmetic::symbol_distribution> context_container;

	std::stringstream compressed_stream;
	std::stringstream uncompressed_stream;
	int max_contexts;

	void require_max_contexts(int amount) {
		max_contexts = amount;
	}

	void prepare_compressed_data(const std::string &what) {
		std::stringstream original_stream(what);
		commons::io::stream_char_source original_source(original_stream);

		context_container *compression_container = new context_container("test_ppmch_decompresor_tmp.data", "test_ppmch_decompresor_tmp.index", 8192);

		commons::io::stream_binary_destination compression_destination(compressed_stream);
		compressor c(compression_destination, max_contexts, compression_container);
		c.compress(original_source);

		compressed_stream.seekg(0);

		delete compression_container;
		std::remove("test_ppmch_decompresor_tmp.data");
		std::remove("test_ppmch_decompresor_tmp.index");
	}

	void decompress_compressed_data() {
		commons::io::stream_binary_source source(compressed_stream);
		context_container container("test_ppmch_decompressor.data", "test_ppmch_decompressor.index", 8192);
		decompressor d(source, max_contexts, &container);

		std::stringstream result_stream;
		{
			commons::io::stream_char_destination expected_destination(uncompressed_stream);
			d.decompress(expected_destination);
		}
	}

	~test_data() {
		std::remove("test_ppmch_decompressor.data");
		std::remove("test_ppmch_decompressor.index");
	}

};

tut::test_group<test_data> test_group("integration::ppmc::decompressor::hash class unit tests");

};

#define TEST_COMPRESS_DECOMPRESS(WHAT) prepare_compressed_data(WHAT); decompress_compressed_data(); ensure_equals(uncompressed_stream.str(), WHAT)

namespace tut {

template<>
template<>
void test_group<test_data>::object::test<5>() {
	std::string quite_large_text =
			"Lorem Ipsum is simply dummy text of the printing and typesetting industry."
			"Lorem Ipsum has been the industry's standard dummy text ever since the 1500s, "
			"when an unknown printer took a galley of type and scrambled it to make a type "
			"specimen book. It has survived not only five centuries, but also the leap into "
			"electronic typesetting, remaining essentially unchanged. It was popularised in "
			"the 1960s with the release of Letraset sheets containing Lorem Ipsum passages, "
			"and more recently with desktop publishing software like Aldus PageMaker "
			"including versions of Lorem Ipsum."
			"Lorem Ipsum is simply dummy text of the printing and typesetting industry."
			"Lorem Ipsum has been the industry's standard dummy text ever since the 1500s, "
			"when an unknown printer took a galley of type and scrambled it to make a type "
			"specimen book. It has survived not only five centuries, but also the leap into "
			"electronic typesetting, remaining essentially unchanged. It was popularised in "
			"the 1960s with the release of Letraset sheets containing Lorem Ipsum passages, "
			"and more recently with desktop publishing software like Aldus PageMaker "
			"Lorem Ipsum is simply dummy text of the printing and typesetting industry."
			"Lorem Ipsum has been the industry's standard dummy text ever since the 1500s, "
			"when an unknown printer took a galley of type and scrambled it to make a type "
			"specimen book. It has survived not only five centuries, but also the leap into "
			"electronic typesetting, remaining essentially unchanged. It was popularised in "
			"the 1960s with the release of Letraset sheets containing Lorem Ipsum passages, "
			"and more recently with desktop publishing software like Aldus PageMaker "
			"including versions of Lorem Ipsum."
			"Lorem Ipsum is simply dummy text of the printing and typesetting industry."
			"Lorem Ipsum has been the industry's standard dummy text ever since the 1500s, "
			"when an unknown printer took a galley of type and scrambled it to make a type "
			"specimen book. It has survived not only five centuries, but also the leap into "
			"electronic typesetting, remaining essentially unchanged. It was popularised in "
			"the 1960s with the release of Letraset sheets containing Lorem Ipsum passages, "
			"and more recently with desktop publishing software like Aldus PageMaker "
			"Lorem Ipsum is simply dummy text of the printing and typesetting industry."
			"Lorem Ipsum has been the industry's standard dummy text ever since the 1500s, "
			"when an unknown printer took a galley of type and scrambled it to make a type "
			"specimen book. It has survived not only five centuries, but also the leap into "
			"electronic typesetting, remaining essentially unchanged. It was popularised in "
			"the 1960s with the release of Letraset sheets containing Lorem Ipsum passages, "
			"and more recently with desktop publishing software like Aldus PageMaker "
			"including versions of Lorem Ipsum."
			"Lorem Ipsum is simply dummy text of the printing and typesetting industry."
			"Lorem Ipsum has been the industry's standard dummy text ever since the 1500s, "
			"when an unknown printer took a galley of type and scrambled it to make a type "
			"specimen book. It has survived not only five centuries, but also the leap into "
			"electronic typesetting, remaining essentially unchanged. It was popularised in "
			"the 1960s with the release of Letraset sheets containing Lorem Ipsum passages, "
			"and more recently with desktop publishing software like Aldus PageMaker ";

	require_max_contexts(3);

	TEST_COMPRESS_DECOMPRESS(quite_large_text);
}

};
