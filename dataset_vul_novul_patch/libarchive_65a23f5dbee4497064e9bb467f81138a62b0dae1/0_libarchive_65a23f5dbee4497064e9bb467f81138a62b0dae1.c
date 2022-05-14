get_uncompressed_data(struct archive_read *a, const void **buff, size_t size,
    size_t minimum)
{
	struct _7zip *zip = (struct _7zip *)a->format->data;
	ssize_t bytes_avail;

 	if (zip->codec == _7Z_COPY && zip->codec2 == (unsigned long)-1) {
 		/* Copy mode. */
 
		*buff = __archive_read_ahead(a, minimum, &bytes_avail);
 		if (bytes_avail <= 0) {
 			archive_set_error(&a->archive,
 			    ARCHIVE_ERRNO_FILE_FORMAT,
			    "Truncated 7-Zip file data");
			return (ARCHIVE_FATAL);
		}
		if ((size_t)bytes_avail >
		    zip->uncompressed_buffer_bytes_remaining)
			bytes_avail = (ssize_t)
			    zip->uncompressed_buffer_bytes_remaining;
		if ((size_t)bytes_avail > size)
			bytes_avail = (ssize_t)size;

		zip->pack_stream_bytes_unconsumed = bytes_avail;
	} else if (zip->uncompressed_buffer_pointer == NULL) {
		/* Decompression has failed. */
		archive_set_error(&(a->archive),
		    ARCHIVE_ERRNO_MISC, "Damaged 7-Zip archive");
		return (ARCHIVE_FATAL);
	} else {
		/* Packed mode. */
		if (minimum > zip->uncompressed_buffer_bytes_remaining) {
			/*
			 * If remaining uncompressed data size is less than
			 * the minimum size, fill the buffer up to the
			 * minimum size.
			 */
			if (extract_pack_stream(a, minimum) < 0)
				return (ARCHIVE_FATAL);
		}
		if (size > zip->uncompressed_buffer_bytes_remaining)
			bytes_avail = (ssize_t)
			    zip->uncompressed_buffer_bytes_remaining;
		else
			bytes_avail = (ssize_t)size;
		*buff = zip->uncompressed_buffer_pointer;
		zip->uncompressed_buffer_pointer += bytes_avail;
	}
	zip->uncompressed_buffer_bytes_remaining -= bytes_avail;
	return (bytes_avail);
}
