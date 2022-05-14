 bool asn1_write_OctetString(struct asn1_data *data, const void *p, size_t length)
 {
       if (!asn1_push_tag(data, ASN1_OCTET_STRING)) return false;
       if (!asn1_write(data, p, length)) return false;
       return asn1_pop_tag(data);
 }
