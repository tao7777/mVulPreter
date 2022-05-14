static int getSingletonPos(const char* str)
{
 	int result =-1;
 	int i=0;
 	int len = 0;
 	if( str && ((len=strlen(str))>0) ){
 		for( i=0; i<len ; i++){
 			if( isIDSeparator(*(str+i)) ){
				if( i==1){
					/* string is of the form x-avy or a-prv1 */
					result =0;
					break;
				} else {
					/* delimiter found; check for singleton */
					if( isIDSeparator(*(str+i+2)) ){
						/* a singleton; so send the position of separator before singleton */
						result = i+1;
						break;
					}
 				}
 			}
 		}/* end of for */
 	}
 	return result;
 }
