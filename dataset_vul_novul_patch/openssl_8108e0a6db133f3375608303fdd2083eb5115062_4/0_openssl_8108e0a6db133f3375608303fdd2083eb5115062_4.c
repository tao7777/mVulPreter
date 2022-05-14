ossl_cipher_update(int argc, VALUE *argv, VALUE self)
{
    EVP_CIPHER_CTX *ctx;
    unsigned char *in;
    long in_len, out_len;
    VALUE data, str;
 
     rb_scan_args(argc, argv, "11", &data, &str);
 
    if (!RTEST(rb_attr_get(self, id_key_set)))
	ossl_raise(eCipherError, "key not set");

     StringValue(data);
     in = (unsigned char *)RSTRING_PTR(data);
     if ((in_len = RSTRING_LEN(data)) == 0)
        ossl_raise(rb_eArgError, "data must not be empty");
    GetCipher(self, ctx);
    out_len = in_len+EVP_CIPHER_CTX_block_size(ctx);
    if (out_len <= 0) {
	ossl_raise(rb_eRangeError,
		   "data too big to make output buffer: %ld bytes", in_len);
    }

    if (NIL_P(str)) {
        str = rb_str_new(0, out_len);
    } else {
        StringValue(str);
        rb_str_resize(str, out_len);
    }

    if (!ossl_cipher_update_long(ctx, (unsigned char *)RSTRING_PTR(str), &out_len, in, in_len))
	ossl_raise(eCipherError, NULL);
    assert(out_len < RSTRING_LEN(str));
    rb_str_set_len(str, out_len);

    return str;
}
