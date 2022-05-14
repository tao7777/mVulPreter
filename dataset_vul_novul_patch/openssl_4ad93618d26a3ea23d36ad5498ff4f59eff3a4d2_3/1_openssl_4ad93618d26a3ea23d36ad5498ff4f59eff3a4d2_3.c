int tls1_mac(SSL *ssl, SSL3_RECORD *rec, unsigned char *md, int send)
{
    unsigned char *seq;
    EVP_MD_CTX *hash;
    size_t md_size;
    int i;
    EVP_MD_CTX *hmac = NULL, *mac_ctx;
    unsigned char header[13];
    int stream_mac = (send ? (ssl->mac_flags & SSL_MAC_FLAG_WRITE_MAC_STREAM)
                      : (ssl->mac_flags & SSL_MAC_FLAG_READ_MAC_STREAM));
    int t;

    if (send) {
        seq = RECORD_LAYER_get_write_sequence(&ssl->rlayer);
        hash = ssl->write_hash;
    } else {
        seq = RECORD_LAYER_get_read_sequence(&ssl->rlayer);
        hash = ssl->read_hash;
    }

    t = EVP_MD_CTX_size(hash);
    OPENSSL_assert(t >= 0);
    md_size = t;

    /* I should fix this up TLS TLS TLS TLS TLS XXXXXXXX */
    if (stream_mac) {
        mac_ctx = hash;
    } else {
        hmac = EVP_MD_CTX_new();
        if (hmac == NULL || !EVP_MD_CTX_copy(hmac, hash))
            return -1;
        mac_ctx = hmac;
    }

    if (SSL_IS_DTLS(ssl)) {
        unsigned char dtlsseq[8], *p = dtlsseq;

        s2n(send ? DTLS_RECORD_LAYER_get_w_epoch(&ssl->rlayer) :
            DTLS_RECORD_LAYER_get_r_epoch(&ssl->rlayer), p);
        memcpy(p, &seq[2], 6);

        memcpy(header, dtlsseq, 8);
    } else
        memcpy(header, seq, 8);

    header[8] = rec->type;
    header[9] = (unsigned char)(ssl->version >> 8);
    header[10] = (unsigned char)(ssl->version);
     header[11] = (rec->length) >> 8;
     header[12] = (rec->length) & 0xff;
 
    if (!send && !SSL_USE_ETM(ssl) &&
         EVP_CIPHER_CTX_mode(ssl->enc_read_ctx) == EVP_CIPH_CBC_MODE &&
         ssl3_cbc_record_digest_supported(mac_ctx)) {
         /*
         * This is a CBC-encrypted record. We must avoid leaking any
         * timing-side channel information about how many blocks of data we
         * are hashing because that gives an attacker a timing-oracle.
         */
        /* Final param == not SSLv3 */
        if (ssl3_cbc_digest_record(mac_ctx,
                                   md, &md_size,
                                   header, rec->input,
                                   rec->length + md_size, rec->orig_len,
                                   ssl->s3->read_mac_secret,
                                   ssl->s3->read_mac_secret_size, 0) <= 0) {
            EVP_MD_CTX_free(hmac);
            return -1;
        }
    } else {
        if (EVP_DigestSignUpdate(mac_ctx, header, sizeof(header)) <= 0
            || EVP_DigestSignUpdate(mac_ctx, rec->input, rec->length) <= 0
            || EVP_DigestSignFinal(mac_ctx, md, &md_size) <= 0) {
             EVP_MD_CTX_free(hmac);
             return -1;
         }
        if (!send && !SSL_USE_ETM(ssl) && FIPS_mode())
             if (!tls_fips_digest_extra(ssl->enc_read_ctx,
                                        mac_ctx, rec->input,
                                        rec->length, rec->orig_len)) {
                EVP_MD_CTX_free(hmac);
                return -1;
            }
    }

    EVP_MD_CTX_free(hmac);

#ifdef SSL_DEBUG
    fprintf(stderr, "seq=");
    {
        int z;
        for (z = 0; z < 8; z++)
            fprintf(stderr, "%02X ", seq[z]);
        fprintf(stderr, "\n");
    }
    fprintf(stderr, "rec=");
    {
        unsigned int z;
        for (z = 0; z < rec->length; z++)
            fprintf(stderr, "%02X ", rec->data[z]);
        fprintf(stderr, "\n");
    }
#endif

    if (!SSL_IS_DTLS(ssl)) {
        for (i = 7; i >= 0; i--) {
            ++seq[i];
            if (seq[i] != 0)
                break;
        }
    }
#ifdef SSL_DEBUG
    {
        unsigned int z;
        for (z = 0; z < md_size; z++)
            fprintf(stderr, "%02X ", md[z]);
        fprintf(stderr, "\n");
    }
#endif
    return (md_size);
}
