 bool asn1_read_BOOLEAN(struct asn1_data *data, bool *v)
 {
        uint8_t tmp = 0;
       asn1_start_tag(data, ASN1_BOOLEAN);
       asn1_read_uint8(data, &tmp);
        if (tmp == 0xFF) {
                *v = true;
       } else {
               *v = false;
        }
       asn1_end_tag(data);
       return !data->has_error;
 }
