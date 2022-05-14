choose_volume(struct archive_read *a, struct iso9660 *iso9660)
{
	struct file_info *file;
	int64_t skipsize;
	struct vd *vd;
	const void *block;
	char seenJoliet;

	vd = &(iso9660->primary);
	if (!iso9660->opt_support_joliet)
		iso9660->seenJoliet = 0;
	if (iso9660->seenJoliet &&
		vd->location > iso9660->joliet.location)
 		/* This condition is unlikely; by way of caution. */
 		vd = &(iso9660->joliet);
 
	skipsize = LOGICAL_BLOCK_SIZE * (int64_t)vd->location;
 	skipsize = __archive_read_consume(a, skipsize);
 	if (skipsize < 0)
 		return ((int)skipsize);
	iso9660->current_position = skipsize;

	block = __archive_read_ahead(a, vd->size, NULL);
	if (block == NULL) {
		archive_set_error(&a->archive, ARCHIVE_ERRNO_MISC,
		    "Failed to read full block when scanning "
		    "ISO9660 directory list");
		return (ARCHIVE_FATAL);
	}

	/*
	 * While reading Root Directory, flag seenJoliet must be zero to
	 * avoid converting special name 0x00(Current Directory) and
	 * next byte to UCS2.
	 */
	seenJoliet = iso9660->seenJoliet;/* Save flag. */
	iso9660->seenJoliet = 0;
	file = parse_file_info(a, NULL, block);
	if (file == NULL)
		return (ARCHIVE_FATAL);
	iso9660->seenJoliet = seenJoliet;

	/*
	 * If the iso image has both RockRidge and Joliet, we preferentially
	 * use RockRidge Extensions rather than Joliet ones.
	 */
	if (vd == &(iso9660->primary) && iso9660->seenRockridge
	    && iso9660->seenJoliet)
		iso9660->seenJoliet = 0;

	if (vd == &(iso9660->primary) && !iso9660->seenRockridge
 	    && iso9660->seenJoliet) {
 		/* Switch reading data from primary to joliet. */
 		vd = &(iso9660->joliet);
		skipsize = LOGICAL_BLOCK_SIZE * (int64_t)vd->location;
 		skipsize -= iso9660->current_position;
 		skipsize = __archive_read_consume(a, skipsize);
 		if (skipsize < 0)
			return ((int)skipsize);
		iso9660->current_position += skipsize;

		block = __archive_read_ahead(a, vd->size, NULL);
		if (block == NULL) {
			archive_set_error(&a->archive, ARCHIVE_ERRNO_MISC,
			    "Failed to read full block when scanning "
			    "ISO9660 directory list");
			return (ARCHIVE_FATAL);
		}
		iso9660->seenJoliet = 0;
		file = parse_file_info(a, NULL, block);
		if (file == NULL)
			return (ARCHIVE_FATAL);
		iso9660->seenJoliet = seenJoliet;
	}

	/* Store the root directory in the pending list. */
	if (add_entry(a, iso9660, file) != ARCHIVE_OK)
		return (ARCHIVE_FATAL);
	if (iso9660->seenRockridge) {
		a->archive.archive_format = ARCHIVE_FORMAT_ISO9660_ROCKRIDGE;
		a->archive.archive_format_name =
		    "ISO9660 with Rockridge extensions";
	}

	return (ARCHIVE_OK);
}
