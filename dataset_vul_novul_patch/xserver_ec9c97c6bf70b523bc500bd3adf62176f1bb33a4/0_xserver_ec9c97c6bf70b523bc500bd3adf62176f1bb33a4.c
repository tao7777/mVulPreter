 int __glXDisp_CreateContext(__GLXclientState *cl, GLbyte *pc)
 {
    ClientPtr client = cl->client;
     xGLXCreateContextReq *req = (xGLXCreateContextReq *) pc;
     __GLXconfig *config;
     __GLXscreen *pGlxScreen;
     int err;
 
    REQUEST_SIZE_MATCH(xGLXCreateContextReq);

     if (!validGlxScreen(cl->client, req->screen, &pGlxScreen, &err))
 	return err;
     if (!validGlxVisual(cl->client, pGlxScreen, req->visual, &config, &err))
			   config, pGlxScreen, req->isDirect);
}
