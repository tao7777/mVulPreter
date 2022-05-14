crm_send_remote_msg(void *session, xmlNode * msg, gboolean encrypted)
static int
crm_send_remote_msg_raw(void *session, const char *buf, size_t len, gboolean encrypted)
 {
    int rc = -1;
     if (encrypted) {
 #ifdef HAVE_GNUTLS_GNUTLS_H
        rc = crm_send_tls(session, buf, len);
 #else
         CRM_ASSERT(encrypted == FALSE);
 #endif
     } else {
        rc = crm_send_plaintext(GPOINTER_TO_INT(session), buf, len);
     }
    return rc;
 }
