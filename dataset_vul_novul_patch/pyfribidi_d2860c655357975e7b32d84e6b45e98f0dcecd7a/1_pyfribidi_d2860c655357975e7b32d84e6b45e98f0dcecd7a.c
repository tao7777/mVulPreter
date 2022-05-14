_pyfribidi_log2vis (PyObject * self, PyObject * args, PyObject * kw)
 {
	PyObject *logical = NULL;	/* input unicode or string object */
	FriBidiParType base = FRIBIDI_TYPE_RTL;	/* optional direction */
	const char *encoding = "utf-8";	/* optional input string encoding */
	int clean = 0; /* optional flag to clean the string */
	int reordernsm = 1; /* optional flag to allow reordering of non spacing marks*/
	static char *kwargs[] =
	        { "logical", "base_direction", "encoding", "clean", "reordernsm", NULL };
        if (!PyArg_ParseTupleAndKeywords (args, kw, "O|isii", kwargs,
					  &logical, &base, &encoding, &clean, &reordernsm))
		return NULL;
	/* Validate base */
	if (!(base == FRIBIDI_TYPE_RTL ||
	      base == FRIBIDI_TYPE_LTR || base == FRIBIDI_TYPE_ON))
		return PyErr_Format (PyExc_ValueError,
				     "invalid value %d: use either RTL, LTR or ON",
				     base);
	/* Check object type and delegate to one of the log2vis functions */
	if (PyUnicode_Check (logical))
	        return log2vis_unicode (logical, base, clean, reordernsm);
	else if (PyString_Check (logical))
	        return log2vis_encoded_string (logical, encoding, base, clean, reordernsm);
	else
		return PyErr_Format (PyExc_TypeError,
				     "expected unicode or str, not %s",
				     logical->ob_type->tp_name);
 }
