int MSG_ReadBits( msg_t *msg, int bits ) {
	int			value;
	int			get;
	qboolean	sgn;
 	int			i, nbits;
 
	if ( msg->readcount > msg->cursize ) {
		return 0;
	}

 	value = 0;
 
 	if ( bits < 0 ) {
		bits = -bits;
		sgn = qtrue;
	} else {
		sgn = qfalse;
 	}
 
 	if (msg->oob) {
		if (msg->readcount + (bits>>3) > msg->cursize) {
			msg->readcount = msg->cursize + 1;
			return 0;
		}

 		if(bits==8)
 		{
 			value = msg->data[msg->readcount];
			msg->readcount += 1;
			msg->bit += 8;
		}
		else if(bits==16)
		{
			short temp;
			
			CopyLittleShort(&temp, &msg->data[msg->readcount]);
			value = temp;
			msg->readcount += 2;
			msg->bit += 16;
		}
		else if(bits==32)
		{
			CopyLittleLong(&value, &msg->data[msg->readcount]);
			msg->readcount += 4;
			msg->bit += 32;
		}
		else
			Com_Error(ERR_DROP, "can't read %d bits", bits);
	} else {
 		nbits = 0;
 		if (bits&7) {
 			nbits = bits&7;
			if (msg->bit + nbits > msg->cursize << 3) {
				msg->readcount = msg->cursize + 1;
				return 0;
			}
 			for(i=0;i<nbits;i++) {
 				value |= (Huff_getBit(msg->data, &msg->bit)<<i);
 			}
			bits = bits - nbits;
		}
 		if (bits) {
 			for(i=0;i<bits;i+=8) {
				Huff_offsetReceive (msgHuff.decompressor.tree, &get, msg->data, &msg->bit, msg->cursize<<3);
 				value |= (get<<(i+nbits));

				if (msg->bit > msg->cursize<<3) {
					msg->readcount = msg->cursize + 1;
					return 0;
				}
 			}
 		}
		msg->readcount = (msg->bit>>3)+1;
	}
	if ( sgn && bits > 0 && bits < 32 ) {
		if ( value & ( 1 << ( bits - 1 ) ) ) {
			value |= -1 ^ ( ( 1 << bits ) - 1 );
		}
	}

	return value;
}
