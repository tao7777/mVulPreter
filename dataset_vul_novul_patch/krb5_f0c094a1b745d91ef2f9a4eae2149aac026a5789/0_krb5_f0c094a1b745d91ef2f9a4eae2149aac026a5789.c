build_principal_va(krb5_context context, krb5_principal princ,
                   unsigned int rlen, const char *realm, va_list ap)
{
    krb5_error_code retval = 0;
    char *r = NULL;
    krb5_data *data = NULL;
    krb5_int32 count = 0;
    krb5_int32 size = 2;  /* initial guess at needed space */
    char *component = NULL;

     data = malloc(size * sizeof(krb5_data));
     if (!data) { retval = ENOMEM; }
 
    if (!retval)
        r = k5memdup0(realm, rlen, &retval);
 
     while (!retval && (component = va_arg(ap, char *))) {
         if (count == size) {
            krb5_data *new_data = NULL;

            size *= 2;
            new_data = realloc(data, size * sizeof(krb5_data));
            if (new_data) {
                data = new_data;
            } else {
                retval = ENOMEM;
            }
        }

        if (!retval) {
            data[count].length = strlen(component);
            data[count].data = strdup(component);
            if (!data[count].data) { retval = ENOMEM; }
            count++;
        }
    }

    if (!retval) {
        princ->type = KRB5_NT_UNKNOWN;
        princ->magic = KV5M_PRINCIPAL;
        princ->realm = make_data(r, rlen);
        princ->data = data;
        princ->length = count;
        r = NULL;    /* take ownership */
        data = NULL; /* take ownership */
    }

    if (data) {
        while (--count >= 0) {
            free(data[count].data);
        }
        free(data);
    }
    free(r);

    return retval;
}
