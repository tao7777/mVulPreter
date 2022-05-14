 int __glXDispSwap_CreateContext(__GLXclientState *cl, GLbyte *pc)
 {
     xGLXCreateContextReq *req = (xGLXCreateContextReq *) pc;
     __GLX_DECLARE_SWAP_VARIABLES;
 
     __GLX_SWAP_SHORT(&req->length);
     __GLX_SWAP_INT(&req->context);
     __GLX_SWAP_INT(&req->visual);
    return __glXDisp_CreateContext(cl, pc);
}
