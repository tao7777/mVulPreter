cib_send_tls(gnutls_session * session, xmlNode * msg)
static int
crm_send_tls(gnutls_session * session, const char *buf, size_t len)
 {
    const char *unsent = buf;
    int rc = 0;
    int total_send;
 
    if (buf == NULL) {
        return -1;
     }
 
    total_send = len;
    crm_trace("Message size: %d", len);
 
    while (TRUE) {
        rc = gnutls_record_send(*session, unsent, len);
 
        if (rc == GNUTLS_E_INTERRUPTED || rc == GNUTLS_E_AGAIN) {
            crm_debug("Retry");
 
        } else if (rc < 0) {
            crm_err("Connection terminated rc = %d", rc);
            break;
 
        } else if (rc < len) {
            crm_debug("Only sent %d of %d bytes", rc, len);
            len -= rc;
            unsent += rc;
        } else {
            crm_debug("Sent %d bytes", rc);
            break;
         }
     }
 
    return rc < 0 ? rc : total_send;
 }
