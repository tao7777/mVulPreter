int am_check_url(request_rec *r, const char *url)
{
    const char *i;

    for (i = url; *i; i++) {
        if (*i >= 0 && *i < ' ') {
            /* Deny all control-characters. */
            AM_LOG_RERROR(APLOG_MARK, APLOG_ERR, HTTP_BAD_REQUEST, r,
                           "Control character detected in URL.");
             return HTTP_BAD_REQUEST;
         }
        if (*i == '\\') {
            /* Reject backslash character, as it can be used to bypass
             * redirect URL validation. */
            AM_LOG_RERROR(APLOG_MARK, APLOG_ERR, HTTP_BAD_REQUEST, r,
                          "Backslash character detected in URL.");
            return HTTP_BAD_REQUEST;
        }
     }
 
     return OK;
}
