 raptor_libxml_resolveEntity(void* user_data, 
                            const xmlChar *publicId, const xmlChar *systemId) {
   raptor_sax2* sax2 = (raptor_sax2*)user_data;
  return libxml2_resolveEntity(sax2->xc, publicId, systemId);
 }
