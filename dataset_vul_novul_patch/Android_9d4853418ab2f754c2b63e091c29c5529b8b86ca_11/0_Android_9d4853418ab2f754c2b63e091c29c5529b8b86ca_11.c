main(void)
{

    fwrite(signature, sizeof signature, 1, stdout);
    put_chunk(IHDR, sizeof IHDR);
 
   for (;;)
       put_chunk(unknown, sizeof unknown);
 }
