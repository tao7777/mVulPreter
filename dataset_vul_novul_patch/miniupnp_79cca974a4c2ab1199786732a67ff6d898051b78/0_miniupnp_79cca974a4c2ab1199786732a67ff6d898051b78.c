 void IGDstartelt(void * d, const char * name, int l)
 {
 	struct IGDdatas * datas = (struct IGDdatas *)d;
	if(l >= MINIUPNPC_URL_MAXSIZE)
		l = MINIUPNPC_URL_MAXSIZE-1;
	memcpy(datas->cureltname, name, l);
 	datas->cureltname[l] = '\0';
 	datas->level++;
 	if( (l==7) && !memcmp(name, "service", l) ) {
		datas->tmp.controlurl[0] = '\0';
		datas->tmp.eventsuburl[0] = '\0';
		datas->tmp.scpdurl[0] = '\0';
		datas->tmp.servicetype[0] = '\0';
	}
}
