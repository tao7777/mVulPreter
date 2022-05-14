void Huff_offsetReceive (node_t *node, int *ch, byte *fin, int *offset) {
 	bloc = *offset;
 	while (node && node->symbol == INTERNAL_NODE) {
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
