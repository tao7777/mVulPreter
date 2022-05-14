 bool asn1_read_BOOLEAN(struct asn1_data *data, bool *v)
 {
        uint8_t tmp = 0;
       if (!asn1_start_tag(data, ASN1_BOOLEAN)) return false;
       *v = false;
       if (!asn1_read_uint8(data, &tmp)) return false;
        if (tmp == 0xFF) {
                *v = true;
        }
       return asn1_end_tag(data);
 }
