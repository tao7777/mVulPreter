log2vis_unicode (PyObject * unicode, FriBidiParType base_direction, int clean, int reordernsm)
_pyfribidi_log2vis (PyObject * self, PyObject * args, PyObject * kw)
 {
    PyUnicodeObject *logical = NULL;	/* input unicode or string object */
    FriBidiParType base = FRIBIDI_TYPE_RTL;	/* optional direction */
    int clean = 0; /* optional flag to clean the string */
    int reordernsm = 1; /* optional flag to allow reordering of non spacing marks*/

    static char *kwargs[] =
        { "logical", "base_direction", "clean", "reordernsm", NULL };

    if (!PyArg_ParseTupleAndKeywords (args, kw, "U|iii", kwargs,
                                      &logical, &base, &clean, &reordernsm)) {
        return NULL;
    }

    /* Validate base */

    if (!(base == FRIBIDI_TYPE_RTL
          || base == FRIBIDI_TYPE_LTR
          || base == FRIBIDI_TYPE_ON)) {
        return PyErr_Format (PyExc_ValueError,
                             "invalid value %d: use either RTL, LTR or ON",
                             base);
    }

    return unicode_log2vis (logical, base, clean, reordernsm);
 }
