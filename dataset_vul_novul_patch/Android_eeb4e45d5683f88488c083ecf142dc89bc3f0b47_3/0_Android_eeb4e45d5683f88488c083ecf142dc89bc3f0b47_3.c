 static ogg_uint32_t decpack(long entry,long used_entry,long quantvals,
                            codebook *b,oggpack_buffer *opb,int maptype){
   ogg_uint32_t ret=0;
   int j;
 
 switch(b->dec_type){

 case 0:
 return (ogg_uint32_t)entry;

 case 1:

     if(maptype==1){
       /* vals are already read into temporary column vector here */
       for(j=0;j<b->dim;j++){
        ogg_uint32_t off=entry%quantvals;
        entry/=quantvals;
        ret|=((ogg_uint16_t *)(b->q_val))[off]<<(b->q_bits*j);
       }
     }else{
       for(j=0;j<b->dim;j++)
        ret|=oggpack_read(opb,b->q_bits)<<(b->q_bits*j);
     }
     return ret;
 
 case 2:
 for(j=0;j<b->dim;j++){
 ogg_uint32_t off=entry%quantvals;
      entry/=quantvals;
      ret|=off<<(b->q_pack*j);
 }
 return ret;

 case 3:
 return (ogg_uint32_t)used_entry;

 }
 return 0; /* silence compiler */
}
