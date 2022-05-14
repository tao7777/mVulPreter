int dtls1_process_buffered_records(SSL *s)
 {
     pitem *item;
     SSL3_BUFFER *rb;
 
     item = pqueue_peek(s->rlayer.d->unprocessed_rcds.q);
     if (item) {
         /* Check if epoch is current. */
         if (s->rlayer.d->unprocessed_rcds.epoch != s->rlayer.d->r_epoch)
            return (1);         /* Nothing to do. */
 
         rb = RECORD_LAYER_get_rbuf(&s->rlayer);
 
             */
            return 1;
        }

        /* Process all the records. */
        while (pqueue_peek(s->rlayer.d->unprocessed_rcds.q)) {
            dtls1_get_unprocessed_record(s);
            if (!dtls1_process_record(s))
                return (0);
            if (dtls1_buffer_record(s, &(s->rlayer.d->processed_rcds),
         /* Process all the records. */
         while (pqueue_peek(s->rlayer.d->unprocessed_rcds.q)) {
             dtls1_get_unprocessed_record(s);
            if (!dtls1_process_record(s))
                return (0);
             if (dtls1_buffer_record(s, &(s->rlayer.d->processed_rcds),
                                    SSL3_RECORD_get_seq_num(s->rlayer.rrec)) <
                0)
                return -1;
         }
     }
 
 *             here, anything else is handled by higher layers
 *     Application data protocol
 *             none of our business
 */
     s->rlayer.d->processed_rcds.epoch = s->rlayer.d->r_epoch;
     s->rlayer.d->unprocessed_rcds.epoch = s->rlayer.d->r_epoch + 1;
 
    return (1);
 }
