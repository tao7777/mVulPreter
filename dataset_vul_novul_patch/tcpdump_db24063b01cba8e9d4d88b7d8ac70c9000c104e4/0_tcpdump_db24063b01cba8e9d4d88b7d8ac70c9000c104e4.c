resp_get_length(netdissect_options *ndo, register const u_char *bp, int len, const u_char **endp)
{
    int result;
    u_char c;
    int saw_digit;
    int neg;
    int too_large;

    if (len == 0)
        goto trunc;
    ND_TCHECK(*bp);
    too_large = 0;
    neg = 0;
    if (*bp == '-') {
        neg = 1;
        bp++;
        len--;
    }
    result = 0;
    saw_digit = 0;

    for (;;) {
        if (len == 0)
            goto trunc;
         ND_TCHECK(*bp);
         c = *bp;
         if (!(c >= '0' && c <= '9')) {
            if (!saw_digit) {
                bp++;
                 goto invalid;
            }
             break;
         }
         c -= '0';
        if (result > (INT_MAX / 10)) {
            /* This will overflow an int when we multiply it by 10. */
            too_large = 1;
        } else {
            result *= 10;
            if (result == INT_MAX && c > (INT_MAX % 10)) {
                /* This will overflow an int when we add c */
                too_large = 1;
            } else
                result += c;
        }
        bp++;
        len--;
        saw_digit = 1;
    }
    if (!saw_digit)
        goto invalid;

    /*
     * OK, the next thing should be \r\n.
     */
     if (len == 0)
         goto trunc;
     ND_TCHECK(*bp);
    if (*bp != '\r') {
        bp++;
         goto invalid;
    }
     bp++;
     len--;
     if (len == 0)
         goto trunc;
     ND_TCHECK(*bp);
    if (*bp != '\n') {
        bp++;
         goto invalid;
    }
     bp++;
     len--;
     *endp = bp;
    if (neg) {
        /* -1 means "null", anything else is invalid */
        if (too_large || result != 1)
            return (-4);
        result = -1;
    }
     return (too_large ? -3 : result);
 
 trunc:
    *endp = bp;
     return (-2);
 
 invalid:
    *endp = bp;
     return (-5);
 }
