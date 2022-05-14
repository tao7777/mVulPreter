varbit_in(PG_FUNCTION_ARGS)
{
	char	   *input_string = PG_GETARG_CSTRING(0);

#ifdef NOT_USED
	Oid			typelem = PG_GETARG_OID(1);
#endif
	int32		atttypmod = PG_GETARG_INT32(2);
	VarBit	   *result;			/* The resulting bit string			  */
	char	   *sp;				/* pointer into the character string  */
	bits8	   *r;				/* pointer into the result */
	int			len,			/* Length of the whole data structure */
				bitlen,			/* Number of bits in the bit string   */
				slen;			/* Length of the input string		  */
	bool		bit_not_hex;	/* false = hex string  true = bit string */
	int			bc;
	bits8		x = 0;

	/* Check that the first character is a b or an x */
	if (input_string[0] == 'b' || input_string[0] == 'B')
	{
		bit_not_hex = true;
		sp = input_string + 1;
	}
	else if (input_string[0] == 'x' || input_string[0] == 'X')
	{
		bit_not_hex = false;
		sp = input_string + 1;
	}
	else
	{
		bit_not_hex = true;
 		sp = input_string;
 	}
 
	/*
	 * Determine bitlength from input string.  MaxAllocSize ensures a regular
	 * input is small enough, but we must check hex input.
	 */
 	slen = strlen(sp);
 	if (bit_not_hex)
 		bitlen = slen;
 	else
	{
		if (slen > VARBITMAXLEN / 4)
			ereport(ERROR,
					(errcode(ERRCODE_PROGRAM_LIMIT_EXCEEDED),
				 errmsg("bit string length exceeds the maximum allowed (%d)",
						VARBITMAXLEN)));
 		bitlen = slen * 4;
	}
 
 	/*
 	 * Sometimes atttypmod is not supplied. If it is supplied we need to make
	 * sure that the bitstring fits.
	 */
	if (atttypmod <= 0)
		atttypmod = bitlen;
	else if (bitlen > atttypmod)
		ereport(ERROR,
				(errcode(ERRCODE_STRING_DATA_RIGHT_TRUNCATION),
				 errmsg("bit string too long for type bit varying(%d)",
						atttypmod)));

	len = VARBITTOTALLEN(bitlen);
	/* set to 0 so that *r is always initialised and string is zero-padded */
	result = (VarBit *) palloc0(len);
	SET_VARSIZE(result, len);
	VARBITLEN(result) = Min(bitlen, atttypmod);

	r = VARBITS(result);
	if (bit_not_hex)
	{
		/* Parse the bit representation of the string */
		/* We know it fits, as bitlen was compared to atttypmod */
		x = HIGHBIT;
		for (; *sp; sp++)
		{
			if (*sp == '1')
				*r |= x;
			else if (*sp != '0')
				ereport(ERROR,
						(errcode(ERRCODE_INVALID_TEXT_REPRESENTATION),
						 errmsg("\"%c\" is not a valid binary digit",
								*sp)));

			x >>= 1;
			if (x == 0)
			{
				x = HIGHBIT;
				r++;
			}
		}
	}
	else
	{
		/* Parse the hex representation of the string */
		for (bc = 0; *sp; sp++)
		{
			if (*sp >= '0' && *sp <= '9')
				x = (bits8) (*sp - '0');
			else if (*sp >= 'A' && *sp <= 'F')
				x = (bits8) (*sp - 'A') + 10;
			else if (*sp >= 'a' && *sp <= 'f')
				x = (bits8) (*sp - 'a') + 10;
			else
				ereport(ERROR,
						(errcode(ERRCODE_INVALID_TEXT_REPRESENTATION),
						 errmsg("\"%c\" is not a valid hexadecimal digit",
								*sp)));

			if (bc)
			{
				*r++ |= x;
				bc = 0;
			}
			else
			{
				*r = x << 4;
				bc = 1;
			}
		}
	}

	PG_RETURN_VARBIT_P(result);
}
