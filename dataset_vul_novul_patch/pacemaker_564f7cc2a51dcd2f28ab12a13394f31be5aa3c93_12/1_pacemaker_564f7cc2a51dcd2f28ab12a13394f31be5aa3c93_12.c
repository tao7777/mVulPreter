cib_recv_plaintext(int sock)
 {
     char *buf = NULL;
     ssize_t rc = 0;
     ssize_t len = 0;
    ssize_t chunk_size = 512;
 
    buf = calloc(1, chunk_size);
 
    while (1) {
        errno = 0;
        rc = read(sock, buf + len, chunk_size);
        crm_trace("Got %d more bytes. errno=%d", (int)rc, errno);
        if (errno == EINTR || errno == EAGAIN) {
            crm_trace("Retry: %d", (int)rc);
            if (rc > 0) {
                len += rc;
                buf = realloc(buf, len + chunk_size);
                CRM_ASSERT(buf != NULL);
            }
 
        } else if (rc < 0) {
            crm_perror(LOG_ERR, "Error receiving message: %d", (int)rc);
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
            return buf;
         }
     }
  bail:
    free(buf);
    return NULL;
 
 }
