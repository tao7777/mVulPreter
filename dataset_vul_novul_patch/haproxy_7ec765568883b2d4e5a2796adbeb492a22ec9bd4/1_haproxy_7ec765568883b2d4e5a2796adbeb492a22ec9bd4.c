 void buffer_slow_realign(struct buffer *buf)
 {
       /* two possible cases :
        *   - the buffer is in one contiguous block, we move it in-place
        *   - the buffer is in two blocks, we move it via the swap_buffer
        */
       if (buf->i) {
               int block1 = buf->i;
               int block2 = 0;
               if (buf->p + buf->i > buf->data + buf->size) {
                       /* non-contiguous block */
                       block1 = buf->data + buf->size - buf->p;
                       block2 = buf->p + buf->i - (buf->data + buf->size);
               }
               if (block2)
                       memcpy(swap_buffer, buf->data, block2);
               memmove(buf->data, buf->p, block1);
               if (block2)
                       memcpy(buf->data + block1, swap_buffer, block2);
        }
 
        buf->p = buf->data;
 }
