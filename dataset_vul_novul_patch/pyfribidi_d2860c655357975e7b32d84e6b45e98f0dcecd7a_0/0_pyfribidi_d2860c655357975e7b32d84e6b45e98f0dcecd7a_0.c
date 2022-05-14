initpyfribidi (void)
init_pyfribidi (void)
 {
        PyObject *module = Py_InitModule ("_pyfribidi", PyfribidiMethods);
 	PyModule_AddIntConstant (module, "RTL", (long) FRIBIDI_TYPE_RTL);
 	PyModule_AddIntConstant (module, "LTR", (long) FRIBIDI_TYPE_LTR);
 	PyModule_AddIntConstant (module, "ON", (long) FRIBIDI_TYPE_ON);
 }
