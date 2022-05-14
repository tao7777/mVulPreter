zip_read_mac_metadata(struct archive_read *a, struct archive_entry *entry,
    struct zip_entry *rsrc)
{
	struct zip *zip = (struct zip *)a->format->data;
	unsigned char *metadata, *mp;
	int64_t offset = archive_filter_bytes(&a->archive, 0);
	size_t remaining_bytes, metadata_bytes;
	ssize_t hsize;
	int ret = ARCHIVE_OK, eof;
 
 	switch(rsrc->compression) {
 	case 0:  /* No compression. */
 #ifdef HAVE_ZLIB_H
 	case 8: /* Deflate compression. */
 #endif
		break;
	default: /* Unsupported compression. */
		/* Return a warning. */
		archive_set_error(&a->archive, ARCHIVE_ERRNO_FILE_FORMAT,
		    "Unsupported ZIP compression method (%s)",
		    compression_name(rsrc->compression));
		/* We can't decompress this entry, but we will
		 * be able to skip() it and try the next entry. */
		return (ARCHIVE_WARN);
	}

	if (rsrc->uncompressed_size > (4 * 1024 * 1024)) {
		archive_set_error(&a->archive, ARCHIVE_ERRNO_FILE_FORMAT,
		    "Mac metadata is too large: %jd > 4M bytes",
 		    (intmax_t)rsrc->uncompressed_size);
 		return (ARCHIVE_WARN);
 	}
 
 	metadata = malloc((size_t)rsrc->uncompressed_size);
 	if (metadata == NULL) {
		archive_set_error(&a->archive, ENOMEM,
		    "Can't allocate memory for Mac metadata");
		return (ARCHIVE_FATAL);
	}

	if (offset < rsrc->local_header_offset)
		__archive_read_consume(a, rsrc->local_header_offset - offset);
	else if (offset != rsrc->local_header_offset) {
		__archive_read_seek(a, rsrc->local_header_offset, SEEK_SET);
	}

	hsize = zip_get_local_file_header_size(a, 0);
	__archive_read_consume(a, hsize);

	remaining_bytes = (size_t)rsrc->compressed_size;
	metadata_bytes = (size_t)rsrc->uncompressed_size;
	mp = metadata;
	eof = 0;
	while (!eof && remaining_bytes) {
		const unsigned char *p;
		ssize_t bytes_avail;
		size_t bytes_used;

		p = __archive_read_ahead(a, 1, &bytes_avail);
		if (p == NULL) {
			archive_set_error(&a->archive,
			    ARCHIVE_ERRNO_FILE_FORMAT,
			    "Truncated ZIP file header");
			ret = ARCHIVE_WARN;
			goto exit_mac_metadata;
		}
		if ((size_t)bytes_avail > remaining_bytes)
 			bytes_avail = remaining_bytes;
 		switch(rsrc->compression) {
 		case 0:  /* No compression. */
 			memcpy(mp, p, bytes_avail);
 			bytes_used = (size_t)bytes_avail;
 			metadata_bytes -= bytes_used;
			mp += bytes_used;
			if (metadata_bytes == 0)
				eof = 1;
			break;
#ifdef HAVE_ZLIB_H
		case 8: /* Deflate compression. */
		{
			int r;

			ret = zip_deflate_init(a, zip);
			if (ret != ARCHIVE_OK)
				goto exit_mac_metadata;
			zip->stream.next_in =
			    (Bytef *)(uintptr_t)(const void *)p;
			zip->stream.avail_in = (uInt)bytes_avail;
			zip->stream.total_in = 0;
			zip->stream.next_out = mp;
			zip->stream.avail_out = (uInt)metadata_bytes;
			zip->stream.total_out = 0;

			r = inflate(&zip->stream, 0);
			switch (r) {
			case Z_OK:
				break;
			case Z_STREAM_END:
				eof = 1;
				break;
			case Z_MEM_ERROR:
				archive_set_error(&a->archive, ENOMEM,
				    "Out of memory for ZIP decompression");
				ret = ARCHIVE_FATAL;
				goto exit_mac_metadata;
			default:
				archive_set_error(&a->archive,
				    ARCHIVE_ERRNO_MISC,
				    "ZIP decompression failed (%d)", r);
				ret = ARCHIVE_FATAL;
				goto exit_mac_metadata;
			}
			bytes_used = zip->stream.total_in;
			metadata_bytes -= zip->stream.total_out;
			mp += zip->stream.total_out;
			break;
		}
#endif
		default:
			bytes_used = 0;
			break;
		}
		__archive_read_consume(a, bytes_used);
		remaining_bytes -= bytes_used;
	}
	archive_entry_copy_mac_metadata(entry, metadata,
	    (size_t)rsrc->uncompressed_size - metadata_bytes);

exit_mac_metadata:
	__archive_read_seek(a, offset, SEEK_SET);
	zip->decompress_init = 0;
	free(metadata);
	return (ret);
}
