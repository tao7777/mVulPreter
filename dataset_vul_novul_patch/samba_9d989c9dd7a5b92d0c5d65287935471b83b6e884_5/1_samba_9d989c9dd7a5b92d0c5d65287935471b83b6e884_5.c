 bool asn1_write_GeneralString(struct asn1_data *data, const char *s)
 {
       asn1_push_tag(data, ASN1_GENERAL_STRING);
       asn1_write_LDAPString(data, s);
       asn1_pop_tag(data);
       return !data->has_error;
 }
