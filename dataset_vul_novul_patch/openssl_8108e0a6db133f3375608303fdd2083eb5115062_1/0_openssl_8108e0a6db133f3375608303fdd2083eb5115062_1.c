ossl_cipher_initialize(VALUE self, VALUE str)
{
     EVP_CIPHER_CTX *ctx;
     const EVP_CIPHER *cipher;
     char *name;
 
     name = StringValueCStr(str);
     GetCipherInit(self, ctx);
    if (ctx) {
	ossl_raise(rb_eRuntimeError, "Cipher already inititalized!");
    }
    AllocCipher(self, ctx);
     if (!(cipher = EVP_get_cipherbyname(name))) {
 	ossl_raise(rb_eRuntimeError, "unsupported cipher algorithm (%"PRIsVALUE")", str);
     }
    if (EVP_CipherInit_ex(ctx, cipher, NULL, NULL, NULL, -1) != 1)
 	ossl_raise(eCipherError, NULL);
 
     return self;
}
