static void parse_sec_attr_44(sc_file_t *file, const u8 *buf, size_t len)
{
	/* OpenSc Operation values for each command operation-type */
	const int df_idx[8] = {	 /* byte 1 = OpenSC type of AC Bit0,  byte 2 = OpenSC type of AC Bit1 ...*/
		SC_AC_OP_DELETE, SC_AC_OP_CREATE, SC_AC_OP_CREATE,
		SC_AC_OP_INVALIDATE, SC_AC_OP_REHABILITATE,
		SC_AC_OP_LOCK, SC_AC_OP_DELETE, -1};
	const int ef_idx[8] = {
		SC_AC_OP_READ, SC_AC_OP_UPDATE, SC_AC_OP_WRITE,
		SC_AC_OP_INVALIDATE, SC_AC_OP_REHABILITATE,
		-1, SC_AC_OP_ERASE, -1};
	const int efi_idx[8] = { /* internal EF used for RSA keys */
		SC_AC_OP_READ, SC_AC_OP_ERASE, SC_AC_OP_UPDATE,
		SC_AC_OP_INVALIDATE, SC_AC_OP_REHABILITATE,
		-1, SC_AC_OP_ERASE, -1};

	u8		bValue;
	int		i;
	int		iKeyRef = 0;
	int		iMethod;
	int		iPinCount;
	int		iOffset = 0;
	int		iOperation;
	const int*	p_idx;

 	/* Check all sub-AC definitions within the total AC */
 	while (len > 1) {				/* minimum length = 2 */
 		int	iACLen   = buf[iOffset] & 0x0F;
		if ((size_t) iACLen > len)
			break;
 
 		iPinCount = -1;			/* default no pin required */
 		iMethod = SC_AC_NONE;		/* default no authentication required */

		if (buf[iOffset] & 0X80) { /* AC in adaptive coding */
			/* Evaluates only the command-byte, not the optional P1/P2/Option bytes */
			int	iParmLen = 1;			/* command-byte is always present */
			int	iKeyLen  = 0;			/* Encryption key is optional */

			if (buf[iOffset]   & 0x20) iKeyLen++;
			if (buf[iOffset+1] & 0x40) iParmLen++;
			if (buf[iOffset+1] & 0x20) iParmLen++;
			if (buf[iOffset+1] & 0x10) iParmLen++;
			if (buf[iOffset+1] & 0x08) iParmLen++;
 
 			/* Get KeyNumber if available */
 			if(iKeyLen) {
				int iSC;
				if (len < 1+iACLen)
					break;
				iSC = buf[iOffset+iACLen];
 
 				switch( (iSC>>5) & 0x03 ){
 				case 0:
					iMethod = SC_AC_TERM;		/* key authentication */
					break;
				case 1:
					iMethod = SC_AC_AUT;		/* key authentication  */
					break;
				case 2:
				case 3:
					iMethod = SC_AC_PRO;		/* secure messaging */
					break;
				}
				iKeyRef = iSC & 0x1F;			/* get key number */
			}
 
 			/* Get PinNumber if available */
 			if (iACLen > (1+iParmLen+iKeyLen)) {  /* check via total length if pin is present */
				if (len < 1+1+1+iParmLen)
					break;
 				iKeyRef = buf[iOffset+1+1+iParmLen];  /* PTL + AM-header + parameter-bytes */
 				iMethod = SC_AC_CHV;
 			}
 
 			/* Convert SETCOS command to OpenSC command group */
			if (len < 1+2)
				break;
 			switch(buf[iOffset+2]){
 			case 0x2A:			/* crypto operation */
 				iOperation = SC_AC_OP_CRYPTO;
				break;
			case 0x46:			/* key-generation operation */
				iOperation = SC_AC_OP_UPDATE;
				break;
			default:
				iOperation = SC_AC_OP_SELECT;
				break;
			}
			sc_file_add_acl_entry(file, iOperation, iMethod, iKeyRef);
		}
		else { /* AC in simple coding */
			   /* Initial AC is treated as an operational AC */

			/* Get specific Cmd groups for specified file-type */
			switch (file->type) {
			case SC_FILE_TYPE_DF:            /* DF */
				p_idx = df_idx;
				break;
			case SC_FILE_TYPE_INTERNAL_EF:   /* EF for RSA keys */
				p_idx = efi_idx;
				break;
			default:                         /* EF */
				p_idx = ef_idx;
				break;
			}

			/* Encryption key present ? */
 			iPinCount = iACLen - 1;		
 
 			if (buf[iOffset] & 0x20) {
				int iSC;
				if (len < 1 + iACLen)
					break;
				iSC = buf[iOffset + iACLen];
 
 				switch( (iSC>>5) & 0x03 ) {
 				case 0:
					iMethod = SC_AC_TERM;		/* key authentication */
					break;
				case 1:
					iMethod = SC_AC_AUT;		/* key authentication  */
					break;
				case 2:
				case 3:
					iMethod = SC_AC_PRO;		/* secure messaging */
					break;
				}
				iKeyRef = iSC & 0x1F;			/* get key number */

				iPinCount--;				/* one byte used for keyReference  */
			}
 
 			/* Pin present ? */
 			if ( iPinCount > 0 ) {
				if (len < 1 + 2)
					break;
 				iKeyRef = buf[iOffset + 2];	/* pin ref */
 				iMethod = SC_AC_CHV;
 			}

			/* Add AC for each command-operationType into OpenSc structure */
			bValue = buf[iOffset + 1];
			for (i = 0; i < 8; i++) {
				if((bValue & 1) && (p_idx[i] >= 0))
					sc_file_add_acl_entry(file, p_idx[i], iMethod, iKeyRef);
				bValue >>= 1;
			}
		}
		/* Current field treated, get next AC sub-field */
		iOffset += iACLen +1;		/* AC + PTL-byte */
		len     -= iACLen +1;
	}
}
