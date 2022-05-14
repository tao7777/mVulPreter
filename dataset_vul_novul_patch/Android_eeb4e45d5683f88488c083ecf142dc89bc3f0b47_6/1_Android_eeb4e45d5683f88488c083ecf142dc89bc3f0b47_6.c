 long vorbis_book_decodevs_add(codebook *book,ogg_int32_t *a,
			      oggpack_buffer *b,int n,int point){
   if(book->used_entries>0){
     int step=n/book->dim;
     ogg_int32_t *v = book->dec_buf;//(ogg_int32_t *)alloca(sizeof(*v)*book->dim);
 int i,j,o;
 if (!v) return -1;


     for (j=0;j<step;j++){
       if(decode_map(book,b,v,point))return -1;
       for(i=0,o=j;i<book->dim;i++,o+=step)
	a[o]+=v[i];
     }
   }
   return 0;
 }
