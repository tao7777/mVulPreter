static void ldap_encode_response(struct asn1_data *data, struct ldap_Result *result)
static bool ldap_encode_response(struct asn1_data *data, struct ldap_Result *result)
 {
       if (!asn1_write_enumerated(data, result->resultcode)) return false;
       if (!asn1_write_OctetString(data, result->dn,
                              (result->dn) ? strlen(result->dn) : 0)) return false;
       if (!asn1_write_OctetString(data, result->errormessage,
                               (result->errormessage) ?
                              strlen(result->errormessage) : 0)) return false;
        if (result->referral) {
               if (!asn1_push_tag(data, ASN1_CONTEXT(3))) return false;
               if (!asn1_write_OctetString(data, result->referral,
                                      strlen(result->referral))) return false;
               if (!asn1_pop_tag(data)) return false;
        }
       return true;
 }
