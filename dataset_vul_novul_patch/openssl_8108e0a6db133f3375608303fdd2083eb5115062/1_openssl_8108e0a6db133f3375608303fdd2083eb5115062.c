Init_ossl_cipher(void)
{
#if 0
    mOSSL = rb_define_module("OpenSSL");
    eOSSLError = rb_define_class_under(mOSSL, "OpenSSLError", rb_eStandardError);
#endif

    /* Document-class: OpenSSL::Cipher
     *
     * Provides symmetric algorithms for encryption and decryption. The
     * algorithms that are available depend on the particular version
     * of OpenSSL that is installed.
     *
     * === Listing all supported algorithms
     *
     * A list of supported algorithms can be obtained by
     *
     *   puts OpenSSL::Cipher.ciphers
     *
     * === Instantiating a Cipher
     *
     * There are several ways to create a Cipher instance. Generally, a
     * Cipher algorithm is categorized by its name, the key length in bits
     * and the cipher mode to be used. The most generic way to create a
     * Cipher is the following
     *
     *   cipher = OpenSSL::Cipher.new('<name>-<key length>-<mode>')
     *
     * That is, a string consisting of the hyphenated concatenation of the
     * individual components name, key length and mode. Either all uppercase
     * or all lowercase strings may be used, for example:
     *
     *  cipher = OpenSSL::Cipher.new('AES-128-CBC')
     *
     * For each algorithm supported, there is a class defined under the
     * Cipher class that goes by the name of the cipher, e.g. to obtain an
     * instance of AES, you could also use
     *
     *   # these are equivalent
     *   cipher = OpenSSL::Cipher::AES.new(128, :CBC)
     *   cipher = OpenSSL::Cipher::AES.new(128, 'CBC')
     *   cipher = OpenSSL::Cipher::AES.new('128-CBC')
     *
     * Finally, due to its wide-spread use, there are also extra classes
     * defined for the different key sizes of AES
     *
     *   cipher = OpenSSL::Cipher::AES128.new(:CBC)
     *   cipher = OpenSSL::Cipher::AES192.new(:CBC)
     *   cipher = OpenSSL::Cipher::AES256.new(:CBC)
     *
     * === Choosing either encryption or decryption mode
     *
     * Encryption and decryption are often very similar operations for
     * symmetric algorithms, this is reflected by not having to choose
     * different classes for either operation, both can be done using the
     * same class. Still, after obtaining a Cipher instance, we need to
     * tell the instance what it is that we intend to do with it, so we
     * need to call either
     *
     *   cipher.encrypt
     *
     * or
     *
     *   cipher.decrypt
     *
     * on the Cipher instance. This should be the first call after creating
     * the instance, otherwise configuration that has already been set could
     * get lost in the process.
     *
     * === Choosing a key
     *
     * Symmetric encryption requires a key that is the same for the encrypting
     * and for the decrypting party and after initial key establishment should
     * be kept as private information. There are a lot of ways to create
     * insecure keys, the most notable is to simply take a password as the key
     * without processing the password further. A simple and secure way to
     * create a key for a particular Cipher is
     *
     *  cipher = OpenSSL::AES256.new(:CFB)
     *  cipher.encrypt
     *  key = cipher.random_key # also sets the generated key on the Cipher
     *
     * If you absolutely need to use passwords as encryption keys, you
     * should use Password-Based Key Derivation Function 2 (PBKDF2) by
     * generating the key with the help of the functionality provided by
     * OpenSSL::PKCS5.pbkdf2_hmac_sha1 or OpenSSL::PKCS5.pbkdf2_hmac.
     *
     * Although there is Cipher#pkcs5_keyivgen, its use is deprecated and
     * it should only be used in legacy applications because it does not use
     * the newer PKCS#5 v2 algorithms.
     *
     * === Choosing an IV
     *
     * The cipher modes CBC, CFB, OFB and CTR all need an "initialization
     * vector", or short, IV. ECB mode is the only mode that does not require
     * an IV, but there is almost no legitimate use case for this mode
     * because of the fact that it does not sufficiently hide plaintext
     * patterns. Therefore
     *
     * <b>You should never use ECB mode unless you are absolutely sure that
     * you absolutely need it</b>
     *
     * Because of this, you will end up with a mode that explicitly requires
     * an IV in any case. Note that for backwards compatibility reasons,
     * setting an IV is not explicitly mandated by the Cipher API. If not
     * set, OpenSSL itself defaults to an all-zeroes IV ("\\0", not the
     * character). Although the IV can be seen as public information, i.e.
     * it may be transmitted in public once generated, it should still stay
     * unpredictable to prevent certain kinds of attacks. Therefore, ideally
     *
     * <b>Always create a secure random IV for every encryption of your
     * Cipher</b>
     *
     * A new, random IV should be created for every encryption of data. Think
     * of the IV as a nonce (number used once) - it's public but random and
     * unpredictable. A secure random IV can be created as follows
     *
     *   cipher = ...
     *   cipher.encrypt
     *   key = cipher.random_key
     *   iv = cipher.random_iv # also sets the generated IV on the Cipher
     *
     * Although the key is generally a random value, too, it is a bad choice
     * as an IV. There are elaborate ways how an attacker can take advantage
     * of such an IV. As a general rule of thumb, exposing the key directly
     * or indirectly should be avoided at all cost and exceptions only be
     * made with good reason.
     *
     * === Calling Cipher#final
     *
     * ECB (which should not be used) and CBC are both block-based modes.
     * This means that unlike for the other streaming-based modes, they
     * operate on fixed-size blocks of data, and therefore they require a
     * "finalization" step to produce or correctly decrypt the last block of
     * data by appropriately handling some form of padding. Therefore it is
     * essential to add the output of OpenSSL::Cipher#final to your
     * encryption/decryption buffer or you will end up with decryption errors
     * or truncated data.
     *
     * Although this is not really necessary for streaming-mode ciphers, it is
     * still recommended to apply the same pattern of adding the output of
     * Cipher#final there as well - it also enables you to switch between
     * modes more easily in the future.
     *
     * === Encrypting and decrypting some data
     *
     *   data = "Very, very confidential data"
     *
     *   cipher = OpenSSL::Cipher::AES.new(128, :CBC)
     *   cipher.encrypt
     *   key = cipher.random_key
     *   iv = cipher.random_iv
     *
     *   encrypted = cipher.update(data) + cipher.final
     *   ...
     *   decipher = OpenSSL::Cipher::AES.new(128, :CBC)
     *   decipher.decrypt
     *   decipher.key = key
     *   decipher.iv = iv
     *
     *   plain = decipher.update(encrypted) + decipher.final
     *
     *   puts data == plain #=> true
     *
     * === Authenticated Encryption and Associated Data (AEAD)
     *
     * If the OpenSSL version used supports it, an Authenticated Encryption
     * mode (such as GCM or CCM) should always be preferred over any
     * unauthenticated mode. Currently, OpenSSL supports AE only in combination
     * with Associated Data (AEAD) where additional associated data is included
     * in the encryption process to compute a tag at the end of the encryption.
     * This tag will also be used in the decryption process and by verifying
     * its validity, the authenticity of a given ciphertext is established.
     *
     * This is superior to unauthenticated modes in that it allows to detect
     * if somebody effectively changed the ciphertext after it had been
     * encrypted. This prevents malicious modifications of the ciphertext that
     * could otherwise be exploited to modify ciphertexts in ways beneficial to
     * potential attackers.
     *
     * An associated data is used where there is additional information, such as
     * headers or some metadata, that must be also authenticated but not
     * necessarily need to be encrypted. If no associated data is needed for
     * encryption and later decryption, the OpenSSL library still requires a
     * value to be set - "" may be used in case none is available.
     *
     * An example using the GCM (Galois/Counter Mode). You have 16 bytes +key+,
     * 12 bytes (96 bits) +nonce+ and the associated data +auth_data+. Be sure
     * not to reuse the +key+ and +nonce+ pair. Reusing an nonce ruins the
     * security gurantees of GCM mode.
     *
     *   cipher = OpenSSL::Cipher::AES.new(128, :GCM).encrypt
     *   cipher.key = key
     *   cipher.iv = nonce
     *   cipher.auth_data = auth_data
     *
     *   encrypted = cipher.update(data) + cipher.final
     *   tag = cipher.auth_tag # produces 16 bytes tag by default
     *
     * Now you are the receiver. You know the +key+ and have received +nonce+,
     * +auth_data+, +encrypted+ and +tag+ through an untrusted network. Note
     * that GCM accepts an arbitrary length tag between 1 and 16 bytes. You may
     * additionally need to check that the received tag has the correct length,
     * or you allow attackers to forge a valid single byte tag for the tampered
     * ciphertext with a probability of 1/256.
     *
     *   raise "tag is truncated!" unless tag.bytesize == 16
     *   decipher = OpenSSL::Cipher::AES.new(128, :GCM).decrypt
     *   decipher.key = key
     *   decipher.iv = nonce
     *   decipher.auth_tag = tag
     *   decipher.auth_data = auth_data
     *
     *   decrypted = decipher.update(encrypted) + decipher.final
     *
     *   puts data == decrypted #=> true
     */
    cCipher = rb_define_class_under(mOSSL, "Cipher", rb_cObject);
    eCipherError = rb_define_class_under(cCipher, "CipherError", eOSSLError);

    rb_define_alloc_func(cCipher, ossl_cipher_alloc);
    rb_define_copy_func(cCipher, ossl_cipher_copy);
    rb_define_module_function(cCipher, "ciphers", ossl_s_ciphers, 0);
    rb_define_method(cCipher, "initialize", ossl_cipher_initialize, 1);
    rb_define_method(cCipher, "reset", ossl_cipher_reset, 0);
    rb_define_method(cCipher, "encrypt", ossl_cipher_encrypt, -1);
    rb_define_method(cCipher, "decrypt", ossl_cipher_decrypt, -1);
    rb_define_method(cCipher, "pkcs5_keyivgen", ossl_cipher_pkcs5_keyivgen, -1);
    rb_define_method(cCipher, "update", ossl_cipher_update, -1);
    rb_define_method(cCipher, "final", ossl_cipher_final, 0);
    rb_define_method(cCipher, "name", ossl_cipher_name, 0);
    rb_define_method(cCipher, "key=", ossl_cipher_set_key, 1);
    rb_define_method(cCipher, "auth_data=", ossl_cipher_set_auth_data, 1);
    rb_define_method(cCipher, "auth_tag=", ossl_cipher_set_auth_tag, 1);
    rb_define_method(cCipher, "auth_tag", ossl_cipher_get_auth_tag, -1);
    rb_define_method(cCipher, "auth_tag_len=", ossl_cipher_set_auth_tag_len, 1);
    rb_define_method(cCipher, "authenticated?", ossl_cipher_is_authenticated, 0);
    rb_define_method(cCipher, "key_len=", ossl_cipher_set_key_length, 1);
    rb_define_method(cCipher, "key_len", ossl_cipher_key_length, 0);
    rb_define_method(cCipher, "iv=", ossl_cipher_set_iv, 1);
    rb_define_method(cCipher, "iv_len=", ossl_cipher_set_iv_length, 1);
    rb_define_method(cCipher, "iv_len", ossl_cipher_iv_length, 0);
    rb_define_method(cCipher, "block_size", ossl_cipher_block_size, 0);
     rb_define_method(cCipher, "padding=", ossl_cipher_set_padding, 1);
 
     id_auth_tag_len = rb_intern_const("auth_tag_len");
 }
