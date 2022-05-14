zsetstrokecolorspace(i_ctx_t * i_ctx_p)
{
    int code;

 zsetstrokecolorspace(i_ctx_t * i_ctx_p)
 {
     int code;
 
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
 
     return code;
 }
