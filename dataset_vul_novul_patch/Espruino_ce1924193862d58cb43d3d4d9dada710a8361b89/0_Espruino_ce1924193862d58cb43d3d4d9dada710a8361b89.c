size_t jsvGetString(const JsVar *v, char *str, size_t len) {
  assert(len>0);
  const char *s = jsvGetConstString(v);
  if (s) {
    /* don't use strncpy here because we don't
      * want to pad the entire buffer with zeros */
     len--;
     int l = 0;
    while (s[l] && l<len) {
       str[l] = s[l];
       l++;
     }
    str[l] = 0;
    return l;
  } else if (jsvIsInt(v)) {
    itostr(v->varData.integer, str, 10);
    return strlen(str);
  } else if (jsvIsFloat(v)) {
    ftoa_bounded(v->varData.floating, str, len);
    return strlen(str);
  } else if (jsvHasCharacterData(v)) {
    assert(!jsvIsStringExt(v));
    size_t l = len;
    JsvStringIterator it;
    jsvStringIteratorNewConst(&it, v, 0);
    while (jsvStringIteratorHasChar(&it)) {
      if (l--<=1) {
        *str = 0;
        jsvStringIteratorFree(&it);
        return len;
      }
      *(str++) = jsvStringIteratorGetChar(&it);
      jsvStringIteratorNext(&it);
    }
    jsvStringIteratorFree(&it);
    *str = 0;
    return len-l;
  } else {
    JsVar *stringVar = jsvAsString((JsVar*)v, false); // we know we're casting to non-const here
    if (stringVar) {
      size_t l = jsvGetString(stringVar, str, len); // call again - but this time with converted var
      jsvUnLock(stringVar);
      return l;
    } else {
      str[0] = 0;
      jsExceptionHere(JSET_INTERNALERROR, "Variable type cannot be converted to string");
      return 0;
    }
  }
}
