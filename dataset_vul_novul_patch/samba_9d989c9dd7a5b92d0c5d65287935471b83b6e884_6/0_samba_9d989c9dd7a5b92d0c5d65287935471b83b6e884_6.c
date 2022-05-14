 bool asn1_write_LDAPString(struct asn1_data *data, const char *s)
 {
       return asn1_write(data, s, strlen(s));
 }
