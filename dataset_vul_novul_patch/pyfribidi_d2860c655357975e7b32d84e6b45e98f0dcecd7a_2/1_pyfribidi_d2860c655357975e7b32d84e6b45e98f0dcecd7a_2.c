log2vis_unicode (PyObject * unicode, FriBidiParType base_direction, int clean, int reordernsm)
 {
	PyObject *logical = NULL;	/* input string encoded in utf-8 */
	PyObject *visual = NULL;	/* output string encoded in utf-8 */
	PyObject *result = NULL;	/* unicode output string */
	int length = PyUnicode_GET_SIZE (unicode);
	logical = PyUnicode_AsUTF8String (unicode);
	if (logical == NULL)
		goto cleanup;
	visual = log2vis_utf8 (logical, length, base_direction, clean, reordernsm);
	if (visual == NULL)
		goto cleanup;
	result = PyUnicode_DecodeUTF8 (PyString_AS_STRING (visual),
				       PyString_GET_SIZE (visual), "strict");
      cleanup:
	Py_XDECREF (logical);
	Py_XDECREF (visual);
	return result;
 }
