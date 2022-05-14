void Huff_transmit (huff_t *huff, int ch, byte *fout) {
 	int i;
 	if (huff->loc[ch] == NULL) { 
 		/* node_t hasn't been transmitted, send a NYT, then the symbol */
		Huff_transmit(huff, NYT, fout);
 		for (i = 7; i >= 0; i--) {
 			add_bit((char)((ch >> i) & 0x1), fout);
 		}
 	} else {
		send(huff->loc[ch], NULL, fout);
 	}
 }
