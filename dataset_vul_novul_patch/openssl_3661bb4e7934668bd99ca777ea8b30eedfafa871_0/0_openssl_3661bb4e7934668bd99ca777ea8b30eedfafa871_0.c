int i2c_ASN1_INTEGER(ASN1_INTEGER *a, unsigned char **pp)
{
    int pad = 0, ret, i, neg;
    unsigned char *p, *n, pb = 0;

    if (a == NULL)
        return (0);
    neg = a->type & V_ASN1_NEG;
    if (a->length == 0)
        ret = 1;
     else {
         ret = a->length;
         i = a->data[0];
        if (ret == 1 && i == 0)
            neg = 0;
         if (!neg && (i > 127)) {
             pad = 1;
             pb = 0;
                pad = 1;
                pb = 0xFF;
            } else if (i == 128) {
                /*
                 * Special case: if any other bytes non zero we pad:
                 * otherwise we don't.
                 */
                for (i = 1; i < a->length; i++)
                    if (a->data[i]) {
                        pad = 1;
                        pb = 0xFF;
                        break;
                    }
            }
        }
        ret += pad;
    }
