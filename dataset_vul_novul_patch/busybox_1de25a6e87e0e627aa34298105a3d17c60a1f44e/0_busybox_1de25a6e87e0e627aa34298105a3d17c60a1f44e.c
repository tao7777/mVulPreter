static int huft_build(const unsigned *b, const unsigned n,
			const unsigned s, const unsigned short *d,
			const unsigned char *e, huft_t **t, unsigned *m)
{
	unsigned a;             /* counter for codes of length k */
	unsigned c[BMAX + 1];   /* bit length count table */
	unsigned eob_len;       /* length of end-of-block code (value 256) */
	unsigned f;             /* i repeats in table every f entries */
	int g;                  /* maximum code length */
	int htl;                /* table level */
 	unsigned i;             /* counter, current code */
 	unsigned j;             /* counter */
 	int k;                  /* number of bits in current code */
	const unsigned *p;      /* pointer into c[], b[], or v[] */
 	huft_t *q;              /* points to current table */
 	huft_t r;               /* table entry for structure assignment */
 	huft_t *u[BMAX];        /* table stack */
 	unsigned v[N_MAX];      /* values in order of bit length */
	unsigned v_end;
 	int ws[BMAX + 1];       /* bits decoded stack */
 	int w;                  /* bits decoded */
 	unsigned x[BMAX + 1];   /* bit offsets, then code stack */
	int y;                  /* number of dummy codes added */
	unsigned z;             /* number of entries in current table */

	/* Length of EOB code, if any */
	eob_len = n > 256 ? b[256] : BMAX;

	*t = NULL;

 
 	/* Generate counts for each bit length */
 	memset(c, 0, sizeof(c));
	p = b;
 	i = n;
 	do {
 		c[*p]++; /* assume all entries <= BMAX */
	} while (--i);
	if (c[0] == n) {  /* null input - all zero length codes */
		*m = 0;
		return 2;
	}

	/* Find minimum and maximum length, bound *m by those */
	for (j = 1; (j <= BMAX) && (c[j] == 0); j++)
		continue;
	k = j; /* minimum code length */
	for (i = BMAX; (c[i] == 0) && i; i--)
		continue;
	g = i; /* maximum code length */
	*m = (*m < j) ? j : ((*m > i) ? i : *m);

	/* Adjust last length count to fill out codes, if needed */
	for (y = 1 << j; j < i; j++, y <<= 1) {
		y -= c[j];
		if (y < 0)
			return 2; /* bad input: more codes than bits */
	}
	y -= c[i];
	if (y < 0)
		return 2;
	c[i] += y;

	/* Generate starting offsets into the value table for each length */
	x[1] = j = 0;
	p = c + 1;
	xp = x + 2;
	while (--i) { /* note that i == g from above */
		j += *p++;
		*xp++ = j;
	}
 	}
