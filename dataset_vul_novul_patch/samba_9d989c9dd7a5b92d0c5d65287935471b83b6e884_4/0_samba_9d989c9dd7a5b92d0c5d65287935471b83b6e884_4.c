 bool asn1_write_DATA_BLOB_LDAPString(struct asn1_data *data, const DATA_BLOB *s)
 {
       return asn1_write(data, s->data, s->length);
 }
