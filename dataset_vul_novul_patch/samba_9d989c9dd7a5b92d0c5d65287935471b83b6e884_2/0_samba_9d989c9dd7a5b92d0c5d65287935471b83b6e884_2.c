 bool asn1_write_BOOLEAN_context(struct asn1_data *data, bool v, int context)
 {
       if (!asn1_push_tag(data, ASN1_CONTEXT_SIMPLE(context))) return false;
       if (!asn1_write_uint8(data, v ? 0xFF : 0)) return false;
       return asn1_pop_tag(data);
 }
