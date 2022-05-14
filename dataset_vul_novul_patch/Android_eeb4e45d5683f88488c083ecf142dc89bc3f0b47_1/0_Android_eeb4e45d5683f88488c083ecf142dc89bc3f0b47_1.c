static int decode_map(codebook *s, oggpack_buffer *b, ogg_int32_t *v, int point){
 ogg_uint32_t entry = decode_packed_entry_number(s,b);
 int i;
 if(oggpack_eop(b))return(-1);

 /* 1 used by test file 0 */

 /* according to decode type */
 switch(s->dec_type){
 case 1:{
 /* packed vector of values */
 int mask=(1<<s->q_bits)-1;
 for(i=0;i<s->dim;i++){
      v[i]=entry&mask;
      entry>>=s->q_bits;
 }
 break;
 }
 case 2:{
 /* packed vector of column offsets */

     int mask=(1<<s->q_pack)-1;
     for(i=0;i<s->dim;i++){
       if(s->q_bits<=8)
        v[i]=((unsigned char *)(s->q_val))[entry&mask];
       else
        v[i]=((ogg_uint16_t *)(s->q_val))[entry&mask];
       entry>>=s->q_pack;
     }
     break;
 }
 case 3:{
 /* offset into array */
 void *ptr=((char *)s->q_val)+entry*s->q_pack;

 
     if(s->q_bits<=8){
       for(i=0;i<s->dim;i++)
        v[i]=((unsigned char *)ptr)[i];
     }else{
       for(i=0;i<s->dim;i++)
        v[i]=((ogg_uint16_t *)ptr)[i];
     }
     break;
   }
 default:
 return -1;
 }

 /* we have the unpacked multiplicands; compute final vals */
 {
 int         shiftM = point-s->q_delp;
 ogg_int32_t add    = point-s->q_minp;
 int         mul    = s->q_del;

 if(add>0)
      add= s->q_min >> add;
 else
      add= s->q_min << -add;
 if (shiftM<0)
 {
      mul <<= -shiftM;
      shiftM = 0;
 }
    add <<= shiftM;

 for(i=0;i<s->dim;i++)
      v[i]= ((add + v[i] * mul) >> shiftM);

 
     if(s->q_seq)
       for(i=1;i<s->dim;i++)
        v[i]+=v[i-1];
   }
 
   return 0;
}
