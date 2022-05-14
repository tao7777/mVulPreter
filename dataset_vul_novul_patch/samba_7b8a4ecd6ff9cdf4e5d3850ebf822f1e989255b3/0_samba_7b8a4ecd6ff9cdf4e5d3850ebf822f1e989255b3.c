int csum_len_for_type(int cst)
{
        switch (cst) {
          case CSUM_NONE:
                return 1;
         case CSUM_MD4_ARCHAIC:
                return 2;
          case CSUM_MD4:
          case CSUM_MD4_OLD:
	  case CSUM_MD4_BUSTED:
		return MD4_DIGEST_LEN;
	  case CSUM_MD5:
		return MD5_DIGEST_LEN;
	}
	return 0;
}
