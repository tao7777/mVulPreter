 static int _make_decode_table(codebook *s,char *lengthlist,long quantvals,
                              oggpack_buffer *opb,int maptype){
   int i;
   ogg_uint32_t *work;
 
 if (!lengthlist) return 1;
 if(s->dec_nodeb==4){
 /* Over-allocate by using s->entries instead of used_entries.
     * This means that we can use s->entries to enforce size in
     * _make_words without messing up length list looping.
     * This probably wastes a bit of space, but it shouldn't
     * impact behavior or size too much.
     */
    s->dec_table=_ogg_malloc((s->entries*2+1)*sizeof(*work));
 if (!s->dec_table) return 1;

     /* +1 (rather than -2) is to accommodate 0 and 1 sized books,
        which are specialcased to nodeb==4 */
     if(_make_words(lengthlist,s->entries,
                   s->dec_table,quantvals,s,opb,maptype))return 1;
 
     return 0;
   }

 if (s->used_entries > INT_MAX/2 ||
      s->used_entries*2 > INT_MAX/((long) sizeof(*work)) - 1) return 1;
 /* Overallocate as above */
  work=calloc((s->entries*2+1),sizeof(*work));
 if (!work) return 1;
 if(_make_words(lengthlist,s->entries,work,quantvals,s,opb,maptype)) goto error_out;

   if (s->used_entries > INT_MAX/(s->dec_leafw+1)) goto error_out;
   if (s->dec_nodeb && s->used_entries * (s->dec_leafw+1) > INT_MAX/s->dec_nodeb) goto error_out;
   s->dec_table=_ogg_malloc((s->used_entries*(s->dec_leafw+1)-2)*
                           s->dec_nodeb);
   if (!s->dec_table) goto error_out;
 
   if(s->dec_leafw==1){
     switch(s->dec_nodeb){
     case 1:
       for(i=0;i<s->used_entries*2-2;i++)
          ((unsigned char *)s->dec_table)[i]=(unsigned char)
            (((work[i] & 0x80000000UL) >> 24) | work[i]);
       break;
     case 2:
       for(i=0;i<s->used_entries*2-2;i++)
          ((ogg_uint16_t *)s->dec_table)[i]=(ogg_uint16_t)
            (((work[i] & 0x80000000UL) >> 16) | work[i]);
       break;
     }
 
 }else{
 /* more complex; we have to do a two-pass repack that updates the
       node indexing. */
 long top=s->used_entries*3-2;
 if(s->dec_nodeb==1){

       unsigned char *out=(unsigned char *)s->dec_table;
 
       for(i=s->used_entries*2-4;i>=0;i-=2){
        if(work[i]&0x80000000UL){
          if(work[i+1]&0x80000000UL){
            top-=4;
            out[top]=(work[i]>>8 & 0x7f)|0x80;
            out[top+1]=(work[i+1]>>8 & 0x7f)|0x80;
            out[top+2]=work[i] & 0xff;
            out[top+3]=work[i+1] & 0xff;
          }else{
            top-=3;
            out[top]=(work[i]>>8 & 0x7f)|0x80;
            out[top+1]=work[work[i+1]*2];
            out[top+2]=work[i] & 0xff;
          }
        }else{
          if(work[i+1]&0x80000000UL){
            top-=3;
            out[top]=work[work[i]*2];
            out[top+1]=(work[i+1]>>8 & 0x7f)|0x80;
            out[top+2]=work[i+1] & 0xff;
          }else{
            top-=2;
            out[top]=work[work[i]*2];
            out[top+1]=work[work[i+1]*2];
          }
        }
        work[i]=top;
       }
     }else{
       ogg_uint16_t *out=(ogg_uint16_t *)s->dec_table;
       for(i=s->used_entries*2-4;i>=0;i-=2){
        if(work[i]&0x80000000UL){
          if(work[i+1]&0x80000000UL){
            top-=4;
            out[top]=(work[i]>>16 & 0x7fff)|0x8000;
            out[top+1]=(work[i+1]>>16 & 0x7fff)|0x8000;
            out[top+2]=work[i] & 0xffff;
            out[top+3]=work[i+1] & 0xffff;
          }else{
            top-=3;
            out[top]=(work[i]>>16 & 0x7fff)|0x8000;
            out[top+1]=work[work[i+1]*2];
            out[top+2]=work[i] & 0xffff;
          }
        }else{
          if(work[i+1]&0x80000000UL){
            top-=3;
            out[top]=work[work[i]*2];
            out[top+1]=(work[i+1]>>16 & 0x7fff)|0x8000;
            out[top+2]=work[i+1] & 0xffff;
          }else{
            top-=2;
            out[top]=work[work[i]*2];
            out[top+1]=work[work[i+1]*2];
          }
        }
        work[i]=top;
       }
     }
   }

  free(work);
 return 0;
error_out:
  free(work);
 return 1;
}
