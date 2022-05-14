void sum_init(int csum_type, int seed)
{
	char s[4];

	if (csum_type < 0)
		csum_type = parse_csum_name(NULL, 0);
	cursum_type = csum_type;

	switch (csum_type) {
	  case CSUM_MD5:
		md5_begin(&md);
		break;
	  case CSUM_MD4:
		mdfour_begin(&md);
		sumresidue = 0;
		break;
	  case CSUM_MD4_OLD:
                break;
          case CSUM_MD4_OLD:
          case CSUM_MD4_BUSTED:
         case CSUM_MD4_ARCHAIC:
                mdfour_begin(&md);
                sumresidue = 0;
                SIVAL(s, 0, seed);
		break;
	}
}
