static void nbd_recv_coroutines_enter_all(NBDClientSession *s)
static void nbd_recv_coroutines_enter_all(BlockDriverState *bs)
 {
    NBDClientSession *s = nbd_get_client_session(bs);
     int i;
 
     for (i = 0; i < MAX_NBD_REQUESTS; i++) {
            qemu_coroutine_enter(s->recv_coroutine[i]);
             qemu_coroutine_enter(s->recv_coroutine[i]);
         }
     }
