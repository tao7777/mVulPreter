static int ne2000_buffer_full(NE2000State *s)
 {
     int avail, index, boundary;
 
    if (s->stop <= s->start) {
        return 1;
    }

     index = s->curpag << 8;
     boundary = s->boundary << 8;
     if (index < boundary)
        return 1;
    return 0;
}
