 int test_sqr(BIO *bp, BN_CTX *ctx)
 	{
	BIGNUM a,c,d,e;
	int i;
 
	BN_init(&a);
	BN_init(&c);
	BN_init(&d);
	BN_init(&e);
 
 	for (i=0; i<num0; i++)
 		{
		BN_bntest_rand(&a,40+i*10,0,0);
		a.neg=rand_neg();
		BN_sqr(&c,&a,ctx);
 		if (bp != NULL)
 			{
 			if (!results)
 				{
				BN_print(bp,&a);
 				BIO_puts(bp," * ");
				BN_print(bp,&a);
 				BIO_puts(bp," - ");
 				}
			BN_print(bp,&c);
 			BIO_puts(bp,"\n");
 			}
		BN_div(&d,&e,&c,&a,ctx);
		BN_sub(&d,&d,&a);
		if(!BN_is_zero(&d) || !BN_is_zero(&e))
		    {
		    fprintf(stderr,"Square test failed!\n");
		    return 0;
		    }
 		}
	BN_free(&a);
	BN_free(&c);
	BN_free(&d);
	BN_free(&e);
	return(1);
 	}
