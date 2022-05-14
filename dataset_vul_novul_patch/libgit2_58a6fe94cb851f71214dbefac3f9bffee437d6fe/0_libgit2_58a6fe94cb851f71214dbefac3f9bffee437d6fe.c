static int parse_index(git_index *index, const char *buffer, size_t buffer_size)
{
	int error = 0;
	unsigned int i;
	struct index_header header = { 0 };
	git_oid checksum_calculated, checksum_expected;
	const char *last = NULL;
	const char *empty = "";

#define seek_forward(_increase) { \
	if (_increase >= buffer_size) { \
		error = index_error_invalid("ran out of data while parsing"); \
		goto done; } \
	buffer += _increase; \
	buffer_size -= _increase;\
}

	if (buffer_size < INDEX_HEADER_SIZE + INDEX_FOOTER_SIZE)
		return index_error_invalid("insufficient buffer space");

	/* Precalculate the SHA1 of the files's contents -- we'll match it to
	 * the provided SHA1 in the footer */
	git_hash_buf(&checksum_calculated, buffer, buffer_size - INDEX_FOOTER_SIZE);

	/* Parse header */
	if ((error = read_header(&header, buffer)) < 0)
		return error;

	index->version = header.version;
	if (index->version >= INDEX_VERSION_NUMBER_COMP)
		last = empty;

	seek_forward(INDEX_HEADER_SIZE);

	assert(!index->entries.length);

	if (index->ignore_case)
		git_idxmap_icase_resize((khash_t(idxicase) *) index->entries_map, header.entry_count);
	else
		git_idxmap_resize(index->entries_map, header.entry_count);

 	/* Parse all the entries */
 	for (i = 0; i < header.entry_count && buffer_size > INDEX_FOOTER_SIZE; ++i) {
 		git_index_entry *entry = NULL;
		size_t entry_size;
 
		if ((error = read_entry(&entry, &entry_size, index, buffer, buffer_size, last)) < 0) {
 			error = index_error_invalid("invalid entry");
 			goto done;
 		}

		if ((error = git_vector_insert(&index->entries, entry)) < 0) {
			index_entry_free(entry);
			goto done;
		}

		INSERT_IN_MAP(index, entry, &error);

		if (error < 0) {
			index_entry_free(entry);
			goto done;
		}
		error = 0;

		if (index->version >= INDEX_VERSION_NUMBER_COMP)
			last = entry->path;

		seek_forward(entry_size);
	}

	if (i != header.entry_count) {
		error = index_error_invalid("header entries changed while parsing");
		goto done;
	}

	/* There's still space for some extensions! */
	while (buffer_size > INDEX_FOOTER_SIZE) {
		size_t extension_size;

		extension_size = read_extension(index, buffer, buffer_size);

		/* see if we have read any bytes from the extension */
		if (extension_size == 0) {
			error = index_error_invalid("extension is truncated");
			goto done;
		}

		seek_forward(extension_size);
	}

	if (buffer_size != INDEX_FOOTER_SIZE) {
		error = index_error_invalid(
			"buffer size does not match index footer size");
		goto done;
	}

	/* 160-bit SHA-1 over the content of the index file before this checksum. */
	git_oid_fromraw(&checksum_expected, (const unsigned char *)buffer);

	if (git_oid__cmp(&checksum_calculated, &checksum_expected) != 0) {
		error = index_error_invalid(
			"calculated checksum does not match expected");
		goto done;
	}

	git_oid_cpy(&index->checksum, &checksum_calculated);

#undef seek_forward

	/* Entries are stored case-sensitively on disk, so re-sort now if
	 * in-memory index is supposed to be case-insensitive
	 */
	git_vector_set_sorted(&index->entries, !index->ignore_case);
	git_vector_sort(&index->entries);

done:
	return error;
}
