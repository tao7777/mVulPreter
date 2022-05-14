apr_status_t h2_stream_add_header(h2_stream *stream,
                                   const char *name, size_t nlen,
                                   const char *value, size_t vlen)
 {
    int error = 0;
     ap_assert(stream);
     
    if (stream->has_response) {
        return APR_EINVAL;    
    }
    ++stream->request_headers_added;
    if (name[0] == ':') {
        if ((vlen) > stream->session->s->limit_req_line) {
            /* pseudo header: approximation of request line size check */
             ap_log_cerror(APLOG_MARK, APLOG_TRACE1, 0, stream->session->c,
                          "h2_stream(%ld-%d): pseudo header %s too long", 
                           stream->session->id, stream->id, name);
            error = HTTP_REQUEST_URI_TOO_LARGE;
         }
    }
    else if ((nlen + 2 + vlen) > stream->session->s->limit_req_fieldsize) {
        /* header too long */
        ap_log_cerror(APLOG_MARK, APLOG_TRACE1, 0, stream->session->c,
                      "h2_stream(%ld-%d): header %s too long", 
                      stream->session->id, stream->id, name);
        error = HTTP_REQUEST_HEADER_FIELDS_TOO_LARGE;
    }
    
    if (stream->request_headers_added 
        > stream->session->s->limit_req_fields + 4) {
        /* too many header lines, include 4 pseudo headers */
        if (stream->request_headers_added 
            > stream->session->s->limit_req_fields + 4 + 100) {
            /* yeah, right */
            return APR_ECONNRESET;
         }
        ap_log_cerror(APLOG_MARK, APLOG_TRACE1, 0, stream->session->c,
                      "h2_stream(%ld-%d): too many header lines", 
                      stream->session->id, stream->id);
        error = HTTP_REQUEST_HEADER_FIELDS_TOO_LARGE;
     }
     
     if (h2_stream_is_scheduled(stream)) {
         return add_trailer(stream, name, nlen, value, vlen);
     }
    else if (error) {
        return h2_stream_set_error(stream, error); 
    }
     else {
         if (!stream->rtmp) {
             stream->rtmp = h2_req_create(stream->id, stream->pool, 
                                         NULL, NULL, NULL, NULL, NULL, 0);
        }
        if (stream->state != H2_STREAM_ST_OPEN) {
            return APR_ECONNRESET;
        }
        return h2_request_add_header(stream->rtmp, stream->pool,
                                     name, nlen, value, vlen);
    }
}
