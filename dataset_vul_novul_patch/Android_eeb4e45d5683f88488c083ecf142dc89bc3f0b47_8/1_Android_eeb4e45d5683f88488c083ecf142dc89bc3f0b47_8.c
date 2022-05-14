int vorbis_book_unpack(oggpack_buffer *opb,codebook *s){
 char *lengthlist=NULL;
 int           quantvals=0;
 long          i,j;
 int           maptype;

  memset(s,0,sizeof(*s));

 /* make sure alignment is correct */
 if(oggpack_read(opb,24)!=0x564342)goto _eofout;

 /* first the basic parameters */
  s->dim=oggpack_read(opb,16);
  s->dec_buf=_ogg_malloc(sizeof(ogg_int32_t)*s->dim);
 if (s->dec_buf == NULL)
 goto _errout;
  s->entries=oggpack_read(opb,24);
 if(s->entries<=0)goto _eofout;
 if(s->dim<=0)goto _eofout;
 if(_ilog(s->dim)+_ilog(s->entries)>24)goto _eofout; 
 if (s->dim > INT_MAX/s->entries) goto _eofout;

 /* codeword ordering.... length ordered or unordered? */
 switch((int)oggpack_read(opb,1)){
 case 0:
 /* unordered */
    lengthlist=(char *)calloc(s->entries, sizeof(*lengthlist));
 if(!lengthlist) goto _eofout;

 /* allocated but unused entries? */
 if(oggpack_read(opb,1)){

       /* yes, unused entries */
 
       for(i=0;i<s->entries;i++){
	if(oggpack_read(opb,1)){
	  long num=oggpack_read(opb,5);
	  if(num==-1)goto _eofout;
	  lengthlist[i]=(char)(num+1);
	  s->used_entries++;
	  if(num+1>s->dec_maxlength)s->dec_maxlength=num+1;
	}else
	  lengthlist[i]=0;
       }
     }else{
       /* all entries used; no tagging */
       s->used_entries=s->entries;
       for(i=0;i<s->entries;i++){
	long num=oggpack_read(opb,5);
	if(num==-1)goto _eofout;
	lengthlist[i]=(char)(num+1);
	if(num+1>s->dec_maxlength)s->dec_maxlength=num+1;
       }
     }
 
 break;
 case 1:
 /* ordered */
 {
 long length=oggpack_read(opb,5)+1;

      s->used_entries=s->entries;
      lengthlist=(char *)calloc(s->entries, sizeof(*lengthlist));

       if (!lengthlist) goto _eofout;
 
       for(i=0;i<s->entries;){
	long num=oggpack_read(opb,_ilog(s->entries-i));
	if(num<0)goto _eofout;
	for(j=0;j<num && i<s->entries;j++,i++)
	  lengthlist[i]=(char)length;
	s->dec_maxlength=length;
	length++;
       }
     }
     break;
 default:
 /* EOF */
 goto _eofout;
 }


 /* Do we have a mapping to unpack? */

 if((maptype=oggpack_read(opb,4))>0){
    s->q_min=_float32_unpack(oggpack_read(opb,32),&s->q_minp);
    s->q_del=_float32_unpack(oggpack_read(opb,32),&s->q_delp);
    s->q_bits=oggpack_read(opb,4)+1;
    s->q_seq=oggpack_read(opb,1);

    s->q_del>>=s->q_bits;
    s->q_delp+=s->q_bits;
 }

 switch(maptype){
 case 0:

 /* no mapping; decode type 0 */

 /* how many bytes for the indexing? */
 /* this is the correct boundary here; we lose one bit to
       node/leaf mark */
    s->dec_nodeb=_determine_node_bytes(s->used_entries,_ilog(s->entries)/8+1);
    s->dec_leafw=_determine_leaf_words(s->dec_nodeb,_ilog(s->entries)/8+1);
    s->dec_type=0;

 if(_make_decode_table(s,lengthlist,quantvals,opb,maptype)) goto _errout;
 break;

 case 1:

 /* mapping type 1; implicit values by lattice  position */
    quantvals=_book_maptype1_quantvals(s);

 /* dec_type choices here are 1,2; 3 doesn't make sense */
 {
 /* packed values */
 long total1=(s->q_bits*s->dim+8)/8; /* remember flag bit */
 if (s->dim > (INT_MAX-8)/s->q_bits) goto _eofout;
 /* vector of column offsets; remember flag bit */
 long total2=(_ilog(quantvals-1)*s->dim+8)/8+(s->q_bits+7)/8;

 
 
       if(total1<=4 && total1<=total2){
	/* use dec_type 1: vector of packed values */
 
	/* need quantized values before  */
	s->q_val=calloc(sizeof(ogg_uint16_t), quantvals);
	if (!s->q_val) goto _eofout;
	for(i=0;i<quantvals;i++)
	  ((ogg_uint16_t *)s->q_val)[i]=(ogg_uint16_t)oggpack_read(opb,s->q_bits);
 
	if(oggpack_eop(opb)){
	  goto _eofout;
	}
 
	s->dec_type=1;
	s->dec_nodeb=_determine_node_bytes(s->used_entries,
					   (s->q_bits*s->dim+8)/8);
	s->dec_leafw=_determine_leaf_words(s->dec_nodeb,
					   (s->q_bits*s->dim+8)/8);
	if(_make_decode_table(s,lengthlist,quantvals,opb,maptype)){
	  goto _errout;
	}
 
	free(s->q_val);
	s->q_val=0;
 
       }else{
	/* use dec_type 2: packed vector of column offsets */
 
	/* need quantized values before */
	if(s->q_bits<=8){
	  s->q_val=_ogg_malloc(quantvals);
	  if (!s->q_val) goto _eofout;
	  for(i=0;i<quantvals;i++)
	    ((unsigned char *)s->q_val)[i]=(unsigned char)oggpack_read(opb,s->q_bits);
	}else{
	  s->q_val=_ogg_malloc(quantvals*2);
	  if (!s->q_val) goto _eofout;
	  for(i=0;i<quantvals;i++)
	    ((ogg_uint16_t *)s->q_val)[i]=(ogg_uint16_t)oggpack_read(opb,s->q_bits);
	}
 
	if(oggpack_eop(opb))goto _eofout;
 
	s->q_pack=_ilog(quantvals-1);
	s->dec_type=2;
	s->dec_nodeb=_determine_node_bytes(s->used_entries,
					   (_ilog(quantvals-1)*s->dim+8)/8);
	s->dec_leafw=_determine_leaf_words(s->dec_nodeb,
					   (_ilog(quantvals-1)*s->dim+8)/8);
	if(_make_decode_table(s,lengthlist,quantvals,opb,maptype))goto _errout;
 
       }
     }
 break;
 case 2:

 /* mapping type 2; explicit array of values */
    quantvals=s->entries*s->dim;
 /* dec_type choices here are 1,3; 2 is not possible */

 if( (s->q_bits*s->dim+8)/8 <=4){ /* remember flag bit */
 /* use dec_type 1: vector of packed values */

      s->dec_type=1;
      s->dec_nodeb=_determine_node_bytes(s->used_entries,(s->q_bits*s->dim+8)/8);
      s->dec_leafw=_determine_leaf_words(s->dec_nodeb,(s->q_bits*s->dim+8)/8);
 if(_make_decode_table(s,lengthlist,quantvals,opb,maptype))goto _errout;

 }else{
 /* use dec_type 3: scalar offset into packed value array */

      s->dec_type=3;
      s->dec_nodeb=_determine_node_bytes(s->used_entries,_ilog(s->used_entries-1)/8+1);
      s->dec_leafw=_determine_leaf_words(s->dec_nodeb,_ilog(s->used_entries-1)/8+1);
 if(_make_decode_table(s,lengthlist,quantvals,opb,maptype))goto _errout;

 /* get the vals & pack them */
      s->q_pack=(s->q_bits+7)/8*s->dim;

       s->q_val=_ogg_malloc(s->q_pack*s->used_entries);
 
       if(s->q_bits<=8){
	for(i=0;i<s->used_entries*s->dim;i++)
	  ((unsigned char *)(s->q_val))[i]=(unsigned char)oggpack_read(opb,s->q_bits);
       }else{
	for(i=0;i<s->used_entries*s->dim;i++)
	  ((ogg_uint16_t *)(s->q_val))[i]=(ogg_uint16_t)oggpack_read(opb,s->q_bits);
       }
     }
     break;
 default:
 goto _errout;
 }

 if (s->dec_nodeb==1)
 if (s->dec_leafw == 1)
      s->dec_method = 0;
 else
      s->dec_method = 1;
 else if (s->dec_nodeb==2)
 if (s->dec_leafw == 1)
      s->dec_method = 2;
 else
      s->dec_method = 3;
 else
    s->dec_method = 4;

 if(oggpack_eop(opb))goto _eofout;

  free(lengthlist);
 return 0;
 _errout:
 _eofout:
  vorbis_book_clear(s);
  free(lengthlist);
  free(s->q_val);
 return -1;
}
