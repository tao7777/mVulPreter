main(int argc, char *argv[])
{
    OM_uint32 minor, major;
    gss_ctx_id_t context;
    gss_union_ctx_id_desc uctx;
    krb5_gss_ctx_id_rec kgctx;
    krb5_key k1, k2;
    krb5_keyblock kb1, kb2;
    gss_buffer_desc in, out;
    unsigned char k1buf[32], k2buf[32], outbuf[44];
    size_t i;

    /*
     * Fake up just enough of a krb5 GSS context to make gss_pseudo_random
     * work, with chosen subkeys and acceptor subkeys.  If we implement
     * gss_import_lucid_sec_context, we can rewrite this to use public
     * interfaces and stop using private headers and internal knowledge of the
     * implementation.
     */
    context = (gss_ctx_id_t)&uctx;
     uctx.mech_type = &mech_krb5;
     uctx.internal_ctx_id = (gss_ctx_id_t)&kgctx;
     kgctx.k5_context = NULL;
    kgctx.established = 1;
     kgctx.have_acceptor_subkey = 1;
     kb1.contents = k1buf;
     kb2.contents = k2buf;
    for (i = 0; i < sizeof(tests) / sizeof(*tests); i++) {
        /* Set up the keys for this test. */
        kb1.enctype = tests[i].enctype;
        kb1.length = fromhex(tests[i].key1, k1buf);
        check_k5err(NULL, "create_key", krb5_k_create_key(NULL, &kb1, &k1));
        kgctx.subkey = k1;
        kb2.enctype = tests[i].enctype;
        kb2.length = fromhex(tests[i].key2, k2buf);
        check_k5err(NULL, "create_key", krb5_k_create_key(NULL, &kb2, &k2));
        kgctx.acceptor_subkey = k2;

        /* Generate a PRF value with the subkey and an empty input, and compare
         * it to the first expected output. */
        in.length = 0;
        in.value = NULL;
        major = gss_pseudo_random(&minor, context, GSS_C_PRF_KEY_PARTIAL, &in,
                                  44, &out);
        check_gsserr("gss_pseudo_random", major, minor);
        (void)fromhex(tests[i].out1, outbuf);
        assert(out.length == 44 && memcmp(out.value, outbuf, 44) == 0);
        (void)gss_release_buffer(&minor, &out);

        /* Generate a PRF value with the acceptor subkey and the 61-byte input
         * string, and compare it to the second expected output. */
        in.length = strlen(inputstr);
        in.value = (char *)inputstr;
        major = gss_pseudo_random(&minor, context, GSS_C_PRF_KEY_FULL, &in, 44,
                                  &out);
        check_gsserr("gss_pseudo_random", major, minor);
        (void)fromhex(tests[i].out2, outbuf);
        assert(out.length == 44 && memcmp(out.value, outbuf, 44) == 0);
        (void)gss_release_buffer(&minor, &out);

        /* Also check that generating zero bytes of output works. */
        major = gss_pseudo_random(&minor, context, GSS_C_PRF_KEY_FULL, &in, 0,
                                  &out);
        check_gsserr("gss_pseudo_random", major, minor);
        assert(out.length == 0);
        (void)gss_release_buffer(&minor, &out);

        krb5_k_free_key(NULL, k1);
        krb5_k_free_key(NULL, k2);
    }
    return 0;
}
