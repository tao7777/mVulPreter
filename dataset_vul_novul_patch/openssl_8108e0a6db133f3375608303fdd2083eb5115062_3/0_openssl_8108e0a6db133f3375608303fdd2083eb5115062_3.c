ossl_cipher_set_key(VALUE self, VALUE key)
{
    EVP_CIPHER_CTX *ctx;
    int key_len;

    StringValue(key);
    GetCipher(self, ctx);

    key_len = EVP_CIPHER_CTX_key_length(ctx);
    if (RSTRING_LEN(key) != key_len)
	ossl_raise(rb_eArgError, "key must be %d bytes", key_len);

     if (EVP_CipherInit_ex(ctx, NULL, NULL, (unsigned char *)RSTRING_PTR(key), NULL, -1) != 1)
 	ossl_raise(eCipherError, NULL);
 
    rb_ivar_set(self, id_key_set, Qtrue);

     return key;
 }
