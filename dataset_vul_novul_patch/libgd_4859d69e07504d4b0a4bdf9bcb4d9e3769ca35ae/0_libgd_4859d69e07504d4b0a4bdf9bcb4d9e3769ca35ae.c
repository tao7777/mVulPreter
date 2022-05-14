 static int dynamicGetbuf(gdIOCtxPtr ctx, void *buf, int len)
 {
 	int rlen, remain;
	dpIOCtxPtr dctx;
	dynamicPtr *dp;

 	dctx = (dpIOCtxPtr) ctx;
 	dp = dctx->dp;
 
	if (dp->pos < 0 || dp->pos >= dp->realSize) {
		return 0;
	}

 	remain = dp->logicalSize - dp->pos;
 	if(remain >= len) {
 		rlen = len;
 	} else {
 		if(remain <= 0) {
			return 0;
 		}
 
 		rlen = remain;
 	}
 
	if (dp->pos + rlen > dp->realSize) {
		rlen = dp->realSize - dp->pos;
	}

 	memcpy(buf, (void *) ((char *)dp->data + dp->pos), rlen);
 	dp->pos += rlen;
 
	return rlen;
}
