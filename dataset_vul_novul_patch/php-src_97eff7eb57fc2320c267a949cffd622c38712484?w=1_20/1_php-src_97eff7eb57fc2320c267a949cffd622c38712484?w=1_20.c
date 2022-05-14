 static int getStrrtokenPos(char* str, int savedPos)
 {
 	int result =-1;
 	int i;
 	for(i=savedPos-1; i>=0; i--) {
 		if(isIDSeparator(*(str+i)) ){
 			/* delimiter found; check for singleton */
			if(i>=2 && isIDSeparator(*(str+i-2)) ){
				/* a singleton; so send the position of token before the singleton */
				result = i-2;
			} else {
				result = i;
			}
			break;
		}
	}
	if(result < 1){
		/* Just in case inavlid locale e.g. '-x-xyz' or '-sl_Latn' */
		result =-1;
	}
	return result;
}
