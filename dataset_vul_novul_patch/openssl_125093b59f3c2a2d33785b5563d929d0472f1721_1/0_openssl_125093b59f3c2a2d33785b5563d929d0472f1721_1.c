int SSL_library_init(void)
	{

#ifndef OPENSSL_NO_DES
	EVP_add_cipher(EVP_des_cbc());
	EVP_add_cipher(EVP_des_ede3_cbc());
#endif
#ifndef OPENSSL_NO_IDEA
	EVP_add_cipher(EVP_idea_cbc());
#endif
#ifndef OPENSSL_NO_RC4
	EVP_add_cipher(EVP_rc4());
#if !defined(OPENSSL_NO_MD5) && (defined(__x86_64) || defined(__x86_64__))
	EVP_add_cipher(EVP_rc4_hmac_md5());
#endif
#endif  
#ifndef OPENSSL_NO_RC2
	EVP_add_cipher(EVP_rc2_cbc());
	/* Not actually used for SSL/TLS but this makes PKCS#12 work
	 * if an application only calls SSL_library_init().
	 */
	EVP_add_cipher(EVP_rc2_40_cbc());
#endif
#ifndef OPENSSL_NO_AES
	EVP_add_cipher(EVP_aes_128_cbc());
	EVP_add_cipher(EVP_aes_192_cbc());
        EVP_add_cipher(EVP_aes_256_cbc());
        EVP_add_cipher(EVP_aes_128_gcm());
        EVP_add_cipher(EVP_aes_256_gcm());
 #if !defined(OPENSSL_NO_SHA) && !defined(OPENSSL_NO_SHA1)
        EVP_add_cipher(EVP_aes_128_cbc_hmac_sha1());
        EVP_add_cipher(EVP_aes_256_cbc_hmac_sha1());
 #endif
 
 #endif
 #ifndef OPENSSL_NO_CAMELLIA
#endif
#ifndef OPENSSL_NO_CAMELLIA
	EVP_add_cipher(EVP_camellia_128_cbc());
	EVP_add_cipher(EVP_camellia_256_cbc());
#endif

#ifndef OPENSSL_NO_SEED
	EVP_add_cipher(EVP_seed_cbc());
#endif
  
#ifndef OPENSSL_NO_MD5
	EVP_add_digest(EVP_md5());
	EVP_add_digest_alias(SN_md5,"ssl2-md5");
	EVP_add_digest_alias(SN_md5,"ssl3-md5");
#endif
#ifndef OPENSSL_NO_SHA
	EVP_add_digest(EVP_sha1()); /* RSA with sha1 */
	EVP_add_digest_alias(SN_sha1,"ssl3-sha1");
	EVP_add_digest_alias(SN_sha1WithRSAEncryption,SN_sha1WithRSA);
#endif
#ifndef OPENSSL_NO_SHA256
	EVP_add_digest(EVP_sha224());
	EVP_add_digest(EVP_sha256());
#endif
#ifndef OPENSSL_NO_SHA512
	EVP_add_digest(EVP_sha384());
	EVP_add_digest(EVP_sha512());
#endif
#if !defined(OPENSSL_NO_SHA) && !defined(OPENSSL_NO_DSA)
	EVP_add_digest(EVP_dss1()); /* DSA with sha1 */
	EVP_add_digest_alias(SN_dsaWithSHA1,SN_dsaWithSHA1_2);
	EVP_add_digest_alias(SN_dsaWithSHA1,"DSS1");
	EVP_add_digest_alias(SN_dsaWithSHA1,"dss1");
#endif
#ifndef OPENSSL_NO_ECDSA
	EVP_add_digest(EVP_ecdsa());
#endif
	/* If you want support for phased out ciphers, add the following */
#if 0
	EVP_add_digest(EVP_sha());
	EVP_add_digest(EVP_dss());
#endif
#ifndef OPENSSL_NO_COMP
	/* This will initialise the built-in compression algorithms.
	   The value returned is a STACK_OF(SSL_COMP), but that can
	   be discarded safely */
	(void)SSL_COMP_get_compression_methods();
#endif
	/* initialize cipher/digest methods table */
	ssl_load_ciphers();
	return(1);
	}
