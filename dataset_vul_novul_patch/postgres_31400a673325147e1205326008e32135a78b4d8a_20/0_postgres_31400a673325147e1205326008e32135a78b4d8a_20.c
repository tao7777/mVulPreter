bit_catenate(VarBit *arg1, VarBit *arg2)
{
	VarBit	   *result;
	int			bitlen1,
				bitlen2,
				bytelen,
				bit1pad,
				bit2shift;
	bits8	   *pr,
			   *pa;

 	bitlen1 = VARBITLEN(arg1);
 	bitlen2 = VARBITLEN(arg2);
 
	if (bitlen1 > VARBITMAXLEN - bitlen2)
		ereport(ERROR,
				(errcode(ERRCODE_PROGRAM_LIMIT_EXCEEDED),
				 errmsg("bit string length exceeds the maximum allowed (%d)",
						VARBITMAXLEN)));
 	bytelen = VARBITTOTALLEN(bitlen1 + bitlen2);
 
 	result = (VarBit *) palloc(bytelen);
	SET_VARSIZE(result, bytelen);
	VARBITLEN(result) = bitlen1 + bitlen2;

	/* Copy the first bitstring in */
	memcpy(VARBITS(result), VARBITS(arg1), VARBITBYTES(arg1));

	/* Copy the second bit string */
	bit1pad = VARBITPAD(arg1);
	if (bit1pad == 0)
	{
		memcpy(VARBITS(result) + VARBITBYTES(arg1), VARBITS(arg2),
			   VARBITBYTES(arg2));
	}
	else if (bitlen2 > 0)
	{
		/* We need to shift all the bits to fit */
		bit2shift = BITS_PER_BYTE - bit1pad;
		pr = VARBITS(result) + VARBITBYTES(arg1) - 1;
		for (pa = VARBITS(arg2); pa < VARBITEND(arg2); pa++)
		{
			*pr |= ((*pa >> bit2shift) & BITMASK);
			pr++;
			if (pr < VARBITEND(result))
				*pr = (*pa << bit1pad) & BITMASK;
		}
	}

	return result;
}
