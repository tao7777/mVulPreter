initpyfribidi (void)
 {
	PyObject *module;
	/* XXX What should be done if we fail here? */
	module = Py_InitModule3 ("pyfribidi", PyfribidiMethods,
				 _pyfribidi__doc__);
 	PyModule_AddIntConstant (module, "RTL", (long) FRIBIDI_TYPE_RTL);
 	PyModule_AddIntConstant (module, "LTR", (long) FRIBIDI_TYPE_LTR);
 	PyModule_AddIntConstant (module, "ON", (long) FRIBIDI_TYPE_ON);
	PyModule_AddStringConstant (module, "__author__",
				    "Yaacov Zamir and Nir Soffer");
 }
