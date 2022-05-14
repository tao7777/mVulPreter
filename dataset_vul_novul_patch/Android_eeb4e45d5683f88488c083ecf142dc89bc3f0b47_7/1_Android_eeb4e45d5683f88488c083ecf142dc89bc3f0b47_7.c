 long vorbis_book_decodevv_add(codebook *book,ogg_int32_t **a,
			      long offset,int ch,
			      oggpack_buffer *b,int n,int point){
   if(book->used_entries>0){
 
     ogg_int32_t *v = book->dec_buf;//(ogg_int32_t *)alloca(sizeof(*v)*book->dim);
 long i,j;
 int chptr=0;


     if (!v) return -1;
     for(i=offset;i<offset+n;){
       if(decode_map(book,b,v,point))return -1;
      for (j=0;j<book->dim;j++){
	a[chptr++][i]+=v[j];
	if(chptr==ch){
	  chptr=0;
	  i++;
	}
       }
     }
   }

 return 0;
}
