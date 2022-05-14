cib_send_tls(gnutls_session * session, xmlNode * msg)
 {
    char *xml_text = NULL;
#  if 0
    const char *name = crm_element_name(msg);
 
    if (safe_str_neq(name, "cib_command")) {
        xmlNodeSetName(msg, "cib_result");
     }
#  endif
    xml_text = dump_xml_unformatted(msg);
    if (xml_text != NULL) {
        char *unsent = xml_text;
        int len = strlen(xml_text);
        int rc = 0;
 
        len++;                  /* null char */
        crm_trace("Message size: %d", len);
 
        while (TRUE) {
            rc = gnutls_record_send(*session, unsent, len);
            crm_debug("Sent %d bytes", rc);
 
            if (rc == GNUTLS_E_INTERRUPTED || rc == GNUTLS_E_AGAIN) {
                crm_debug("Retry");
 
            } else if (rc < 0) {
                crm_debug("Connection terminated");
                break;
 
            } else if (rc < len) {
                crm_debug("Only sent %d of %d bytes", rc, len);
                len -= rc;
                unsent += rc;
            } else {
                break;
            }
         }
     }
    free(xml_text);
    return NULL;
 
 }
