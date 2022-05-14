 bool asn1_write_OctetString(struct asn1_data *data, const void *p, size_t length)
 {
       asn1_push_tag(data, ASN1_OCTET_STRING);
       asn1_write(data, p, length);
       asn1_pop_tag(data);
       return !data->has_error;
 }
