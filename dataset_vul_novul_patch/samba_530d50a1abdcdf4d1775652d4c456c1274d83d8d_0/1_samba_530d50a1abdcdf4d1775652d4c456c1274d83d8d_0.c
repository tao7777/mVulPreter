static void ldap_encode_response(struct asn1_data *data, struct ldap_Result *result)
 {
       asn1_write_enumerated(data, result->resultcode);
       asn1_write_OctetString(data, result->dn,
                              (result->dn) ? strlen(result->dn) : 0);
       asn1_write_OctetString(data, result->errormessage,
                               (result->errormessage) ?
                              strlen(result->errormessage) : 0);
        if (result->referral) {
               asn1_push_tag(data, ASN1_CONTEXT(3));
               asn1_write_OctetString(data, result->referral,
                                      strlen(result->referral));
               asn1_pop_tag(data);
        }
 }
