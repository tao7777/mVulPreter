static int asn1_d2i_ex_primitive(ASN1_VALUE **pval,
 static int asn1_template_ex_d2i(ASN1_VALUE **pval,
                                 const unsigned char **in, long len,
                                 const ASN1_TEMPLATE *tt, char opt,
                                ASN1_TLC *ctx, int depth);
 static int asn1_template_noexp_d2i(ASN1_VALUE **val,
                                    const unsigned char **in, long len,
                                    const ASN1_TEMPLATE *tt, char opt,
                                   ASN1_TLC *ctx, int depth);
 static int asn1_d2i_ex_primitive(ASN1_VALUE **pval,
                                  const unsigned char **in, long len,
                                  const ASN1_ITEM *it,
    /* tags  4- 7 */
    B_ASN1_OCTET_STRING, 0, 0, B_ASN1_UNKNOWN,
    /* tags  8-11 */
    B_ASN1_UNKNOWN, B_ASN1_UNKNOWN, B_ASN1_UNKNOWN, B_ASN1_UNKNOWN,
    /* tags 12-15 */
    B_ASN1_UTF8STRING, B_ASN1_UNKNOWN, B_ASN1_UNKNOWN, B_ASN1_UNKNOWN,
    /* tags 16-19 */
    B_ASN1_SEQUENCE, 0, B_ASN1_NUMERICSTRING, B_ASN1_PRINTABLESTRING,
    /* tags 20-22 */
    B_ASN1_T61STRING, B_ASN1_VIDEOTEXSTRING, B_ASN1_IA5STRING,
    /* tags 23-24 */
    B_ASN1_UTCTIME, B_ASN1_GENERALIZEDTIME,
    /* tags 25-27 */
    B_ASN1_GRAPHICSTRING, B_ASN1_ISO64STRING, B_ASN1_GENERALSTRING,
    /* tags 28-31 */
    B_ASN1_UNIVERSALSTRING, B_ASN1_UNKNOWN, B_ASN1_BMPSTRING, B_ASN1_UNKNOWN,
};

unsigned long ASN1_tag2bit(int tag)
{
