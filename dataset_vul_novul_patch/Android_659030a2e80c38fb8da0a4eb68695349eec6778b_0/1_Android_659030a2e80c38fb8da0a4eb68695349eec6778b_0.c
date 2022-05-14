 int res_unpack(vorbis_info_residue *info,
		vorbis_info *vi,oggpack_buffer *opb){
   int j,k;
   codec_setup_info     *ci=(codec_setup_info *)vi->codec_setup;
   memset(info,0,sizeof(*info));

  info->type=oggpack_read(opb,16);
 if(info->type>2 || info->type<0)goto errout;
  info->begin=oggpack_read(opb,24);
  info->end=oggpack_read(opb,24);
  info->grouping=oggpack_read(opb,24)+1;
  info->partitions=(char)(oggpack_read(opb,6)+1);
  info->groupbook=(unsigned char)oggpack_read(opb,8);
 if(info->groupbook>=ci->books)goto errout;

  info->stagemasks=_ogg_malloc(info->partitions*sizeof(*info->stagemasks));
  info->stagebooks=_ogg_malloc(info->partitions*8*sizeof(*info->stagebooks));

 for(j=0;j<info->partitions;j++){
 int cascade=oggpack_read(opb,3);
 if(oggpack_read(opb,1))
      cascade|=(oggpack_read(opb,5)<<3);
    info->stagemasks[j]=cascade;
 }


   for(j=0;j<info->partitions;j++){
     for(k=0;k<8;k++){
       if((info->stagemasks[j]>>k)&1){
	unsigned char book=(unsigned char)oggpack_read(opb,8);
	if(book>=ci->books)goto errout;
	info->stagebooks[j*8+k]=book;
	if(k+1>info->stages)info->stages=k+1;
       }else
	info->stagebooks[j*8+k]=0xff;
     }
   }
 
 if(oggpack_eop(opb))goto errout;

 return 0;
 errout:
  res_clear_info(info);
 return 1;

 }
