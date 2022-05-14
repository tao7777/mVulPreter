static double ipow( double n, int exp )
 {
	double r;
 
	if ( exp < 0 )
		return 1.0 / ipow( n, -exp );
	r = 1;
	while ( exp > 0 ) {
		if ( exp & 1 )
			r *= n;
		exp >>= 1;
		n *= n;
 	}
	return r;
}
