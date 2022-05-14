crm_send_remote_msg(void *session, xmlNode * msg, gboolean encrypted)
 {
     if (encrypted) {
 #ifdef HAVE_GNUTLS_GNUTLS_H
        cib_send_tls(session, msg);
 #else
         CRM_ASSERT(encrypted == FALSE);
 #endif
     } else {
        cib_send_plaintext(GPOINTER_TO_INT(session), msg);
     }
 }
