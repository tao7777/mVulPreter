cib_recv_tls(gnutls_session * session)
crm_recv_tls(gnutls_session * session, size_t max_size, size_t *recv_len, int *disconnected)
 {
     char *buf = NULL;
     int rc = 0;
    size_t len = 0;
    size_t chunk_size = max_size ? max_size : 1024;
    size_t buf_size = 0;
    size_t read_size = 0;
 
     if (session == NULL) {
        if (disconnected) {
            *disconnected = 1;
        }
        goto done;
     }
 
    buf = calloc(1, chunk_size + 1);
    buf_size = chunk_size;
 
     while (TRUE) {
        read_size = buf_size - len;
 
        /* automatically grow the buffer when needed if max_size is not set.*/
        if (!max_size && (read_size < (chunk_size / 2))) {
            buf_size += chunk_size;
            crm_trace("Grow buffer by %d more bytes. buf is now %d bytes", (int)chunk_size, buf_size);
            buf = realloc(buf, buf_size + 1);
            CRM_ASSERT(buf != NULL);
 
            read_size = buf_size - len;
        }
 
        rc = gnutls_record_recv(*session, buf + len, read_size);
 
        if (rc > 0) {
            crm_trace("Got %d more bytes.", rc);
             len += rc;
            /* always null terminate buffer, the +1 to alloc always allows for this.*/
            buf[len] = '\0';
        }
        if (max_size && (max_size == read_size)) {
            crm_trace("Buffer max read size %d met" , max_size);
            goto done;
        }
 
        /* process any errors. */
        if (rc == GNUTLS_E_INTERRUPTED) {
            crm_trace("EINTR encoutered, retry tls read");
        } else if (rc == GNUTLS_E_AGAIN) {
            crm_trace("non-blocking, exiting read on rc = %d", rc);
            goto done;
        } else if (rc <= 0) {
            if (rc == 0) {
                crm_debug("EOF encoutered during TLS read");
            } else {
                crm_debug("Error receiving message: %s (%d)", gnutls_strerror(rc), rc);
            }
            if (disconnected) {
                *disconnected = 1;
            }
            goto done;
         }
     }

done:
    if (recv_len) {
        *recv_len = len;
    }
    if (!len) {
        free(buf);
        buf = NULL;
    }
    return buf;
 
 }
