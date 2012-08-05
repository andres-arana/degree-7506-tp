template<>
template<>
void test_group<test_data>::object::test<1>() {
	require_max_contexts(3);
	TEST_COMPRESS_DECOMPRESS("casa");
}

template<>
template<>
void test_group<test_data>::object::test<2>() {
	require_max_contexts(3);
	TEST_COMPRESS_DECOMPRESS("algo mas para comprimir");
}

template<>
template<>
void test_group<test_data>::object::test<3>() {
	require_max_contexts(3);
	TEST_COMPRESS_DECOMPRESS("tapatapitatapon");
}

template<>
template<>
void test_group<test_data>::object::test<4>() {
	std::string quite_large_text =
			"Lorem Ipsum is";

	require_max_contexts(3);

	TEST_COMPRESS_DECOMPRESS(quite_large_text);
}
