 zsetstrokecolor(i_ctx_t * i_ctx_p)
 {
     int code;
 
     code = zswapcolors(i_ctx_p);
     if (code < 0)

    /* Set up for the continuation procedure which will finish by restoring the fill colour space */
    /* Make sure the exec stack has enough space */
    check_estack(1);
    /* Now, the actual continuation routine */
    push_op_estack(setstrokecolor_cont);

    code = zsetcolor(i_ctx_p);

    if (code >= 0)
     if (code >= 0)
         return o_push_estack;
 
     return code;
 }
