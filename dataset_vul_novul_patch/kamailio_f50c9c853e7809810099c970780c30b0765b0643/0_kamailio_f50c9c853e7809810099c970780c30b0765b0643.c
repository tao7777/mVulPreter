int encode_msg(struct sip_msg *msg,char *payload,int len)
{
   int i,j,k,u,request;
   unsigned short int h;
   struct hdr_field* hf;
   struct msg_start* ms;
   struct sip_uri miuri;
   char *myerror=NULL;
   ptrdiff_t diff;
 
    if(len < MAX_ENCODED_MSG + MAX_MESSAGE_LEN)
       return -1;

    if(parse_headers(msg,HDR_EOH_F,0)<0){
       myerror="in parse_headers";
       goto error;
   }
   memset(payload,0,len);
   ms=&msg->first_line;
	if(ms->type == SIP_REQUEST)
		request=1;
	else if(ms->type == SIP_REPLY)
		request=0;
	else{
		myerror="message is neither request nor response";
		goto error;
	}
	if(request) {
		for(h=0;h<32;j=(0x01<<h),h++)
			if(j & ms->u.request.method_value)
				break;
	} else {
		h=(unsigned short)(ms->u.reply.statuscode);
	}
   if(h==32){/*statuscode wont be 32...*/
      myerror="unknown message type\n";
      goto error;
   }
   h=htons(h);
   /*first goes the message code type*/
   memcpy(payload,&h,2);
   h=htons((unsigned short int)msg->len);
   /*then goes the message start idx, but we'll put it later*/
   /*then goes the message length (we hope it to be less than 65535 bytes...)*/
   memcpy(&payload[MSG_LEN_IDX],&h,2);
   /*then goes the content start index (starting from SIP MSG START)*/
   if(0>(diff=(get_body(msg)-(msg->buf)))){
      myerror="body starts before the message (uh ?)";
      goto error;
   }else
      h=htons((unsigned short int)diff);
   memcpy(payload+CONTENT_IDX,&h,2);
   payload[METHOD_CODE_IDX]=(unsigned char)(request?
	 (ms->u.request.method.s-msg->buf):
	 (ms->u.reply.status.s-msg->buf));
   payload[METHOD_CODE_IDX+1]=(unsigned char)(request?
	 (ms->u.request.method.len):
	 (ms->u.reply.status.len));
   payload[URI_REASON_IDX]=(unsigned char)(request?
	 (ms->u.request.uri.s-msg->buf):
	 (ms->u.reply.reason.s-msg->buf));
   payload[URI_REASON_IDX+1]=(unsigned char)(request?
	 (ms->u.request.uri.len):
	 (ms->u.reply.reason.len));
   payload[VERSION_IDX]=(unsigned char)(request?
	 (ms->u.request.version.s-msg->buf):
	 (ms->u.reply.version.s-msg->buf));
   if(request){
      if (parse_uri(ms->u.request.uri.s,ms->u.request.uri.len, &miuri)<0){
	 LM_ERR("<%.*s>\n",ms->u.request.uri.len,ms->u.request.uri.s);
	 myerror="while parsing the R-URI";
	 goto error;
      }
      if(0>(j=encode_uri2(msg->buf,
		  ms->u.request.method.s-msg->buf+ms->len,
		  ms->u.request.uri,&miuri,
		  (unsigned char*)&payload[REQUEST_URI_IDX+1])))
      {
	    myerror="ENCODE_MSG: ERROR while encoding the R-URI";
	    goto error;
      }
      payload[REQUEST_URI_IDX]=(unsigned char)j;
      k=REQUEST_URI_IDX+1+j;
   }else
      k=REQUEST_URI_IDX;
   u=k;
   k++;
   for(i=0,hf=msg->headers;hf;hf=hf->next,i++);
   i++;/*we do as if there was an extra header, that marks the end of
	 the previous header in the headers hashtable(read below)*/
   j=k+3*i;
   for(i=0,hf=msg->headers;hf;hf=hf->next,k+=3){
      payload[k]=(unsigned char)(hf->type & 0xFF);
      h=htons(j);
      /*now goes a payload-based-ptr to where the header-code starts*/
      memcpy(&payload[k+1],&h,2);
      /*TODO fix this... fixed with k-=3?*/
      if(0>(i=encode_header(msg,hf,(unsigned char*)(payload+j),MAX_ENCODED_MSG+MAX_MESSAGE_LEN-j))){
	 LM_ERR("encoding header %.*s\n",hf->name.len,hf->name.s);
	 goto error;
	 k-=3;
	 continue;
      }
      j+=(unsigned short int)i;
   }
   /*now goes the number of headers that have been found, right after the meta-msg-section*/
   payload[u]=(unsigned char)((k-u-1)/3);
   j=htons(j);
   /*now copy the number of bytes that the headers-meta-section has occupied,right afther
    * headers-meta-section(the array with ['v',[2:where],'r',[2:where],'R',[2:where],...]
    * this is to know where the LAST header ends, since the length of each header-struct
    * is calculated substracting the nextHeaderStart - presentHeaderStart 
    * the k+1 is because payload[k] is usually the letter*/
   memcpy(&payload[k+1],&j,2);
   k+=3;
   j=ntohs(j);
   /*now we copy the headers-meta-section after the msg-headers-meta-section*/
   /*memcpy(&payload[k],payload2,j);*/
    /*j+=k;*/
    /*pkg_free(payload2);*/
    /*now we copy the actual message after the headers-meta-section*/

	if(len < j + msg->len + 1) {
   	   LM_ERR("not enough space to encode sip message\n");
   	   return -1;
	}
    memcpy(&payload[j],msg->buf,msg->len);
    LM_DBG("msglen = %d,msg starts at %d\n",msg->len,j);
    j=htons(j);
   /*now we copy at the beginning, the index to where the actual message starts*/
   memcpy(&payload[MSG_START_IDX],&j,2);
   return GET_PAY_SIZE( payload );
error:
   LM_ERR("%s\n",myerror);
   return -1;
}
