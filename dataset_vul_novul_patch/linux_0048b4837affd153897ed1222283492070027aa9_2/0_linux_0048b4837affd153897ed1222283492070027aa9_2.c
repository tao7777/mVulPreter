static void bt_tags_for_each(struct blk_mq_tags *tags,
		struct blk_mq_bitmap_tags *bt, unsigned int off,
		busy_tag_iter_fn *fn, void *data, bool reserved)
{
	struct request *rq;
	int bit, i;

	if (!tags->rqs)
		return;
	for (i = 0; i < bt->map_nr; i++) {
		struct blk_align_bitmap *bm = &bt->map[i];

 		for (bit = find_first_bit(&bm->word, bm->depth);
 		     bit < bm->depth;
 		     bit = find_next_bit(&bm->word, bm->depth, bit + 1)) {
			rq = tags->rqs[off + bit];
 			fn(rq, data, reserved);
 		}
 
		off += (1 << bt->bits_per_word);
	}
}
