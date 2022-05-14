long ZEXPORT inflateMark(strm)
z_streamp strm;
 {
     struct inflate_state FAR *state;
 
    if (strm == Z_NULL || strm->state == Z_NULL)
        return (long)(((unsigned long)0 - 1) << 16);
     state = (struct inflate_state FAR *)strm->state;
    return (long)(((unsigned long)((long)state->back)) << 16) +
         (state->mode == COPY ? state->length :
             (state->mode == MATCH ? state->was - state->length : 0));
 }
