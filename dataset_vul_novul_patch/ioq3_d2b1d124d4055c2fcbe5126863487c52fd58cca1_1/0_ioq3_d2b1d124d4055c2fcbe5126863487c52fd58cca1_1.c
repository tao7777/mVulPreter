void Huff_offsetTransmit (huff_t *huff, int ch, byte *fout, int *offset) {
void Huff_offsetTransmit (huff_t *huff, int ch, byte *fout, int *offset, int maxoffset) {
 	bloc = *offset;
	send(huff->loc[ch], NULL, fout, maxoffset);
 	*offset = bloc;
 }
