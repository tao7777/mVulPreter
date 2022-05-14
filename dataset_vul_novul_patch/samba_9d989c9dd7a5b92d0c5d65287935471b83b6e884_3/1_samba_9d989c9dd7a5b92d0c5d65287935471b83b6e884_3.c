 bool asn1_write_ContextSimple(struct asn1_data *data, uint8_t num, DATA_BLOB *blob)
 {
       asn1_push_tag(data, ASN1_CONTEXT_SIMPLE(num));
       asn1_write(data, blob->data, blob->length);
       asn1_pop_tag(data);
       return !data->has_error;
 }
