 _archive_write_data(struct archive *_a, const void *buff, size_t s)
 {
 	struct archive_write *a = (struct archive_write *)_a;
 	archive_check_magic(&a->archive, ARCHIVE_WRITE_MAGIC,
 	    ARCHIVE_STATE_DATA, "archive_write_data");
 	archive_clear_error(&a->archive);
 	return ((a->format_write_data)(a, buff, s));
 }
