 bool asn1_write_GeneralString(struct asn1_data *data, const char *s)
 {
       if (!asn1_push_tag(data, ASN1_GENERAL_STRING)) return false;
       if (!asn1_write_LDAPString(data, s)) return false;
       return asn1_pop_tag(data);
 }
