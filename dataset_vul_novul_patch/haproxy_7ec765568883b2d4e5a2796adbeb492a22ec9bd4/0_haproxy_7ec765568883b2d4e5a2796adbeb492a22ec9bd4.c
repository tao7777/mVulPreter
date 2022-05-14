 void buffer_slow_realign(struct buffer *buf)
 {
       int block1 = buf->o;
       int block2 = 0;

       /* process output data in two steps to cover wrapping */
       if (block1 > buf->p - buf->data) {
               block2 = buf->p - buf->data;
               block1 -= block2;
       }
       memcpy(swap_buffer + buf->size - buf->o, bo_ptr(buf), block1);
       memcpy(swap_buffer + buf->size - block2, buf->data, block2);

       /* process input data in two steps to cover wrapping */
       block1 = buf->i;
       block2 = 0;

       if (block1 > buf->data + buf->size - buf->p) {
               block1 = buf->data + buf->size - buf->p;
               block2 = buf->i - block1;
        }
       memcpy(swap_buffer, bi_ptr(buf), block1);
       memcpy(swap_buffer + block1, buf->data, block2);

       /* reinject changes into the buffer */
       memcpy(buf->data, swap_buffer, buf->i);
       memcpy(buf->data + buf->size - buf->o, swap_buffer + buf->size - buf->o, buf->o);
 
        buf->p = buf->data;
 }
