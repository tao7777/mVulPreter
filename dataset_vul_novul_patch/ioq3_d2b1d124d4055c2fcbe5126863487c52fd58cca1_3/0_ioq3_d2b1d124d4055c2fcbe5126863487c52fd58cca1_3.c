static void send(node_t *node, node_t *child, byte *fout) {
static void send(node_t *node, node_t *child, byte *fout, int maxoffset) {
 	if (node->parent) {
		send(node->parent, node, fout, maxoffset);
 	}
 	if (child) {
		if (bloc >= maxoffset) {
			bloc = maxoffset + 1;
			return;
		}
 		if (node->right == child) {
 			add_bit(1, fout);
 		} else {
			add_bit(0, fout);
		}
	}
 }
