static const char *parse_number( cJSON *item, const char *num )
{
	int64_t i = 0;
	double f = 0;
	int isint = 1;
	int sign = 1, scale = 0, subscale = 0, signsubscale = 1;
 
	/* Could use sscanf for this? */
	if ( *num == '-' ) {
		/* Has sign. */
		sign = -1;
		++num;
	}
	if ( *num == '0' )
		/* Is zero. */
		++num;
	if ( *num >= '1' && *num<='9' ) {
		/* Number. */
		do {
			i = ( i * 10 ) + ( *num - '0' );
			f = ( f * 10.0 ) + ( *num - '0' );
			++num;
		} while ( *num >= '0' && *num <= '9' );
	}
	if ( *num == '.' && num[1] >= '0' && num[1] <= '9' ) {
		/* Fractional part. */
		isint = 0;
		++num;
		do {
			f = ( f * 10.0 ) + ( *num++ - '0' );
			scale--;
		} while ( *num >= '0' && *num <= '9' );
	}
	if ( *num == 'e' || *num == 'E' ) {
		/* Exponent. */
		isint = 0;
		++num;
		if ( *num == '+' )
			++num;
		else if ( *num == '-' ) {
			/* With sign. */
			signsubscale = -1;
			++num;
		}
		while ( *num >= '0' && *num <= '9' )
			subscale = ( subscale * 10 ) + ( *num++ - '0' );
	}
 
	/* Put it together. */
	if ( isint ) {
		/* Int: number = +/- number */
		i = sign * i;
		item->valueint = i;
		item->valuefloat = i;
	} else {
		/* Float: number = +/- number.fraction * 10^+/- exponent */
		f = sign * f * ipow( 10.0, scale + subscale * signsubscale );
		item->valueint = f;
		item->valuefloat = f;
	}
 
	item->type = cJSON_Number;
	return num;
 }
