 bool asn1_write_BOOLEAN(struct asn1_data *data, bool v)
 {
       if (!asn1_push_tag(data, ASN1_BOOLEAN)) return false;
       if (!asn1_write_uint8(data, v ? 0xFF : 0)) return false;
       return asn1_pop_tag(data);
 }
