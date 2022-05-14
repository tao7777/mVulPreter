void Huff_offsetTransmit (huff_t *huff, int ch, byte *fout, int *offset) {
 	bloc = *offset;
	send(huff->loc[ch], NULL, fout);
 	*offset = bloc;
 }
