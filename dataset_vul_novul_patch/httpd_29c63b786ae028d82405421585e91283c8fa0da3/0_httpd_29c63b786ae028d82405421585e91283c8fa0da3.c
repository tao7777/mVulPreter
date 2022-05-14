static int on_header_cb(nghttp2_session *ngh2, const nghttp2_frame *frame,
                        const uint8_t *name, size_t namelen,
                        const uint8_t *value, size_t valuelen,
                        uint8_t flags,
                        void *userp)
{
    h2_session *session = (h2_session *)userp;
    h2_stream * stream;
    apr_status_t status;
    
     (void)flags;
     stream = get_stream(session, frame->hd.stream_id);
     if (!stream) {
        ap_log_cerror(APLOG_MARK, APLOG_DEBUG, 0, session->c,
                       APLOGNO(02920) 
                       "h2_session:  stream(%ld-%d): on_header unknown stream",
                       session->id, (int)frame->hd.stream_id);
        return NGHTTP2_ERR_TEMPORAL_CALLBACK_FAILURE;
    }
     
     status = h2_stream_add_header(stream, (const char *)name, namelen,
                                   (const char *)value, valuelen);
    if (status == APR_ECONNRESET) {
        ap_log_cerror(APLOG_MARK, APLOG_TRACE1, status, session->c,
                      "h2-stream(%ld-%d): on_header, reset stream",
                      session->id, stream->id);
        nghttp2_submit_rst_stream(ngh2, NGHTTP2_FLAG_NONE, stream->id,
                                  NGHTTP2_INTERNAL_ERROR);
    }
    else if (status != APR_SUCCESS && !h2_stream_is_ready(stream)) {
         return NGHTTP2_ERR_TEMPORAL_CALLBACK_FAILURE;
     }
     return 0;
}
