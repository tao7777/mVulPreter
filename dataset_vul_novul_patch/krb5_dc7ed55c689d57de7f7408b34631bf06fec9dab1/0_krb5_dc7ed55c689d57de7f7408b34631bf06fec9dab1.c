krb5_encode_krbsecretkey(krb5_key_data *key_data_in, int n_key_data,
                         krb5_kvno mkvno) {
    struct berval **ret = NULL;
    int currkvno;
    int num_versions = 1;
    int i, j, last;
    krb5_error_code err = 0;
    krb5_key_data *key_data;

    if (n_key_data <= 0)
        return NULL;

    /* Make a shallow copy of the key data so we can alter it. */
    key_data = k5calloc(n_key_data, sizeof(*key_data), &err);
    if (key_data_in == NULL)
        goto cleanup;
    memcpy(key_data, key_data_in, n_key_data * sizeof(*key_data));

    /* Unpatched krb5 1.11 and 1.12 cannot decode KrbKey sequences with no salt
     * field.  For compatibility, always encode a salt field. */
    for (i = 0; i < n_key_data; i++) {
        if (key_data[i].key_data_ver == 1) {
            key_data[i].key_data_ver = 2;
            key_data[i].key_data_type[1] = KRB5_KDB_SALTTYPE_NORMAL;
            key_data[i].key_data_length[1] = 0;
            key_data[i].key_data_contents[1] = NULL;
        }
    }

    /* Find the number of key versions */
    for (i = 0; i < n_key_data - 1; i++)
        if (key_data[i].key_data_kvno != key_data[i + 1].key_data_kvno)
            num_versions++;

    ret = (struct berval **) calloc (num_versions + 1, sizeof (struct berval *));
    if (ret == NULL) {
        err = ENOMEM;
        goto cleanup;
    }
    for (i = 0, last = 0, j = 0, currkvno = key_data[0].key_data_kvno; i < n_key_data; i++) {
        krb5_data *code;
        if (i == n_key_data - 1 || key_data[i + 1].key_data_kvno != currkvno) {
            ret[j] = k5alloc(sizeof(struct berval), &err);
            if (ret[j] == NULL)
                goto cleanup;
            err = asn1_encode_sequence_of_keys(key_data + last,
                                               (krb5_int16)i - last + 1,
                                               mkvno, &code);
            if (err)
                goto cleanup;
            /*CHECK_NULL(ret[j]); */
            ret[j]->bv_len = code->length;
            ret[j]->bv_val = code->data;
            free(code);
             j++;
             last = i + 1;
 
            if (i < n_key_data - 1)
                currkvno = key_data[i + 1].key_data_kvno;
         }
     }
     ret[num_versions] = NULL;

cleanup:

    free(key_data);
    if (err != 0) {
        if (ret != NULL) {
            for (i = 0; i <= num_versions; i++)
                if (ret[i] != NULL)
                    free (ret[i]);
            free (ret);
            ret = NULL;
        }
    }

    return ret;
}
