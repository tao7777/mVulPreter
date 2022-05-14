zsetstrokecolorspace(i_ctx_t * i_ctx_p)
{
    int code;

 zsetstrokecolorspace(i_ctx_t * i_ctx_p)
 {
     int code;
    es_ptr iesp = esp;  /* preserve exec stack in case of error */
 
     code = zswapcolors(i_ctx_p);
     if (code < 0)
    /* Now, the actual continuation routine */
    push_op_estack(setstrokecolorspace_cont);

    code = zsetcolorspace(i_ctx_p);
    if (code >= 0)
        return o_push_estack;

    return code;
}
     if (code >= 0)
         return o_push_estack;
 
    /* Something went wrong, swap back to the non-stroking space and restore the exec stack */
    esp = iesp;
    (void)zswapcolors(i_ctx_p);
     return code;
 }
