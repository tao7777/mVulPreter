void Huff_Compress(msg_t *mbuf, int offset) {
	int			i, ch, size;
	byte		seq[65536];
	byte*		buffer;
	huff_t		huff;

	size = mbuf->cursize - offset;
	buffer = mbuf->data+ + offset;

	if (size<=0) {
		return;
	}

	Com_Memset(&huff, 0, sizeof(huff_t));
	huff.tree = huff.lhead = huff.loc[NYT] =  &(huff.nodeList[huff.blocNode++]);
	huff.tree->symbol = NYT;
	huff.tree->weight = 0;
	huff.lhead->next = huff.lhead->prev = NULL;
	huff.tree->parent = huff.tree->left = huff.tree->right = NULL;

	seq[0] = (size>>8);
	seq[1] = size&0xff;

	bloc = 16;
 
 	for (i=0; i<size; i++ ) {
 		ch = buffer[i];
		Huff_transmit(&huff, ch, seq, size<<3);						/* Transmit symbol */
 		Huff_addRef(&huff, (byte)ch);								/* Do update */
 	}
 
	bloc += 8;												// next byte

	mbuf->cursize = (bloc>>3) + offset;
	Com_Memcpy(mbuf->data+offset, seq, (bloc>>3));
}
