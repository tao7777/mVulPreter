BIO *PKCS7_dataInit(PKCS7 *p7, BIO *bio)
{
    int i;
    BIO *out = NULL, *btmp = NULL;
    X509_ALGOR *xa = NULL;
    const EVP_CIPHER *evp_cipher = NULL;
    STACK_OF(X509_ALGOR) *md_sk = NULL;
    STACK_OF(PKCS7_RECIP_INFO) *rsk = NULL;
    X509_ALGOR *xalg = NULL;
     PKCS7_RECIP_INFO *ri = NULL;
     ASN1_OCTET_STRING *os = NULL;
 
    if (p7 == NULL) {
        PKCS7err(PKCS7_F_PKCS7_DATAINIT, PKCS7_R_INVALID_NULL_POINTER);
        return NULL;
    }
    /*
     * The content field in the PKCS7 ContentInfo is optional, but that really
     * only applies to inner content (precisely, detached signatures).
     *
     * When reading content, missing outer content is therefore treated as an
     * error.
     *
     * When creating content, PKCS7_content_new() must be called before
     * calling this method, so a NULL p7->d is always an error.
     */
    if (p7->d.ptr == NULL) {
        PKCS7err(PKCS7_F_PKCS7_DATAINIT, PKCS7_R_NO_CONTENT);
        return NULL;
    }

     i = OBJ_obj2nid(p7->type);
     p7->state = PKCS7_S_HEADER;
 
        if (evp_cipher == NULL) {
            PKCS7err(PKCS7_F_PKCS7_DATAINIT, PKCS7_R_CIPHER_NOT_INITIALIZED);
            goto err;
        }
        break;
    case NID_pkcs7_digest:
        xa = p7->d.digest->md;
        os = PKCS7_get_octet_string(p7->d.digest->contents);
        break;
    case NID_pkcs7_data:
        break;
    default:
        PKCS7err(PKCS7_F_PKCS7_DATAINIT, PKCS7_R_UNSUPPORTED_CONTENT_TYPE);
        goto err;
    }
