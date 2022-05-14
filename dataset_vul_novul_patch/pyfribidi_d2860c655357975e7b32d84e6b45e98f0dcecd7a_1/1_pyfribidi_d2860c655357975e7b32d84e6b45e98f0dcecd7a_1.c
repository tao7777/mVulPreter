log2vis_encoded_string (PyObject * string, const char *encoding,
			FriBidiParType base_direction, int clean, int reordernsm)
{
	PyObject *logical = NULL;	/* logical unicode object */
	PyObject *result = NULL;	/* output string object */
	/* Always needed for the string length */
	logical = PyUnicode_Decode (PyString_AS_STRING (string),
				    PyString_GET_SIZE (string),
				    encoding, "strict");
	if (logical == NULL)
		return NULL;
	if (strcmp (encoding, "utf-8") == 0)
		/* Shortcut for utf8 strings (little faster) */
		result = log2vis_utf8 (string,
				       PyUnicode_GET_SIZE (logical),
				       base_direction, clean, reordernsm);
	else
	{
		/* Invoke log2vis_unicode and encode back to encoding */
		PyObject *visual = log2vis_unicode (logical, base_direction, clean, reordernsm);
		if (visual)
		{
			result = PyUnicode_Encode (PyUnicode_AS_UNICODE
						   (visual),
						   PyUnicode_GET_SIZE (visual),
						   encoding, "strict");
			Py_DECREF (visual);
		}
	}
	Py_DECREF (logical);
	return result;
}
