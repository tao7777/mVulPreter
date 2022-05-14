long PKCS7_ctrl(PKCS7 *p7, int cmd, long larg, char *parg)
{
    int nid;
    long ret;

     nid = OBJ_obj2nid(p7->type);
 
     switch (cmd) {
     case PKCS7_OP_SET_DETACHED_SIGNATURE:
         if (nid == NID_pkcs7_signed) {
             ret = p7->detached = (int)larg;
                ASN1_OCTET_STRING *os;
                os = p7->d.sign->contents->d.data;
                ASN1_OCTET_STRING_free(os);
                p7->d.sign->contents->d.data = NULL;
            }
        } else {
            PKCS7err(PKCS7_F_PKCS7_CTRL,
                     PKCS7_R_OPERATION_NOT_SUPPORTED_ON_THIS_TYPE);
            ret = 0;
        }
        break;
    case PKCS7_OP_GET_DETACHED_SIGNATURE:
        if (nid == NID_pkcs7_signed) {
            if (!p7->d.sign || !p7->d.sign->contents->d.ptr)
                ret = 1;
            else
                ret = 0;

            p7->detached = ret;
        } else {
            PKCS7err(PKCS7_F_PKCS7_CTRL,
                     PKCS7_R_OPERATION_NOT_SUPPORTED_ON_THIS_TYPE);
            ret = 0;
        }

        break;
    default:
        PKCS7err(PKCS7_F_PKCS7_CTRL, PKCS7_R_UNKNOWN_OPERATION);
        ret = 0;
    }
