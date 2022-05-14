 int __glXDispSwap_CreateContext(__GLXclientState *cl, GLbyte *pc)
 {
    ClientPtr client = cl->client;
     xGLXCreateContextReq *req = (xGLXCreateContextReq *) pc;
     __GLX_DECLARE_SWAP_VARIABLES;
 
    REQUEST_SIZE_MATCH(xGLXCreateContextReq);

     __GLX_SWAP_SHORT(&req->length);
     __GLX_SWAP_INT(&req->context);
     __GLX_SWAP_INT(&req->visual);
    return __glXDisp_CreateContext(cl, pc);
}
