raptor_libxml_getEntity(void* user_data, const xmlChar *name) {
   raptor_sax2* sax2 = (raptor_sax2*)user_data;
  return libxml2_getEntity(sax2->xc, name);
 }
