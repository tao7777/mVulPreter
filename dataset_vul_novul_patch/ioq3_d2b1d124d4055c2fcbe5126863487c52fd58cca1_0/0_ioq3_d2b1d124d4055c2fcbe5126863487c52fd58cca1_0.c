void Huff_offsetReceive (node_t *node, int *ch, byte *fin, int *offset) {
void Huff_offsetReceive (node_t *node, int *ch, byte *fin, int *offset, int maxoffset) {
 	bloc = *offset;
 	while (node && node->symbol == INTERNAL_NODE) {
		if (bloc >= maxoffset) {
			*ch = 0;
			*offset = maxoffset + 1;
			return;
		}
 		if (get_bit(fin)) {
 			node = node->right;
 		} else {
			node = node->left;
		}
	}
	if (!node) {
		*ch = 0;
		return;
	}
	*ch = node->symbol;
	*offset = bloc;
 }
