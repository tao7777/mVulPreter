ossl_cipher_init(int argc, VALUE *argv, VALUE self, int mode)
{
    EVP_CIPHER_CTX *ctx;
    unsigned char key[EVP_MAX_KEY_LENGTH], *p_key = NULL;
    unsigned char iv[EVP_MAX_IV_LENGTH], *p_iv = NULL;
    VALUE pass, init_v;

    if(rb_scan_args(argc, argv, "02", &pass, &init_v) > 0){
	/*
	 * oops. this code mistakes salt for IV.
	 * We deprecated the arguments for this method, but we decided
	 * keeping this behaviour for backward compatibility.
	 */
	VALUE cname  = rb_class_path(rb_obj_class(self));
	rb_warn("arguments for %"PRIsVALUE"#encrypt and %"PRIsVALUE"#decrypt were deprecated; "
                "use %"PRIsVALUE"#pkcs5_keyivgen to derive key and IV",
                cname, cname, cname);
	StringValue(pass);
	GetCipher(self, ctx);
	if (NIL_P(init_v)) memcpy(iv, "OpenSSL for Ruby rulez!", sizeof(iv));
	else{
	    StringValue(init_v);
	    if (EVP_MAX_IV_LENGTH > RSTRING_LEN(init_v)) {
		memset(iv, 0, EVP_MAX_IV_LENGTH);
		memcpy(iv, RSTRING_PTR(init_v), RSTRING_LEN(init_v));
	    }
	    else memcpy(iv, RSTRING_PTR(init_v), sizeof(iv));
	}
	EVP_BytesToKey(EVP_CIPHER_CTX_cipher(ctx), EVP_md5(), iv,
		       (unsigned char *)RSTRING_PTR(pass), RSTRING_LENINT(pass), 1, key, NULL);
	p_key = key;
	p_iv = iv;
    }
    else {
	GetCipher(self, ctx);
    }
    if (EVP_CipherInit_ex(ctx, NULL, NULL, p_key, p_iv, mode) != 1) {
 	ossl_raise(eCipherError, NULL);
     }
 
     return self;
 }
