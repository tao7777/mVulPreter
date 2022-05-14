static inline void write_s3row_data(
	const entity_stage3_row *r,
	unsigned orig_cp,
	enum entity_charset charset,
	zval *arr)
{
	char key[9] = ""; /* two unicode code points in UTF-8 */
	char entity[LONGEST_ENTITY_LENGTH + 2] = {'&'};
	size_t written_k1;

	written_k1 = write_octet_sequence(key, charset, orig_cp);

	if (!r->ambiguous) {
		size_t l = r->data.ent.entity_len;
		memcpy(&entity[1], r->data.ent.entity, l);
		entity[l + 1] = ';';
		add_assoc_stringl_ex(arr, key, written_k1 + 1, entity, l + 2, 1);
	} else {
		unsigned i,
			     num_entries;
		const entity_multicodepoint_row *mcpr = r->data.multicodepoint_table;

		if (mcpr[0].leading_entry.default_entity != NULL) {
			size_t l = mcpr[0].leading_entry.default_entity_len;
			memcpy(&entity[1], mcpr[0].leading_entry.default_entity, l);
			entity[l + 1] = ';';
			add_assoc_stringl_ex(arr, key, written_k1 + 1, entity, l + 2, 1);
		}
		num_entries = mcpr[0].leading_entry.size;
		for (i = 1; i <= num_entries; i++) {
			size_t   l,
				     written_k2;
			unsigned uni_cp,
					 spe_cp;

			uni_cp = mcpr[i].normal_entry.second_cp;
			l = mcpr[i].normal_entry.entity_len;

			if (!CHARSET_UNICODE_COMPAT(charset)) {
				if (map_from_unicode(uni_cp, charset, &spe_cp) == FAILURE)
					continue; /* non representable in this charset */
 			} else {
 				spe_cp = uni_cp;
 			}
 			written_k2 = write_octet_sequence(&key[written_k1], charset, spe_cp);
 			memcpy(&entity[1], mcpr[i].normal_entry.entity, l);
 			entity[l + 1] = ';';
			entity[l + 1] = '\0';
			add_assoc_stringl_ex(arr, key, written_k1 + written_k2 + 1, entity, l + 1, 1);
		}
	}
}
