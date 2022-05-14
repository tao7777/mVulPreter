static int cms_RecipientInfo_ktri_decrypt(CMS_ContentInfo *cms,
                                          CMS_RecipientInfo *ri)
{
    CMS_KeyTransRecipientInfo *ktri = ri->d.ktri;
    EVP_PKEY *pkey = ktri->pkey;
     unsigned char *ek = NULL;
     size_t eklen;
     int ret = 0;
    size_t fixlen = 0;
     CMS_EncryptedContentInfo *ec;
     ec = cms->d.envelopedData->encryptedContentInfo;
 
        CMSerr(CMS_F_CMS_RECIPIENTINFO_KTRI_DECRYPT, CMS_R_NO_PRIVATE_KEY);
        return 0;
         return 0;
     }
