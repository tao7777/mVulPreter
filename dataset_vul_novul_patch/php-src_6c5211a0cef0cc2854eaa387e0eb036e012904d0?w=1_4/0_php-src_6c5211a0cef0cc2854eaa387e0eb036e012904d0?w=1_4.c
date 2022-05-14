PHP_FUNCTION(mcrypt_enc_is_block_algorithm)
{
 	MCRYPT_GET_TD_ARG
 
 	if (mcrypt_enc_is_block_algorithm(pm->td) == 1) {
		RETURN_TRUE
 	} else {
 		RETURN_FALSE
 	}
}
