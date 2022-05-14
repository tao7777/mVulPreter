_pyfribidi_log2vis (PyObject * self, PyObject * args, PyObject * kw)
unicode_log2vis (PyUnicodeObject* string,
                 FriBidiParType base_direction, int clean, int reordernsm)
 {
    int i;
    int length = string->length;
    FriBidiChar *logical = NULL; /* input fribidi unicode buffer */
    FriBidiChar *visual = NULL;      /* output fribidi unicode buffer */
    FriBidiStrIndex new_len = 0; /* length of the UTF-8 buffer */
    PyUnicodeObject *result = NULL;

    /* Allocate fribidi unicode buffers
       TODO - Don't copy strings if sizeof(FriBidiChar) == sizeof(Py_UNICODE)
    */

    logical = PyMem_New (FriBidiChar, length + 1);
    if (logical == NULL) {
        PyErr_NoMemory();
        goto cleanup;
    }

    visual = PyMem_New (FriBidiChar, length + 1);
    if (visual == NULL) {
        PyErr_NoMemory();
        goto cleanup;
    }

    for (i=0; i<length; ++i) {
        logical[i] = string->str[i];
    }

    /* Convert to unicode and order visually */
    fribidi_set_reorder_nsm(reordernsm);

    if (!fribidi_log2vis (logical, length, &base_direction, visual,
                          NULL, NULL, NULL)) {

        PyErr_SetString (PyExc_RuntimeError,
                         "fribidi failed to order string");
        goto cleanup;
    }

    /* Cleanup the string if requested */
    if (clean) {
        length = fribidi_remove_bidi_marks (visual, length, NULL, NULL, NULL);
    }

    result = (PyUnicodeObject*) PyUnicode_FromUnicode(NULL, length);
    if (result == NULL) {
        goto cleanup;
    }

    for (i=0; i<length; ++i) {
        result->str[i] = visual[i];
    }

  cleanup:
    /* Delete unicode buffers */
    PyMem_Del (logical);
    PyMem_Del (visual);

    return (PyObject *)result;
 }
