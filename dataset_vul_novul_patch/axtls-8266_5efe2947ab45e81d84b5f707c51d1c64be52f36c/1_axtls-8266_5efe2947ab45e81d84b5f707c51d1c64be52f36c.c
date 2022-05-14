static const uint8_t *get_signature(const uint8_t *asn1_sig, int *len)
{
    int offset = 0;
    const uint8_t *ptr = NULL;
    if (asn1_next_obj(asn1_sig, &offset, ASN1_SEQUENCE) < 0 || 
            asn1_skip_obj(asn1_sig, &offset, ASN1_SEQUENCE))
        goto end_get_sig;
    if (asn1_sig[offset++] != ASN1_OCTET_STRING)
        goto end_get_sig;
    *len = get_asn1_length(asn1_sig, &offset);
    ptr = &asn1_sig[offset];          /* all ok */
end_get_sig:
    return ptr;
}
