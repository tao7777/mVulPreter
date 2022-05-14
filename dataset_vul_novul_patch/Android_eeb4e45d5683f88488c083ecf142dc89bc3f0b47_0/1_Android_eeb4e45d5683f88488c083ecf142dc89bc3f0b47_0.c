 static int _make_words(char *l,long n,ogg_uint32_t *r,long quantvals,
		       codebook *b, oggpack_buffer *opb,int maptype){
   long i,j,count=0;
   long top=0;
   ogg_uint32_t marker[MARKER_SIZE];

 if (n<1)
 return 1;

 if(n<2){
    r[0]=0x80000000;
 }else{
    memset(marker,0,sizeof(marker));

 for(i=0;i<n;i++){
 long length=l[i];
 if(length){
 if (length < 0 || length >= MARKER_SIZE) {

           ALOGE("b/23881715");
           return 1;
         }
	ogg_uint32_t entry=marker[length];
	long chase=0;
	if(count && !entry)return -1; /* overpopulated tree! */
 
	/* chase the tree as far as it's already populated, fill in past */
	for(j=0;j<length-1;j++){
	  int bit=(entry>>(length-j-1))&1;
	  if(chase>=top){
	    if (chase < 0 || chase >= n) return 1;
	    top++;
	    r[chase*2]=top;
	    r[chase*2+1]=0;
	  }else
	    if (chase < 0 || chase >= n || chase*2+bit > n*2+1) return 1;
	    if(!r[chase*2+bit])
	      r[chase*2+bit]=top;
	  chase=r[chase*2+bit];
	  if (chase < 0 || chase >= n) return 1;
	}
	{
	  int bit=(entry>>(length-j-1))&1;
	  if(chase>=top){
	    top++;
	    r[chase*2+1]=0;
	  }
	  r[chase*2+bit]= decpack(i,count++,quantvals,b,opb,maptype) |
	    0x80000000;
	}
 
	/* Look to see if the next shorter marker points to the node
	   above. if so, update it and repeat.  */
	for(j=length;j>0;j--){
	  if(marker[j]&1){
	    marker[j]=marker[j-1]<<1;
	    break;
	  }
	  marker[j]++;
	}
 
	/* prune the tree; the implicit invariant says all the longer
	   markers were dangling from our just-taken node.  Dangle them
	   from our *new* node. */
	for(j=length+1;j<MARKER_SIZE;j++)
	  if((marker[j]>>1) == entry){
	    entry=marker[j];
	    marker[j]=marker[j-1]<<1;
	  }else
	    break;
       }
     }
   }

 /* sanity check the huffman tree; an underpopulated tree must be
     rejected. The only exception is the one-node pseudo-nil tree,
     which appears to be underpopulated because the tree doesn't
     really exist; there's only one possible 'codeword' or zero bits,
     but the above tree-gen code doesn't mark that. */
 if(b->used_entries != 1){
 for(i=1;i<MARKER_SIZE;i++)
 if(marker[i] & (0xffffffffUL>>(32-i))){
 return 1;
 }
 }


 return 0;

 }
