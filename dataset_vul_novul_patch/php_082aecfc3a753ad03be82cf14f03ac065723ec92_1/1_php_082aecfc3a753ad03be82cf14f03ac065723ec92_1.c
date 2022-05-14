static int exif_process_IFD_in_JPEG(image_info_type *ImageInfo, char *dir_start, char *offset_base, size_t IFDlength, size_t displacement, int section_index TSRMLS_DC)
{
	int de;
	int NumDirEntries;
	int NextDirOffset;

#ifdef EXIF_DEBUG
	exif_error_docref(NULL EXIFERR_CC, ImageInfo, E_NOTICE, "Process %s (x%04X(=%d))", exif_get_sectionname(section_index), IFDlength, IFDlength);
#endif
 
        ImageInfo->sections_found |= FOUND_IFD0;
 
        NumDirEntries = php_ifd_get16u(dir_start, ImageInfo->motorola_intel);
 
        if ((dir_start+2+NumDirEntries*12) > (offset_base+IFDlength)) {
		if (!exif_process_IFD_TAG(ImageInfo, dir_start + 2 + 12 * de,
								  offset_base, IFDlength, displacement, section_index, 1, exif_get_tag_table(section_index) TSRMLS_CC)) {
			return FALSE;
		}
	}
	/*
	 * Ignore IFD2 if it purportedly exists
	 */
	if (section_index == SECTION_THUMBNAIL) {
		return TRUE;
	}
	/*
	 * Hack to make it process IDF1 I hope
	 * There are 2 IDFs, the second one holds the keys (0x0201 and 0x0202) to the thumbnail
	 */
	NextDirOffset = php_ifd_get32u(dir_start+2+12*de, ImageInfo->motorola_intel);
	if (NextDirOffset) {
         * Hack to make it process IDF1 I hope
         * There are 2 IDFs, the second one holds the keys (0x0201 and 0x0202) to the thumbnail
         */
        NextDirOffset = php_ifd_get32u(dir_start+2+12*de, ImageInfo->motorola_intel);
        if (NextDirOffset) {
                /* the next line seems false but here IFDlength means length of all IFDs */
#ifdef EXIF_DEBUG
			exif_error_docref(NULL EXIFERR_CC, ImageInfo, E_NOTICE, "Thumbnail size: 0x%04X", ImageInfo->Thumbnail.size);
#endif
			if (ImageInfo->Thumbnail.filetype != IMAGE_FILETYPE_UNKNOWN
			&&  ImageInfo->Thumbnail.size
			&&  ImageInfo->Thumbnail.offset
			&&  ImageInfo->read_thumbnail
			) {
				exif_thumbnail_extract(ImageInfo, offset_base, IFDlength TSRMLS_CC);
			}
			return TRUE;
		} else {
			return FALSE;
		}
	}
	return TRUE;
}
