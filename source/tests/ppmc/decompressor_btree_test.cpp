#include "../../dependencies/tut/tut.hpp"
#include "../../ppmc/decompressor.h"
#include "../../ppmc/compressor.h"
#include "../../bplus/bplus_container.h"
#include "../../commons/io/stream_binary_source.h"
#include "../../commons/io/stream_binary_destination.h"
#include "../../commons/io/stream_char_destination.h"
#include "../../commons/io/stream_char_source.h"
#include "../../commons/utils/bit_utils.h"
#include <sstream>

using namespace ppmc;

namespace {

struct test_data {
	typedef bplus::bplus_container<std::string, arithmetic::symbol_distribution> context_container;

	std::stringstream compressed_stream;
	std::stringstream uncompressed_stream;
	int max_contexts;

	void require_max_contexts(int amount) {
		max_contexts = amount;
	}

	void prepare_compressed_data(const std::string &what) {
		std::stringstream original_stream(what);
		commons::io::stream_char_source original_source(original_stream);

		context_container *compression_container = new context_container("test_ppmct_decompresor_tmp.data", 8192);

		commons::io::stream_binary_destination compression_destination(compressed_stream);
		compressor c(compression_destination, max_contexts, compression_container);
		c.compress(original_source);

		compressed_stream.seekg(0);

		delete compression_container;
		std::remove("test_ppmct_decompresor_tmp.data");
		std::remove("test_ppmct_decompresor_tmp.index");
	}

	void decompress_compressed_data() {
		commons::io::stream_binary_source source(compressed_stream);
		context_container container("test_ppmct_decompressor.data", 8192);
		decompressor d(source, max_contexts, &container);

		std::stringstream result_stream;
		{
			commons::io::stream_char_destination expected_destination(uncompressed_stream);
			d.decompress(expected_destination);
		}
	}

	~test_data() {
		std::remove("test_ppmct_decompressor.data");
	}

};

tut::test_group<test_data> test_group("ppmc::decompressor::tree class unit tests");

};

#define TEST_COMPRESS_DECOMPRESS(WHAT) prepare_compressed_data(WHAT); decompress_compressed_data(); ensure_equals(uncompressed_stream.str(), WHAT)

namespace tut {

#include "decompressor_test_suite.h"

};
