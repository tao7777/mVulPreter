static void nbd_recv_coroutines_enter_all(NBDClientSession *s)
 {
     int i;
 
     for (i = 0; i < MAX_NBD_REQUESTS; i++) {
            qemu_coroutine_enter(s->recv_coroutine[i]);
             qemu_coroutine_enter(s->recv_coroutine[i]);
         }
     }
