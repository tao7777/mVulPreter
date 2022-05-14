 bool asn1_write_BOOLEAN_context(struct asn1_data *data, bool v, int context)
 {
       asn1_push_tag(data, ASN1_CONTEXT_SIMPLE(context));
       asn1_write_uint8(data, v ? 0xFF : 0);
       asn1_pop_tag(data);
       return !data->has_error;
 }
