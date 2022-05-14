log2vis_utf8 (PyObject * string, int unicode_length,
	      FriBidiParType base_direction, int clean, int reordernsm)
{
	FriBidiChar *logical = NULL; /* input fribidi unicode buffer */
	FriBidiChar *visual = NULL;	 /* output fribidi unicode buffer */
	char *visual_utf8 = NULL;    /* output fribidi UTF-8 buffer */
	FriBidiStrIndex new_len = 0; /* length of the UTF-8 buffer */
	PyObject *result = NULL;	 /* failure */
	/* Allocate fribidi unicode buffers */
	logical = PyMem_New (FriBidiChar, unicode_length + 1);
	if (logical == NULL)
	{
		PyErr_SetString (PyExc_MemoryError,
				 "failed to allocate unicode buffer");
		goto cleanup;
	}
	visual = PyMem_New (FriBidiChar, unicode_length + 1);
	if (visual == NULL)
	{
		PyErr_SetString (PyExc_MemoryError,
				 "failed to allocate unicode buffer");
		goto cleanup;
	}
	/* Convert to unicode and order visually */
	fribidi_set_reorder_nsm(reordernsm);
	fribidi_utf8_to_unicode (PyString_AS_STRING (string),
				 PyString_GET_SIZE (string), logical);
	if (!fribidi_log2vis (logical, unicode_length, &base_direction, visual,
			      NULL, NULL, NULL))
	{
		PyErr_SetString (PyExc_RuntimeError,
				 "fribidi failed to order string");
		goto cleanup;
	}
	/* Cleanup the string if requested */
	if (clean)
		fribidi_remove_bidi_marks (visual, unicode_length, NULL, NULL, NULL);
	/* Allocate fribidi UTF-8 buffer */
	visual_utf8 = PyMem_New(char, (unicode_length * 4)+1);
	if (visual_utf8 == NULL)
	{
		PyErr_SetString (PyExc_MemoryError,
				"failed to allocate UTF-8 buffer");
		goto cleanup;
	}
	/* Encode the reordered string  and create result string */
	new_len = fribidi_unicode_to_utf8 (visual, unicode_length, visual_utf8);
	result = PyString_FromStringAndSize (visual_utf8, new_len);
	if (result == NULL)
		/* XXX does it raise any error? */
		goto cleanup;
      cleanup:
	/* Delete unicode buffers */
	PyMem_Del (logical);
	PyMem_Del (visual);
	PyMem_Del (visual_utf8);
	return result;
}
