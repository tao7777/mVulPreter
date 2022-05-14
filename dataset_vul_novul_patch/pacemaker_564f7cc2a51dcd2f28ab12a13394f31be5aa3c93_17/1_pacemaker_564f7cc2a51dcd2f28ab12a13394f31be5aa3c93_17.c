crm_recv_remote_msg(void *session, gboolean encrypted)
 {
    char *reply = NULL;
     xmlNode *xml = NULL;
 
     if (encrypted) {
 #ifdef HAVE_GNUTLS_GNUTLS_H
        reply = cib_recv_tls(session);
 #else
         CRM_ASSERT(encrypted == FALSE);
 #endif
     } else {
        reply = cib_recv_plaintext(GPOINTER_TO_INT(session));
     }
     if (reply == NULL || strlen(reply) == 0) {
         crm_trace("Empty reply");
 
    } else {
        xml = string2xml(reply);
        if (xml == NULL) {
            crm_err("Couldn't parse: '%.120s'", reply);
         }
     }
 
    free(reply);
    return xml;
 }
