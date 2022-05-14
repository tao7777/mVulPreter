CURLcode Curl_smtp_escape_eob(struct connectdata *conn, const ssize_t nread)
{
  /* When sending a SMTP payload we must detect CRLF. sequences making sure
     they are sent as CRLF.. instead, as a . on the beginning of a line will
     be deleted by the server when not part of an EOB terminator and a
     genuine CRLF.CRLF which isn't escaped will wrongly be detected as end of
     data by the server
  */
  ssize_t i;
  ssize_t si;
  struct Curl_easy *data = conn->data;
  struct SMTP *smtp = data->req.protop;
  char *scratch = data->state.scratch;
  char *newscratch = NULL;
  char *oldscratch = NULL;
  size_t eob_sent;

  /* Do we need to allocate a scratch buffer? */
   if(!scratch || data->set.crlf) {
     oldscratch = scratch;
 
    scratch = newscratch = malloc(2 * UPLOAD_BUFSIZE);
     if(!newscratch) {
       failf(data, "Failed to alloc scratch buffer!");
 
       return CURLE_OUT_OF_MEMORY;
     }
   }
  DEBUGASSERT(UPLOAD_BUFSIZE >= nread);
 
   /* Have we already sent part of the EOB? */
   eob_sent = smtp->eob;

  /* This loop can be improved by some kind of Boyer-Moore style of
     approach but that is saved for later... */
  for(i = 0, si = 0; i < nread; i++) {
    if(SMTP_EOB[smtp->eob] == data->req.upload_fromhere[i]) {
      smtp->eob++;

      /* Is the EOB potentially the terminating CRLF? */
      if(2 == smtp->eob || SMTP_EOB_LEN == smtp->eob)
        smtp->trailing_crlf = TRUE;
      else
        smtp->trailing_crlf = FALSE;
    }
    else if(smtp->eob) {
      /* A previous substring matched so output that first */
      memcpy(&scratch[si], &SMTP_EOB[eob_sent], smtp->eob - eob_sent);
      si += smtp->eob - eob_sent;

      /* Then compare the first byte */
      if(SMTP_EOB[0] == data->req.upload_fromhere[i])
        smtp->eob = 1;
      else
        smtp->eob = 0;

      eob_sent = 0;

      /* Reset the trailing CRLF flag as there was more data */
      smtp->trailing_crlf = FALSE;
    }

    /* Do we have a match for CRLF. as per RFC-5321, sect. 4.5.2 */
    if(SMTP_EOB_FIND_LEN == smtp->eob) {
      /* Copy the replacement data to the target buffer */
      memcpy(&scratch[si], &SMTP_EOB_REPL[eob_sent],
             SMTP_EOB_REPL_LEN - eob_sent);
      si += SMTP_EOB_REPL_LEN - eob_sent;
      smtp->eob = 0;
      eob_sent = 0;
    }
    else if(!smtp->eob)
      scratch[si++] = data->req.upload_fromhere[i];
  }

  if(smtp->eob - eob_sent) {
    /* A substring matched before processing ended so output that now */
    memcpy(&scratch[si], &SMTP_EOB[eob_sent], smtp->eob - eob_sent);
    si += smtp->eob - eob_sent;
  }

  /* Only use the new buffer if we replaced something */
  if(si != nread) {
    /* Upload from the new (replaced) buffer instead */
    data->req.upload_fromhere = scratch;

    /* Save the buffer so it can be freed later */
    data->state.scratch = scratch;

    /* Free the old scratch buffer */
    free(oldscratch);

    /* Set the new amount too */
    data->req.upload_present = si;
  }
  else
    free(newscratch);

  return CURLE_OK;
}
