static CURLcode imap_parse_url_path(struct connectdata *conn)
{
  /* the imap struct is already inited in imap_connect() */
   struct imap_conn *imapc = &conn->proto.imapc;
   struct SessionHandle *data = conn->data;
   const char *path = data->state.path;
 
   if(!*path)
     path = "INBOX";
 
   /* url decode the path and use this mailbox */
  return Curl_urldecode(data, path, 0, &imapc->mailbox, NULL, TRUE);
 }
