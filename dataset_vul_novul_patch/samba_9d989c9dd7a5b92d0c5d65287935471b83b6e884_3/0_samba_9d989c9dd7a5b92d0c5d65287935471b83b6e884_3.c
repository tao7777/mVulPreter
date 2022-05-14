 bool asn1_write_ContextSimple(struct asn1_data *data, uint8_t num, DATA_BLOB *blob)
 {
       if (!asn1_push_tag(data, ASN1_CONTEXT_SIMPLE(num))) return false;
       if (!asn1_write(data, blob->data, blob->length)) return false;
       return asn1_pop_tag(data);
 }
