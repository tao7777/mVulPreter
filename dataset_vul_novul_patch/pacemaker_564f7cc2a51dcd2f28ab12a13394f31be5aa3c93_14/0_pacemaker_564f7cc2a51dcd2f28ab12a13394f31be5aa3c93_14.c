cib_send_plaintext(int sock, xmlNode * msg)
static int
crm_send_plaintext(int sock, const char *buf, size_t len)
 {
 
    int rc = 0;
    const char *unsent = buf;
    int total_send;
 
    if (buf == NULL) {
        return -1;
    }
    total_send = len;
 
    crm_trace("Message on socket %d: size=%d", sock, len);
  retry:
    rc = write(sock, unsent, len);
    if (rc < 0) {
        switch (errno) {
        case EINTR:
        case EAGAIN:
            crm_trace("Retry");
             goto retry;
        default:
            crm_perror(LOG_ERR, "Could only write %d of the remaining %d bytes", rc, (int) len);
            break;
         }

    } else if (rc < len) {
        crm_trace("Only sent %d of %d remaining bytes", rc, len);
        len -= rc;
        unsent += rc;
        goto retry;

     } else {
        crm_trace("Sent %d bytes: %.100s", rc, buf);
     }

    return rc < 0 ? rc : total_send;
 
 }
