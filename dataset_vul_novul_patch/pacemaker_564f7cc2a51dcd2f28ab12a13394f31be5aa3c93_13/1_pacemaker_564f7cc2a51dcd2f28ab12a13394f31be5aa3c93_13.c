cib_recv_tls(gnutls_session * session)
 {
     char *buf = NULL;
     int rc = 0;
    int len = 0;
    int chunk_size = 1024;
 
     if (session == NULL) {
        return NULL;
     }
 
    buf = calloc(1, chunk_size);
 
     while (TRUE) {
        errno = 0;
        rc = gnutls_record_recv(*session, buf + len, chunk_size);
        crm_trace("Got %d more bytes. errno=%d", rc, errno);
 
        if (rc == GNUTLS_E_INTERRUPTED || rc == GNUTLS_E_AGAIN) {
            crm_trace("Retry");
 
        } else if (rc == GNUTLS_E_UNEXPECTED_PACKET_LENGTH) {
            crm_trace("Session disconnected");
            goto bail;
 
        } else if (rc < 0) {
            crm_err("Error receiving message: %s (%d)", gnutls_strerror(rc), rc);
            goto bail;
 
        } else if (rc == chunk_size) {
             len += rc;
            chunk_size *= 2;
            buf = realloc(buf, len + chunk_size);
            crm_trace("Retry with %d more bytes", (int)chunk_size);
            CRM_ASSERT(buf != NULL);
        } else if (buf[len + rc - 1] != 0) {
            crm_trace("Last char is %d '%c'", buf[len + rc - 1], buf[len + rc - 1]);
            crm_trace("Retry with %d more bytes", (int)chunk_size);
            len += rc;
            buf = realloc(buf, len + chunk_size);
            CRM_ASSERT(buf != NULL);
 
        } else {
            crm_trace("Got %d more bytes", (int)rc);
            return buf;
         }
     }
  bail:
    free(buf);
    return NULL;
 
 }
