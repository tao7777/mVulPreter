 int test_sqr(BIO *bp, BN_CTX *ctx)
 	{
	BIGNUM *a,*c,*d,*e;
	int i, ret = 0;
 
	a = BN_new();
	c = BN_new();
	d = BN_new();
	e = BN_new();
	if (a == NULL || c == NULL || d == NULL || e == NULL)
		{
		goto err;
		}
 
 	for (i=0; i<num0; i++)
 		{
		BN_bntest_rand(a,40+i*10,0,0);
		a->neg=rand_neg();
		BN_sqr(c,a,ctx);
 		if (bp != NULL)
 			{
 			if (!results)
 				{
				BN_print(bp,a);
 				BIO_puts(bp," * ");
				BN_print(bp,a);
 				BIO_puts(bp," - ");
 				}
			BN_print(bp,c);
 			BIO_puts(bp,"\n");
 			}
		BN_div(d,e,c,a,ctx);
		BN_sub(d,d,a);
		if(!BN_is_zero(d) || !BN_is_zero(e))
			{
			fprintf(stderr,"Square test failed!\n");
			goto err;
			}
 		}

	/* Regression test for a BN_sqr overflow bug. */
	BN_hex2bn(&a,
		"80000000000000008000000000000001FFFFFFFFFFFFFFFE0000000000000000");
	BN_sqr(c, a, ctx);
	if (bp != NULL)
		{
		if (!results)
			{
			BN_print(bp,a);
			BIO_puts(bp," * ");
			BN_print(bp,a);
			BIO_puts(bp," - ");
			}
		BN_print(bp,c);
		BIO_puts(bp,"\n");
		}
	BN_mul(d, a, a, ctx);
	if (BN_cmp(c, d))
		{
		fprintf(stderr, "Square test failed: BN_sqr and BN_mul produce "
			"different results!\n");
		goto err;
		}

	/* Regression test for a BN_sqr overflow bug. */
	BN_hex2bn(&a,
		"80000000000000000000000080000001FFFFFFFE000000000000000000000000");
	BN_sqr(c, a, ctx);
	if (bp != NULL)
		{
		if (!results)
			{
			BN_print(bp,a);
			BIO_puts(bp," * ");
			BN_print(bp,a);
			BIO_puts(bp," - ");
			}
		BN_print(bp,c);
		BIO_puts(bp,"\n");
		}
	BN_mul(d, a, a, ctx);
	if (BN_cmp(c, d))
		{
		fprintf(stderr, "Square test failed: BN_sqr and BN_mul produce "
			"different results!\n");
		goto err;
		}
	ret = 1;
err:
	if (a != NULL) BN_free(a);
	if (c != NULL) BN_free(c);
	if (d != NULL) BN_free(d);
	if (e != NULL) BN_free(e);
	return ret;
 	}
