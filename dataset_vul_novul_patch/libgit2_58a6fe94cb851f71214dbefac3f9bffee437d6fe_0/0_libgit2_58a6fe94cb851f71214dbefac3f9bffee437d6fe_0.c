static size_t read_entry(
static int read_entry(
 	git_index_entry **out,
	size_t *out_size,
 	git_index *index,
 	const void *buffer,
 	size_t buffer_size,
	const char *last)
{
	size_t path_length, entry_size;
	const char *path_ptr;
	struct entry_short source;
	git_index_entry entry = {{0}};
	bool compressed = index->version >= INDEX_VERSION_NUMBER_COMP;
 	char *tmp_path = NULL;
 
 	if (INDEX_FOOTER_SIZE + minimal_entry_size > buffer_size)
		return -1;
 
 	/* buffer is not guaranteed to be aligned */
 	memcpy(&source, buffer, sizeof(struct entry_short));

	entry.ctime.seconds = (git_time_t)ntohl(source.ctime.seconds);
	entry.ctime.nanoseconds = ntohl(source.ctime.nanoseconds);
	entry.mtime.seconds = (git_time_t)ntohl(source.mtime.seconds);
	entry.mtime.nanoseconds = ntohl(source.mtime.nanoseconds);
	entry.dev = ntohl(source.dev);
	entry.ino = ntohl(source.ino);
	entry.mode = ntohl(source.mode);
	entry.uid = ntohl(source.uid);
	entry.gid = ntohl(source.gid);
	entry.file_size = ntohl(source.file_size);
	git_oid_cpy(&entry.id, &source.oid);
	entry.flags = ntohs(source.flags);

	if (entry.flags & GIT_IDXENTRY_EXTENDED) {
		uint16_t flags_raw;
		size_t flags_offset;

		flags_offset = offsetof(struct entry_long, flags_extended);
		memcpy(&flags_raw, (const char *) buffer + flags_offset,
			sizeof(flags_raw));
		flags_raw = ntohs(flags_raw);

		memcpy(&entry.flags_extended, &flags_raw, sizeof(flags_raw));
		path_ptr = (const char *) buffer + offsetof(struct entry_long, path);
	} else
		path_ptr = (const char *) buffer + offsetof(struct entry_short, path);

	if (!compressed) {
		path_length = entry.flags & GIT_IDXENTRY_NAMEMASK;

		/* if this is a very long string, we must find its
		 * real length without overflowing */
		if (path_length == 0xFFF) {
			const char *path_end;
 
 			path_end = memchr(path_ptr, '\0', buffer_size);
 			if (path_end == NULL)
				return -1;
 
 			path_length = path_end - path_ptr;
 		}

		entry_size = index_entry_size(path_length, 0, entry.flags);
		entry.path = (char *)path_ptr;
	} else {
		size_t varint_len;
		size_t strip_len = git_decode_varint((const unsigned char *)path_ptr,
						     &varint_len);
		size_t last_len = strlen(last);
		size_t prefix_len = last_len - strip_len;
		size_t suffix_len = strlen(path_ptr + varint_len);
		size_t path_len;

		if (varint_len == 0)
			return index_error_invalid("incorrect prefix length");

		GITERR_CHECK_ALLOC_ADD(&path_len, prefix_len, suffix_len);
		GITERR_CHECK_ALLOC_ADD(&path_len, path_len, 1);
		tmp_path = git__malloc(path_len);
		GITERR_CHECK_ALLOC(tmp_path);

		memcpy(tmp_path, last, prefix_len);
		memcpy(tmp_path + prefix_len, path_ptr + varint_len, suffix_len + 1);
		entry_size = index_entry_size(suffix_len, varint_len, entry.flags);
 		entry.path = tmp_path;
 	}
 
	if (entry_size == 0)
		return -1;

 	if (INDEX_FOOTER_SIZE + entry_size > buffer_size)
		return -1;
 
 	if (index_entry_dup(out, index, &entry) < 0) {
 		git__free(tmp_path);
		return -1;
 	}
 
 	git__free(tmp_path);
	*out_size = entry_size;
	return 0;
 }
