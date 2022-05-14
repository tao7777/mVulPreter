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
