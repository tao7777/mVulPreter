static void send(node_t *node, node_t *child, byte *fout) {
 	if (node->parent) {
		send(node->parent, node, fout);
 	}
 	if (child) {
 		if (node->right == child) {
 			add_bit(1, fout);
 		} else {
			add_bit(0, fout);
		}
	}
 }
