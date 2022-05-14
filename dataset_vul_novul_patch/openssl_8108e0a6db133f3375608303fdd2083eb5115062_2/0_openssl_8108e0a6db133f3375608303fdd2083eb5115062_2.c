ossl_cipher_pkcs5_keyivgen(int argc, VALUE *argv, VALUE self)
{
    EVP_CIPHER_CTX *ctx;
    const EVP_MD *digest;
    VALUE vpass, vsalt, viter, vdigest;
    unsigned char key[EVP_MAX_KEY_LENGTH], iv[EVP_MAX_IV_LENGTH], *salt = NULL;
    int iter;

    rb_scan_args(argc, argv, "13", &vpass, &vsalt, &viter, &vdigest);
    StringValue(vpass);
    if(!NIL_P(vsalt)){
	StringValue(vsalt);
	if(RSTRING_LEN(vsalt) != PKCS5_SALT_LEN)
	    ossl_raise(eCipherError, "salt must be an 8-octet string");
	salt = (unsigned char *)RSTRING_PTR(vsalt);
    }
    iter = NIL_P(viter) ? 2048 : NUM2INT(viter);
    digest = NIL_P(vdigest) ? EVP_md5() : GetDigestPtr(vdigest);
    GetCipher(self, ctx);
    EVP_BytesToKey(EVP_CIPHER_CTX_cipher(ctx), digest, salt,
		   (unsigned char *)RSTRING_PTR(vpass), RSTRING_LENINT(vpass), iter, key, iv);
    if (EVP_CipherInit_ex(ctx, NULL, NULL, key, iv, -1) != 1)
	ossl_raise(eCipherError, NULL);
     OPENSSL_cleanse(key, sizeof key);
     OPENSSL_cleanse(iv, sizeof iv);
 
    rb_ivar_set(self, id_key_set, Qtrue);

     return Qnil;
 }
