static int dtls1_retrieve_buffered_fragment(SSL *s, int *ok)
{
    /*-
     * (0) check whether the desired fragment is available
     * if so:
     * (1) copy over the fragment to s->init_buf->data[]
     * (2) update s->init_num
     */
    pitem *item;
    hm_fragment *frag;
     int al;
 
     *ok = 0;
    item = pqueue_peek(s->d1->buffered_messages);
    if (item == NULL)
        return 0;
 
    frag = (hm_fragment *)item->data;
 
     /* Don't return if reassembly still in progress */
     if (frag->reassembly != NULL)
                   frag->msg_header.frag_len);
        }
