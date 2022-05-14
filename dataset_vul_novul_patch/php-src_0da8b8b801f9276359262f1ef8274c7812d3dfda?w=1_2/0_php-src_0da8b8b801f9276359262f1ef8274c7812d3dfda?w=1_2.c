static inline void find_entity_for_char(
	unsigned int k,
	enum entity_charset charset,
	const entity_stage1_row *table,
	const unsigned char **entity,
	size_t *entity_len,
	unsigned char *old,
	size_t oldlen,
	size_t *cursor)
 {
 	unsigned stage1_idx = ENT_STAGE1_INDEX(k);
 	const entity_stage3_row *c;

 	if (stage1_idx > 0x1D) {
 		*entity     = NULL;
 		*entity_len = 0;
		return;
	}

	c = &table[stage1_idx][ENT_STAGE2_INDEX(k)][ENT_STAGE3_INDEX(k)];

	if (!c->ambiguous) {
		*entity     = (const unsigned char *)c->data.ent.entity;
		*entity_len = c->data.ent.entity_len;
	} else {
		/* peek at next char */
		size_t	 cursor_before	= *cursor;
		int		 status			= SUCCESS;
		unsigned next_char;

 		if (!(*cursor < oldlen))
 			goto no_suitable_2nd;
 
		next_char = get_next_char(charset, old, oldlen, cursor, &status);
 
 		if (status == FAILURE)
 			goto no_suitable_2nd;

		{
			const entity_multicodepoint_row *s, *e;

			s = &c->data.multicodepoint_table[1];
			e = s - 1 + c->data.multicodepoint_table[0].leading_entry.size;
			/* we could do a binary search but it's not worth it since we have
			 * at most two entries... */
			for ( ; s <= e; s++) {
				if (s->normal_entry.second_cp == next_char) {
					*entity     = s->normal_entry.entity;
					*entity_len = s->normal_entry.entity_len;
					return;
				}
			}
		}
no_suitable_2nd:
		*cursor = cursor_before;
 		*entity = (const unsigned char *)
 			c->data.multicodepoint_table[0].leading_entry.default_entity;
 		*entity_len = c->data.multicodepoint_table[0].leading_entry.default_entity_len;
	}
 }
