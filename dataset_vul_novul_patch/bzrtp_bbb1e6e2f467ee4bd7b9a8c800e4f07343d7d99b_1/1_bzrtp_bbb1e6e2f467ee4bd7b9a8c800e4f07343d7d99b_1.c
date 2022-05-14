int main(int argc, char *argv[] ) {
	int i, fails_count=0;
	CU_pSuite cryptoUtilsTestSuite, parserTestSuite;

	CU_pSuite *suites[] = {
		&cryptoUtilsTestSuite,
		&parserTestSuite,
		NULL
	};

	if (argc>1) {
		if (argv[1][0] == '-') {
			if (strcmp(argv[1], "-verbose") == 0) {
				verbose = 1;
			} else {
				printf ("Usage:\n %s [-verbose] to enable extensive logging\n", argv[0]);
				return 1;
			}
		} else {
			printf ("Usage:\n %s [-verbose] to enable extensive logging\n", argv[0]);
			return 1;
		}
	}
#ifdef HAVE_LIBXML2
	xmlInitParser();
#endif
	
	/* initialize the CUnit test registry */
	if (CUE_SUCCESS != CU_initialize_registry()) {
		return CU_get_error();
	}

	/* Add the cryptoUtils suite to the registry */
	cryptoUtilsTestSuite = CU_add_suite("Bzrtp Crypto Utils", NULL, NULL);
	CU_add_test(cryptoUtilsTestSuite, "zrtpKDF", test_zrtpKDF);
	CU_add_test(cryptoUtilsTestSuite, "CRC32", test_CRC32);
	CU_add_test(cryptoUtilsTestSuite, "algo agreement", test_algoAgreement);
	CU_add_test(cryptoUtilsTestSuite, "context algo setter and getter", test_algoSetterGetter);
	CU_add_test(cryptoUtilsTestSuite, "adding mandatory crypto algorithms if needed", test_addMandatoryCryptoTypesIfNeeded);

 	/* Add the parser suite to the registry */
 	parserTestSuite = CU_add_suite("Bzrtp ZRTP Packet Parser", NULL, NULL);
 	CU_add_test(parserTestSuite, "Parse", test_parser);
 	CU_add_test(parserTestSuite, "Parse Exchange", test_parserComplete);
 	CU_add_test(parserTestSuite, "State machine", test_stateMachine);
 
	/* Run all suites */
	for(i=0; suites[i]; i++){
		CU_basic_run_suite(*suites[i]);
		fails_count += CU_get_number_of_tests_failed();
	}
	
	/* cleanup the CUnit registry */
	CU_cleanup_registry();

#ifdef HAVE_LIBXML2
	/* cleanup libxml2 */
	xmlCleanupParser();
#endif

	return (fails_count == 0 ? 0 : 1);
}
