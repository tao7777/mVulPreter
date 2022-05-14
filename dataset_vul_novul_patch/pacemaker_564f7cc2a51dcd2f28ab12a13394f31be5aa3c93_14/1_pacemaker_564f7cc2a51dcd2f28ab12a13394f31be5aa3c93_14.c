cib_send_plaintext(int sock, xmlNode * msg)
 {
    char *xml_text = dump_xml_unformatted(msg);
 
    if (xml_text != NULL) {
        int rc = 0;
        char *unsent = xml_text;
        int len = strlen(xml_text);
 
        len++;                  /* null char */
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
                    crm_perror(LOG_ERR, "Could only write %d of the remaining %d bytes", rc, len);
                    break;
            }
 
        } else if (rc < len) {
            crm_trace("Only sent %d of %d remaining bytes", rc, len);
            len -= rc;
            unsent += rc;
             goto retry;
        } else {
            crm_trace("Sent %d bytes: %.100s", rc, xml_text);
         }
     }
    free(xml_text);
    return NULL;
 
 }
