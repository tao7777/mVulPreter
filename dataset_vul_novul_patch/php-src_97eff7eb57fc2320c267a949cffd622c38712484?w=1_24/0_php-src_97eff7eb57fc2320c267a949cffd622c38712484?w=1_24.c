static char* get_private_subtags(const char* loc_name)
 {
 	char* 	result =NULL;
 	int 	singletonPos = 0;
	int 	len =0;
 	const char* 	mod_loc_name =NULL;
 
 	if( loc_name && (len = strlen(loc_name)>0 ) ){
		mod_loc_name = loc_name ;
 		len   = strlen(mod_loc_name);
 		while( (singletonPos = getSingletonPos(mod_loc_name))!= -1){
 
			if( singletonPos!=-1){
				if( (*(mod_loc_name+singletonPos)=='x') || (*(mod_loc_name+singletonPos)=='X') ){
 					/* private subtag start found */
 					if( singletonPos + 2 ==  len){
 						/* loc_name ends with '-x-' ; return  NULL */
					}
					else{
						/* result = mod_loc_name + singletonPos +2; */
						result = estrndup(mod_loc_name + singletonPos+2  , (len -( singletonPos +2) ) );
					}
					break;
				}
				else{
					if( singletonPos + 1 >=  len){
						/* String end */
						break;
					} else {
						/* singleton found but not a private subtag , hence check further in the string for the private subtag */
						mod_loc_name = mod_loc_name + singletonPos +1;
						len = strlen(mod_loc_name);
					}
				}
			}
 
 		} /* end of while */
 	}

 	return result;
 }
