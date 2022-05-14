static SECStatus SelectClientCert(void *arg, PRFileDesc *sock,
                                  struct CERTDistNamesStr *caNames,
                                  struct CERTCertificateStr **pRetCert,
                                  struct SECKEYPrivateKeyStr **pRetKey)
{
   struct ssl_connect_data *connssl = (struct ssl_connect_data *)arg;
   struct Curl_easy *data = connssl->data;
   const char *nickname = connssl->client_nickname;
  static const char pem_slotname[] = "PEM Token #1";
 
   if(connssl->obj_clicert) {
     /* use the cert/key provided by PEM reader */
     SECItem cert_der = { 0, NULL, 0 };
     void *proto_win = SSL_RevealPinArg(sock);
     struct CERTCertificateStr *cert;
    struct SECKEYPrivateKeyStr *key;

    PK11SlotInfo *slot = PK11_FindSlotByName(pem_slotname);
    if(NULL == slot) {
      failf(data, "NSS: PK11 slot not found: %s", pem_slotname);
      return SECFailure;
    }

    if(PK11_ReadRawAttribute(PK11_TypeGeneric, connssl->obj_clicert, CKA_VALUE,
                             &cert_der) != SECSuccess) {
      failf(data, "NSS: CKA_VALUE not found in PK11 generic object");
      PK11_FreeSlot(slot);
      return SECFailure;
    }

    cert = PK11_FindCertFromDERCertItem(slot, &cert_der, proto_win);
    SECITEM_FreeItem(&cert_der, PR_FALSE);
    if(NULL == cert) {
      failf(data, "NSS: client certificate from file not found");
      PK11_FreeSlot(slot);
      return SECFailure;
    }

    key = PK11_FindPrivateKeyFromCert(slot, cert, NULL);
    PK11_FreeSlot(slot);
    if(NULL == key) {
      failf(data, "NSS: private key from file not found");
      CERT_DestroyCertificate(cert);
      return SECFailure;
    }

    infof(data, "NSS: client certificate from file\n");
    display_cert_info(data, cert);

    *pRetCert = cert;
    *pRetKey = key;
    return SECSuccess;
  }

  /* use the default NSS hook */
  if(SECSuccess != NSS_GetClientAuthData((void *)nickname, sock, caNames,
                                          pRetCert, pRetKey)
      || NULL == *pRetCert) {

    if(NULL == nickname)
      failf(data, "NSS: client certificate not found (nickname not "
            "specified)");
    else
      failf(data, "NSS: client certificate not found: %s", nickname);

    return SECFailure;
  }

  /* get certificate nickname if any */
  nickname = (*pRetCert)->nickname;
   if(NULL == nickname)
     nickname = "[unknown]";
 
  if(!strncmp(nickname, pem_slotname, sizeof(pem_slotname) - 1U)) {
    failf(data, "NSS: refusing previously loaded certificate from file: %s",
          nickname);
    return SECFailure;
  }

   if(NULL == *pRetKey) {
     failf(data, "NSS: private key not found for certificate: %s", nickname);
     return SECFailure;
  }

  infof(data, "NSS: using client certificate: %s\n", nickname);
  display_cert_info(data, *pRetCert);
  return SECSuccess;
}
