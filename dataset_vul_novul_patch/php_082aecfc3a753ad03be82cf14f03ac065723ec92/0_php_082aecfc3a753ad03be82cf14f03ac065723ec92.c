char * exif_dump_data(int *dump_free, int format, int components, int length, int motorola_intel, char *value_ptr TSRMLS_DC) /* {{{ */
{
	char *dump;
	int len;

	*dump_free = 0;
	if (format == TAG_FMT_STRING) {
		return value_ptr ? value_ptr : "<no data>";
	}
	if (format == TAG_FMT_UNDEFINED) {
		return "<undefined>\n";
	}
	if (format == TAG_FMT_IFD) {
		return "";
	}
	if (format == TAG_FMT_SINGLE || format == TAG_FMT_DOUBLE) {
		return "<not implemented>";
	}
	*dump_free = 1;
	if (components > 1) {
		len = spprintf(&dump, 0, "(%d,%d) {", components, length);
	} else {
		len = spprintf(&dump, 0, "{");
	}
	while(components > 0) {
		switch(format) {
			case TAG_FMT_BYTE:
			case TAG_FMT_UNDEFINED:
			case TAG_FMT_STRING:
			case TAG_FMT_SBYTE:
				dump = erealloc(dump, len + 4 + 1);
				snprintf(dump + len, 4 + 1, "0x%02X", *value_ptr);
				len += 4;
				value_ptr++;
				break;
			case TAG_FMT_USHORT:
			case TAG_FMT_SSHORT:
				dump = erealloc(dump, len + 6 + 1);
				snprintf(dump + len, 6 + 1, "0x%04X", php_ifd_get16s(value_ptr, motorola_intel));
				len += 6;
				value_ptr += 2;
				break;
			case TAG_FMT_ULONG:
			case TAG_FMT_SLONG:
				dump = erealloc(dump, len + 6 + 1);
				snprintf(dump + len, 6 + 1, "0x%04X", php_ifd_get32s(value_ptr, motorola_intel));
				len += 6;
				value_ptr += 4;
				break;
			case TAG_FMT_URATIONAL:
			case TAG_FMT_SRATIONAL:
				dump = erealloc(dump, len + 13 + 1);
				snprintf(dump + len, 13 + 1, "0x%04X/0x%04X", php_ifd_get32s(value_ptr, motorola_intel), php_ifd_get32s(value_ptr+4, motorola_intel));
				len += 13;
				value_ptr += 8;
				break;
		}
		if (components > 0) {
			dump = erealloc(dump, len + 2 + 1);
			snprintf(dump + len, 2 + 1, ", ");
			len += 2;
			components--;
		} else{
			break;
		}
	}
	dump = erealloc(dump, len + 1 + 1);
	snprintf(dump + len, 1 + 1, "}");
	return dump;
}
/* }}} */
#endif

/* {{{ exif_convert_any_format
 * Evaluate number, be it int, rational, or float from directory. */
static double exif_convert_any_format(void *value, int format, int motorola_intel TSRMLS_DC)
{
	int 		s_den;
	unsigned 	u_den;

	switch(format) {
		case TAG_FMT_SBYTE:     return *(signed char *)value;
		case TAG_FMT_BYTE:      return *(uchar *)value;

		case TAG_FMT_USHORT:    return php_ifd_get16u(value, motorola_intel);
		case TAG_FMT_ULONG:     return php_ifd_get32u(value, motorola_intel);

		case TAG_FMT_URATIONAL:
			u_den = php_ifd_get32u(4+(char *)value, motorola_intel);
			if (u_den == 0) {
				return 0;
			} else {
				return (double)php_ifd_get32u(value, motorola_intel) / u_den;
			}

		case TAG_FMT_SRATIONAL:
			s_den = php_ifd_get32s(4+(char *)value, motorola_intel);
			if (s_den == 0) {
				return 0;
			} else {
				return (double)php_ifd_get32s(value, motorola_intel) / s_den;
			}

		case TAG_FMT_SSHORT:    return (signed short)php_ifd_get16u(value, motorola_intel);
		case TAG_FMT_SLONG:     return php_ifd_get32s(value, motorola_intel);

		/* Not sure if this is correct (never seen float used in Exif format) */
		case TAG_FMT_SINGLE:
#ifdef EXIF_DEBUG
			php_error_docref(NULL TSRMLS_CC, E_NOTICE, "Found value of type single");
#endif
			return (double)*(float *)value;
		case TAG_FMT_DOUBLE:
#ifdef EXIF_DEBUG
			php_error_docref(NULL TSRMLS_CC, E_NOTICE, "Found value of type double");
#endif
			return *(double *)value;
	}
	return 0;
}
/* }}} */

/* {{{ exif_convert_any_to_int
 * Evaluate number, be it int, rational, or float from directory. */
static size_t exif_convert_any_to_int(void *value, int format, int motorola_intel TSRMLS_DC)
{
	int 		s_den;
	unsigned 	u_den;

	switch(format) {
		case TAG_FMT_SBYTE:     return *(signed char *)value;
		case TAG_FMT_BYTE:      return *(uchar *)value;

		case TAG_FMT_USHORT:    return php_ifd_get16u(value, motorola_intel);
		case TAG_FMT_ULONG:     return php_ifd_get32u(value, motorola_intel);

		case TAG_FMT_URATIONAL:
			u_den = php_ifd_get32u(4+(char *)value, motorola_intel);
			if (u_den == 0) {
				return 0;
			} else {
				return php_ifd_get32u(value, motorola_intel) / u_den;
			}

		case TAG_FMT_SRATIONAL:
			s_den = php_ifd_get32s(4+(char *)value, motorola_intel);
			if (s_den == 0) {
				return 0;
			} else {
				return php_ifd_get32s(value, motorola_intel) / s_den;
			}

		case TAG_FMT_SSHORT:    return php_ifd_get16u(value, motorola_intel);
		case TAG_FMT_SLONG:     return php_ifd_get32s(value, motorola_intel);

		/* Not sure if this is correct (never seen float used in Exif format) */
		case TAG_FMT_SINGLE:
#ifdef EXIF_DEBUG
			php_error_docref(NULL TSRMLS_CC, E_NOTICE, "Found value of type single");
#endif
			return (size_t)*(float *)value;
		case TAG_FMT_DOUBLE:
#ifdef EXIF_DEBUG
			php_error_docref(NULL TSRMLS_CC, E_NOTICE, "Found value of type double");
#endif
			return (size_t)*(double *)value;
	}
	return 0;
}
/* }}} */

/* {{{ struct image_info_value, image_info_list
*/
#ifndef WORD
#define WORD unsigned short
#endif
#ifndef DWORD
#define DWORD unsigned int
#endif

typedef struct {
	int             num;
	int             den;
} signed_rational;

typedef struct {
	unsigned int    num;
	unsigned int    den;
} unsigned_rational;

typedef union _image_info_value {
	char 				*s;
	unsigned            u;
	int 				i;
	float               f;
	double              d;
	signed_rational 	sr;
	unsigned_rational 	ur;
	union _image_info_value   *list;
} image_info_value;

typedef struct {
	WORD                tag;
	WORD                format;
	DWORD               length;
	DWORD               dummy;  /* value ptr of tiff directory entry */
	char 				*name;
	image_info_value    value;
} image_info_data;

typedef struct {
	int                 count;
	image_info_data 	*list;
} image_info_list;
/* }}} */

/* {{{ exif_get_sectionname
 Returns the name of a section
*/
#define SECTION_FILE        0
#define SECTION_COMPUTED    1
#define SECTION_ANY_TAG     2
#define SECTION_IFD0        3
#define SECTION_THUMBNAIL   4
#define SECTION_COMMENT     5
#define SECTION_APP0        6
#define SECTION_EXIF        7
#define SECTION_FPIX        8
#define SECTION_GPS         9
#define SECTION_INTEROP     10
#define SECTION_APP12       11
#define SECTION_WINXP       12
#define SECTION_MAKERNOTE   13
#define SECTION_COUNT       14

#define FOUND_FILE          (1<<SECTION_FILE)
#define FOUND_COMPUTED      (1<<SECTION_COMPUTED)
#define FOUND_ANY_TAG       (1<<SECTION_ANY_TAG)
#define FOUND_IFD0          (1<<SECTION_IFD0)
#define FOUND_THUMBNAIL     (1<<SECTION_THUMBNAIL)
#define FOUND_COMMENT       (1<<SECTION_COMMENT)
#define FOUND_APP0          (1<<SECTION_APP0)
#define FOUND_EXIF          (1<<SECTION_EXIF)
#define FOUND_FPIX          (1<<SECTION_FPIX)
#define FOUND_GPS           (1<<SECTION_GPS)
#define FOUND_INTEROP       (1<<SECTION_INTEROP)
#define FOUND_APP12         (1<<SECTION_APP12)
#define FOUND_WINXP         (1<<SECTION_WINXP)
#define FOUND_MAKERNOTE     (1<<SECTION_MAKERNOTE)

static char *exif_get_sectionname(int section)
{
	switch(section) {
		case SECTION_FILE:      return "FILE";
		case SECTION_COMPUTED:  return "COMPUTED";
		case SECTION_ANY_TAG:   return "ANY_TAG";
		case SECTION_IFD0:      return "IFD0";
		case SECTION_THUMBNAIL: return "THUMBNAIL";
		case SECTION_COMMENT:   return "COMMENT";
		case SECTION_APP0:      return "APP0";
		case SECTION_EXIF:      return "EXIF";
		case SECTION_FPIX:      return "FPIX";
		case SECTION_GPS:       return "GPS";
		case SECTION_INTEROP:   return "INTEROP";
		case SECTION_APP12:     return "APP12";
		case SECTION_WINXP:     return "WINXP";
		case SECTION_MAKERNOTE: return "MAKERNOTE";
	}
	return "";
}

static tag_table_type exif_get_tag_table(int section)
{
	switch(section) {
		case SECTION_FILE:      return &tag_table_IFD[0];
		case SECTION_COMPUTED:  return &tag_table_IFD[0];
		case SECTION_ANY_TAG:   return &tag_table_IFD[0];
		case SECTION_IFD0:      return &tag_table_IFD[0];
		case SECTION_THUMBNAIL: return &tag_table_IFD[0];
		case SECTION_COMMENT:   return &tag_table_IFD[0];
		case SECTION_APP0:      return &tag_table_IFD[0];
		case SECTION_EXIF:      return &tag_table_IFD[0];
		case SECTION_FPIX:      return &tag_table_IFD[0];
		case SECTION_GPS:       return &tag_table_GPS[0];
		case SECTION_INTEROP:   return &tag_table_IOP[0];
		case SECTION_APP12:     return &tag_table_IFD[0];
		case SECTION_WINXP:     return &tag_table_IFD[0];
	}
	return &tag_table_IFD[0];
}
/* }}} */

/* {{{ exif_get_sectionlist
   Return list of sectionnames specified by sectionlist. Return value must be freed
*/
static char *exif_get_sectionlist(int sectionlist TSRMLS_DC)
{
	int i, len, ml = 0;
	char *sections;

	for(i=0; i<SECTION_COUNT; i++) {
		ml += strlen(exif_get_sectionname(i))+2;
	}
	sections = safe_emalloc(ml, 1, 1);
	sections[0] = '\0';
	len = 0;
	for(i=0; i<SECTION_COUNT; i++) {
		if (sectionlist&(1<<i)) {
			snprintf(sections+len, ml-len, "%s, ", exif_get_sectionname(i));
			len = strlen(sections);
		}
	}
	if (len>2)
		sections[len-2] = '\0';
	return sections;
}
/* }}} */

/* {{{ struct image_info_type
   This structure stores Exif header image elements in a simple manner
   Used to store camera data as extracted from the various ways that it can be
   stored in a nexif header
*/

typedef struct {
	int     type;
	size_t  size;
	uchar   *data;
} file_section;

typedef struct {
	int             count;
	file_section    *list;
} file_section_list;

typedef struct {
	image_filetype  filetype;
	size_t          width, height;
	size_t          size;
	size_t          offset;
	char 	        *data;
} thumbnail_data;

typedef struct {
	char			*value;
	size_t			size;
	int				tag;
} xp_field_type;

typedef struct {
	int             count;
	xp_field_type   *list;
} xp_field_list;

/* This structure is used to store a section of a Jpeg file. */
typedef struct {
	php_stream      *infile;
	char            *FileName;
	time_t          FileDateTime;
	size_t          FileSize;
	image_filetype  FileType;
	int             Height, Width;
	int             IsColor;

	char            *make;
	char            *model;

	float           ApertureFNumber;
	float           ExposureTime;
	double          FocalplaneUnits;
	float           CCDWidth;
	double          FocalplaneXRes;
	size_t          ExifImageWidth;
	float           FocalLength;
	float           Distance;

	int             motorola_intel; /* 1 Motorola; 0 Intel */

	char            *UserComment;
	int             UserCommentLength;
	char            *UserCommentEncoding;
	char            *encode_unicode;
	char            *decode_unicode_be;
	char            *decode_unicode_le;
	char            *encode_jis;
	char            *decode_jis_be;
	char            *decode_jis_le;
	char            *Copyright;/* EXIF standard defines Copyright as "<Photographer> [ '\0' <Editor> ] ['\0']" */
	char            *CopyrightPhotographer;
	char            *CopyrightEditor;

	xp_field_list   xp_fields;

	thumbnail_data  Thumbnail;
	/* other */
	int             sections_found; /* FOUND_<marker> */
	image_info_list info_list[SECTION_COUNT];
	/* for parsing */
	int             read_thumbnail;
	int             read_all;
	int             ifd_nesting_level;
	/* internal */
	file_section_list 	file;
} image_info_type;
/* }}} */

/* {{{ exif_error_docref */
static void exif_error_docref(const char *docref EXIFERR_DC, const image_info_type *ImageInfo, int type, const char *format, ...)
{
	va_list args;

	va_start(args, format);
#ifdef EXIF_DEBUG
	{
		char *buf;

		spprintf(&buf, 0, "%s(%d): %s", _file, _line, format);
		php_verror(docref, ImageInfo->FileName?ImageInfo->FileName:"", type, buf, args TSRMLS_CC);
		efree(buf);
	}
#else
	php_verror(docref, ImageInfo->FileName?ImageInfo->FileName:"", type, format, args TSRMLS_CC);
#endif
	va_end(args);
}
/* }}} */

/* {{{ jpeg_sof_info
 */
typedef struct {
	int     bits_per_sample;
	size_t  width;
	size_t  height;
	int     num_components;
} jpeg_sof_info;
/* }}} */

/* {{{ exif_file_sections_add
 Add a file_section to image_info
 returns the used block or -1. if size>0 and data == NULL buffer of size is allocated
*/
static int exif_file_sections_add(image_info_type *ImageInfo, int type, size_t size, uchar *data)
{
	file_section    *tmp;
	int             count = ImageInfo->file.count;

	tmp = safe_erealloc(ImageInfo->file.list, (count+1), sizeof(file_section), 0);
	ImageInfo->file.list = tmp;
	ImageInfo->file.list[count].type = 0xFFFF;
	ImageInfo->file.list[count].data = NULL;
	ImageInfo->file.list[count].size = 0;
	ImageInfo->file.count = count+1;
	if (!size) {
		data = NULL;
	} else if (data == NULL) {
		data = safe_emalloc(size, 1, 0);
	}
	ImageInfo->file.list[count].type = type;
	ImageInfo->file.list[count].data = data;
	ImageInfo->file.list[count].size = size;
	return count;
}
/* }}} */

/* {{{ exif_file_sections_realloc
 Reallocate a file section returns 0 on success and -1 on failure
*/
static int exif_file_sections_realloc(image_info_type *ImageInfo, int section_index, size_t size TSRMLS_DC)
{
	void *tmp;

	/* This is not a malloc/realloc check. It is a plausibility check for the
	 * function parameters (requirements engineering).
	 */
	if (section_index >= ImageInfo->file.count) {
		EXIF_ERRLOG_FSREALLOC(ImageInfo)
		return -1;
	}
	tmp = safe_erealloc(ImageInfo->file.list[section_index].data, 1, size, 0);
	ImageInfo->file.list[section_index].data = tmp;
	ImageInfo->file.list[section_index].size = size;
	return 0;
}
/* }}} */

/* {{{ exif_file_section_free
   Discard all file_sections in ImageInfo
*/
static int exif_file_sections_free(image_info_type *ImageInfo)
{
	int i;

	if (ImageInfo->file.count) {
		for (i=0; i<ImageInfo->file.count; i++) {
			EFREE_IF(ImageInfo->file.list[i].data);
		}
	}
	EFREE_IF(ImageInfo->file.list);
	ImageInfo->file.count = 0;
	return TRUE;
}
/* }}} */

/* {{{ exif_iif_add_value
 Add a value to image_info
*/
static void exif_iif_add_value(image_info_type *image_info, int section_index, char *name, int tag, int format, int length, void* value, int motorola_intel TSRMLS_DC)
{
	size_t idex;
	void *vptr;
	image_info_value *info_value;
	image_info_data  *info_data;
	image_info_data  *list;

	if (length < 0) {
		return;
	}

	list = safe_erealloc(image_info->info_list[section_index].list, (image_info->info_list[section_index].count+1), sizeof(image_info_data), 0);
	image_info->info_list[section_index].list = list;

	info_data  = &image_info->info_list[section_index].list[image_info->info_list[section_index].count];
	memset(info_data, 0, sizeof(image_info_data));
	info_data->tag    = tag;
	info_data->format = format;
	info_data->length = length;
	info_data->name   = estrdup(name);
	info_value        = &info_data->value;

	switch (format) {
		case TAG_FMT_STRING:
			if (value) {
				length = php_strnlen(value, length);
				info_value->s = estrndup(value, length);
				info_data->length = length;
			} else {
				info_data->length = 0;
				info_value->s = estrdup("");
			}
			break;

		default:
			/* Standard says more types possible but skip them...
			 * but allow users to handle data if they know how to
			 * So not return but use type UNDEFINED
			 * return;
			 */
			info_data->tag = TAG_FMT_UNDEFINED;/* otherwise not freed from memory */
		case TAG_FMT_SBYTE:
		case TAG_FMT_BYTE:
		/* in contrast to strings bytes do not need to allocate buffer for NULL if length==0 */
			if (!length)
				break;
		case TAG_FMT_UNDEFINED:
			if (value) {
				/* do not recompute length here */
				info_value->s = estrndup(value, length);
				info_data->length = length;
			} else {
				info_data->length = 0;
				info_value->s = estrdup("");
			}
			break;

		case TAG_FMT_USHORT:
		case TAG_FMT_ULONG:
		case TAG_FMT_URATIONAL:
		case TAG_FMT_SSHORT:
		case TAG_FMT_SLONG:
		case TAG_FMT_SRATIONAL:
		case TAG_FMT_SINGLE:
		case TAG_FMT_DOUBLE:
			if (length==0) {
				break;
			} else
			if (length>1) {
				info_value->list = safe_emalloc(length, sizeof(image_info_value), 0);
			} else {
				info_value = &info_data->value;
			}
			for (idex=0,vptr=value; idex<(size_t)length; idex++,vptr=(char *) vptr + php_tiff_bytes_per_format[format]) {
				if (length>1) {
					info_value = &info_data->value.list[idex];
				}
				switch (format) {
					case TAG_FMT_USHORT:
						info_value->u = php_ifd_get16u(vptr, motorola_intel);
						break;

					case TAG_FMT_ULONG:
						info_value->u = php_ifd_get32u(vptr, motorola_intel);
						break;

					case TAG_FMT_URATIONAL:
						info_value->ur.num = php_ifd_get32u(vptr, motorola_intel);
						info_value->ur.den = php_ifd_get32u(4+(char *)vptr, motorola_intel);
						break;

					case TAG_FMT_SSHORT:
						info_value->i = php_ifd_get16s(vptr, motorola_intel);
						break;

					case TAG_FMT_SLONG:
						info_value->i = php_ifd_get32s(vptr, motorola_intel);
						break;

					case TAG_FMT_SRATIONAL:
						info_value->sr.num = php_ifd_get32u(vptr, motorola_intel);
						info_value->sr.den = php_ifd_get32u(4+(char *)vptr, motorola_intel);
						break;

					case TAG_FMT_SINGLE:
#ifdef EXIF_DEBUG
						php_error_docref(NULL TSRMLS_CC, E_WARNING, "Found value of type single");
#endif
						info_value->f = *(float *)value;

					case TAG_FMT_DOUBLE:
#ifdef EXIF_DEBUG
						php_error_docref(NULL TSRMLS_CC, E_WARNING, "Found value of type double");
#endif
						info_value->d = *(double *)value;
						break;
				}
			}
	}
	image_info->sections_found |= 1<<section_index;
	image_info->info_list[section_index].count++;
}
/* }}} */

/* {{{ exif_iif_add_tag
 Add a tag from IFD to image_info
*/
static void exif_iif_add_tag(image_info_type *image_info, int section_index, char *name, int tag, int format, size_t length, void* value TSRMLS_DC)
{
	exif_iif_add_value(image_info, section_index, name, tag, format, (int)length, value, image_info->motorola_intel TSRMLS_CC);
}
/* }}} */

/* {{{ exif_iif_add_int
 Add an int value to image_info
*/
static void exif_iif_add_int(image_info_type *image_info, int section_index, char *name, int value TSRMLS_DC)
{
	image_info_data  *info_data;
	image_info_data  *list;

	list = safe_erealloc(image_info->info_list[section_index].list, (image_info->info_list[section_index].count+1), sizeof(image_info_data), 0);
	image_info->info_list[section_index].list = list;

	info_data  = &image_info->info_list[section_index].list[image_info->info_list[section_index].count];
	info_data->tag    = TAG_NONE;
	info_data->format = TAG_FMT_SLONG;
	info_data->length = 1;
	info_data->name   = estrdup(name);
	info_data->value.i = value;
	image_info->sections_found |= 1<<section_index;
	image_info->info_list[section_index].count++;
}
/* }}} */

/* {{{ exif_iif_add_str
 Add a string value to image_info MUST BE NUL TERMINATED
*/
static void exif_iif_add_str(image_info_type *image_info, int section_index, char *name, char *value TSRMLS_DC)
{
	image_info_data  *info_data;
	image_info_data  *list;

	if (value) {
		list = safe_erealloc(image_info->info_list[section_index].list, (image_info->info_list[section_index].count+1), sizeof(image_info_data), 0);
		image_info->info_list[section_index].list = list;
		info_data  = &image_info->info_list[section_index].list[image_info->info_list[section_index].count];
		info_data->tag    = TAG_NONE;
		info_data->format = TAG_FMT_STRING;
		info_data->length = 1;
		info_data->name   = estrdup(name);
		info_data->value.s = estrdup(value);
		image_info->sections_found |= 1<<section_index;
		image_info->info_list[section_index].count++;
	}
}
/* }}} */

/* {{{ exif_iif_add_fmt
 Add a format string value to image_info MUST BE NUL TERMINATED
*/
static void exif_iif_add_fmt(image_info_type *image_info, int section_index, char *name TSRMLS_DC, char *value, ...)
{
	char             *tmp;
	va_list 		 arglist;

	va_start(arglist, value);
	if (value) {
		vspprintf(&tmp, 0, value, arglist);
		exif_iif_add_str(image_info, section_index, name, tmp TSRMLS_CC);
		efree(tmp);
	}
	va_end(arglist);
}
/* }}} */

/* {{{ exif_iif_add_str
 Add a string value to image_info MUST BE NUL TERMINATED
*/
static void exif_iif_add_buffer(image_info_type *image_info, int section_index, char *name, int length, char *value TSRMLS_DC)
{
	image_info_data  *info_data;
	image_info_data  *list;

	if (value) {
		list = safe_erealloc(image_info->info_list[section_index].list, (image_info->info_list[section_index].count+1), sizeof(image_info_data), 0);
		image_info->info_list[section_index].list = list;
		info_data  = &image_info->info_list[section_index].list[image_info->info_list[section_index].count];
		info_data->tag    = TAG_NONE;
		info_data->format = TAG_FMT_UNDEFINED;
		info_data->length = length;
		info_data->name   = estrdup(name);
		info_data->value.s = safe_emalloc(length, 1, 1);
		memcpy(info_data->value.s, value, length);
		info_data->value.s[length] = 0;
		image_info->sections_found |= 1<<section_index;
		image_info->info_list[section_index].count++;
	}
}
/* }}} */

/* {{{ exif_iif_free
 Free memory allocated for image_info
*/
static void exif_iif_free(image_info_type *image_info, int section_index) {
	int  i;
	void *f; /* faster */

	if (image_info->info_list[section_index].count) {
		for (i=0; i < image_info->info_list[section_index].count; i++) {
			if ((f=image_info->info_list[section_index].list[i].name) != NULL) {
				efree(f);
			}
			switch(image_info->info_list[section_index].list[i].format) {
				case TAG_FMT_SBYTE:
				case TAG_FMT_BYTE:
					/* in contrast to strings bytes do not need to allocate buffer for NULL if length==0 */
					if (image_info->info_list[section_index].list[i].length<1)
						break;
				default:
				case TAG_FMT_UNDEFINED:
				case TAG_FMT_STRING:
					if ((f=image_info->info_list[section_index].list[i].value.s) != NULL) {
						efree(f);
					}
					break;

				case TAG_FMT_USHORT:
				case TAG_FMT_ULONG:
				case TAG_FMT_URATIONAL:
				case TAG_FMT_SSHORT:
				case TAG_FMT_SLONG:
				case TAG_FMT_SRATIONAL:
				case TAG_FMT_SINGLE:
				case TAG_FMT_DOUBLE:
					/* nothing to do here */
					if (image_info->info_list[section_index].list[i].length > 1) {
						if ((f=image_info->info_list[section_index].list[i].value.list) != NULL) {
							efree(f);
						}
					}
					break;
			}
		}
	}
	EFREE_IF(image_info->info_list[section_index].list);
}
/* }}} */

/* {{{ add_assoc_image_info
 * Add image_info to associative array value. */
static void add_assoc_image_info(zval *value, int sub_array, image_info_type *image_info, int section_index TSRMLS_DC)
{
	char    buffer[64], *val, *name, uname[64];
	int     i, ap, l, b, idx=0, unknown=0;
#ifdef EXIF_DEBUG
	int     info_tag;
#endif
	image_info_value *info_value;
	image_info_data  *info_data;
	zval 			 *tmpi, *array = NULL;

#ifdef EXIF_DEBUG
/*		php_error_docref(NULL TSRMLS_CC, E_NOTICE, "Adding %d infos from section %s", image_info->info_list[section_index].count, exif_get_sectionname(section_index));*/
#endif
	if (image_info->info_list[section_index].count) {
		if (sub_array) {
			MAKE_STD_ZVAL(tmpi);
			array_init(tmpi);
		} else {
			tmpi = value;
		}

		for(i=0; i<image_info->info_list[section_index].count; i++) {
			info_data  = &image_info->info_list[section_index].list[i];
#ifdef EXIF_DEBUG
			info_tag   = info_data->tag; /* conversion */
#endif
			info_value = &info_data->value;
			if (!(name = info_data->name)) {
				snprintf(uname, sizeof(uname), "%d", unknown++);
				name = uname;
			}
#ifdef EXIF_DEBUG
/*		php_error_docref(NULL TSRMLS_CC, E_NOTICE, "Adding infos: tag(0x%04X,%12s,L=0x%04X): %s", info_tag, exif_get_tagname(info_tag, buffer, -12, exif_get_tag_table(section_index) TSRMLS_CC), info_data->length, info_data->format==TAG_FMT_STRING?(info_value&&info_value->s?info_value->s:"<no data>"):exif_get_tagformat(info_data->format));*/
#endif
			if (info_data->length==0) {
				add_assoc_null(tmpi, name);
			} else {
				switch (info_data->format) {
					default:
						/* Standard says more types possible but skip them...
						 * but allow users to handle data if they know how to
						 * So not return but use type UNDEFINED
						 * return;
						 */
					case TAG_FMT_BYTE:
					case TAG_FMT_SBYTE:
					case TAG_FMT_UNDEFINED:
						if (!info_value->s) {
							add_assoc_stringl(tmpi, name, "", 0, 1);
						} else {
							add_assoc_stringl(tmpi, name, info_value->s, info_data->length, 1);
						}
						break;

					case TAG_FMT_STRING:
						if (!(val = info_value->s)) {
							val = "";
						}
						if (section_index==SECTION_COMMENT) {
							add_index_string(tmpi, idx++, val, 1);
						} else {
							add_assoc_string(tmpi, name, val, 1);
						}
						break;

					case TAG_FMT_URATIONAL:
					case TAG_FMT_SRATIONAL:
					/*case TAG_FMT_BYTE:
					case TAG_FMT_SBYTE:*/
					case TAG_FMT_USHORT:
					case TAG_FMT_SSHORT:
					case TAG_FMT_SINGLE:
					case TAG_FMT_DOUBLE:
					case TAG_FMT_ULONG:
					case TAG_FMT_SLONG:
						/* now the rest, first see if it becomes an array */
						if ((l = info_data->length) > 1) {
							array = NULL;
							MAKE_STD_ZVAL(array);
							array_init(array);
						}
						for(ap=0; ap<l; ap++) {
							if (l>1) {
								info_value = &info_data->value.list[ap];
							}
							switch (info_data->format) {
								case TAG_FMT_BYTE:
									if (l>1) {
										info_value = &info_data->value;
										for (b=0;b<l;b++) {
											add_index_long(array, b, (int)(info_value->s[b]));
										}
										break;
									}
								case TAG_FMT_USHORT:
								case TAG_FMT_ULONG:
									if (l==1) {
										add_assoc_long(tmpi, name, (int)info_value->u);
									} else {
										add_index_long(array, ap, (int)info_value->u);
									}
									break;

								case TAG_FMT_URATIONAL:
									snprintf(buffer, sizeof(buffer), "%i/%i", info_value->ur.num, info_value->ur.den);
									if (l==1) {
										add_assoc_string(tmpi, name, buffer, 1);
									} else {
										add_index_string(array, ap, buffer, 1);
									}
									break;

								case TAG_FMT_SBYTE:
									if (l>1) {
										info_value = &info_data->value;
										for (b=0;b<l;b++) {
											add_index_long(array, ap, (int)info_value->s[b]);
										}
										break;
									}
								case TAG_FMT_SSHORT:
								case TAG_FMT_SLONG:
									if (l==1) {
										add_assoc_long(tmpi, name, info_value->i);
									} else {
										add_index_long(array, ap, info_value->i);
									}
									break;

								case TAG_FMT_SRATIONAL:
									snprintf(buffer, sizeof(buffer), "%i/%i", info_value->sr.num, info_value->sr.den);
									if (l==1) {
										add_assoc_string(tmpi, name, buffer, 1);
									} else {
										add_index_string(array, ap, buffer, 1);
									}
									break;

								case TAG_FMT_SINGLE:
									if (l==1) {
										add_assoc_double(tmpi, name, info_value->f);
									} else {
										add_index_double(array, ap, info_value->f);
									}
									break;

								case TAG_FMT_DOUBLE:
									if (l==1) {
										add_assoc_double(tmpi, name, info_value->d);
									} else {
										add_index_double(array, ap, info_value->d);
									}
									break;
							}
							info_value = &info_data->value.list[ap];
						}
						if (l>1) {
							add_assoc_zval(tmpi, name, array);
						}
						break;
				}
			}
		}
		if (sub_array) {
			add_assoc_zval(value, exif_get_sectionname(section_index), tmpi);
		}
	}
}
/* }}} */

/* {{{ Markers
   JPEG markers consist of one or more 0xFF bytes, followed by a marker
   code byte (which is not an FF).  Here are the marker codes of interest
   in this program.  (See jdmarker.c for a more complete list.)
*/

#define M_TEM   0x01    /* temp for arithmetic coding              */
#define M_RES   0x02    /* reserved                                */
#define M_SOF0  0xC0    /* Start Of Frame N                        */
#define M_SOF1  0xC1    /* N indicates which compression process   */
#define M_SOF2  0xC2    /* Only SOF0-SOF2 are now in common use    */
#define M_SOF3  0xC3
#define M_DHT   0xC4
#define M_SOF5  0xC5    /* NB: codes C4 and CC are NOT SOF markers */
#define M_SOF6  0xC6
#define M_SOF7  0xC7
#define M_JPEG  0x08    /* reserved for extensions                 */
#define M_SOF9  0xC9
#define M_SOF10 0xCA
#define M_SOF11 0xCB
#define M_DAC   0xCC    /* arithmetic table                         */
#define M_SOF13 0xCD
#define M_SOF14 0xCE
#define M_SOF15 0xCF
#define M_RST0  0xD0    /* restart segment                          */
#define M_RST1  0xD1
#define M_RST2  0xD2
#define M_RST3  0xD3
#define M_RST4  0xD4
#define M_RST5  0xD5
#define M_RST6  0xD6
#define M_RST7  0xD7
#define M_SOI   0xD8    /* Start Of Image (beginning of datastream) */
#define M_EOI   0xD9    /* End Of Image (end of datastream)         */
#define M_SOS   0xDA    /* Start Of Scan (begins compressed data)   */
#define M_DQT   0xDB
#define M_DNL   0xDC
#define M_DRI   0xDD
#define M_DHP   0xDE
#define M_EXP   0xDF
#define M_APP0  0xE0    /* JPEG: 'JFIFF' AND (additional 'JFXX')    */
#define M_EXIF  0xE1    /* Exif Attribute Information               */
#define M_APP2  0xE2    /* Flash Pix Extension Data?                */
#define M_APP3  0xE3
#define M_APP4  0xE4
#define M_APP5  0xE5
#define M_APP6  0xE6
#define M_APP7  0xE7
#define M_APP8  0xE8
#define M_APP9  0xE9
#define M_APP10 0xEA
#define M_APP11 0xEB
#define M_APP12 0xEC
#define M_APP13 0xED    /* IPTC International Press Telecommunications Council */
#define M_APP14 0xEE    /* Software, Copyright?                     */
#define M_APP15 0xEF
#define M_JPG0  0xF0
#define M_JPG1  0xF1
#define M_JPG2  0xF2
#define M_JPG3  0xF3
#define M_JPG4  0xF4
#define M_JPG5  0xF5
#define M_JPG6  0xF6
#define M_JPG7  0xF7
#define M_JPG8  0xF8
#define M_JPG9  0xF9
#define M_JPG10 0xFA
#define M_JPG11 0xFB
#define M_JPG12 0xFC
#define M_JPG13 0xFD
#define M_COM   0xFE    /* COMment                                  */

#define M_PSEUDO 0x123 	/* Extra value.                             */

/* }}} */

/* {{{ jpeg2000 markers
 */
/* Markers x30 - x3F do not have a segment */
/* Markers x00, x01, xFE, xC0 - xDF ISO/IEC 10918-1 -> M_<xx> */
/* Markers xF0 - xF7 ISO/IEC 10918-3 */
/* Markers xF7 - xF8 ISO/IEC 14495-1 */
/* XY=Main/Tile-header:(R:required, N:not_allowed, O:optional, L:last_marker) */
#define JC_SOC   0x4F   /* NN, Start of codestream                          */
#define JC_SIZ   0x51   /* RN, Image and tile size                          */
#define JC_COD   0x52   /* RO, Codeing style defaulte                       */
#define JC_COC   0x53   /* OO, Coding style component                       */
#define JC_TLM   0x55   /* ON, Tile part length main header                 */
#define JC_PLM   0x57   /* ON, Packet length main header                    */
#define JC_PLT   0x58   /* NO, Packet length tile part header               */
#define JC_QCD   0x5C   /* RO, Quantization default                         */
#define JC_QCC   0x5D   /* OO, Quantization component                       */
#define JC_RGN   0x5E   /* OO, Region of interest                           */
#define JC_POD   0x5F   /* OO, Progression order default                    */
#define JC_PPM   0x60   /* ON, Packed packet headers main header            */
#define JC_PPT   0x61   /* NO, Packet packet headers tile part header       */
#define JC_CME   0x64   /* OO, Comment: "LL E <text>" E=0:binary, E=1:ascii */
#define JC_SOT   0x90   /* NR, Start of tile                                */
#define JC_SOP   0x91   /* NO, Start of packeter default                    */
#define JC_EPH   0x92   /* NO, End of packet header                         */
#define JC_SOD   0x93   /* NL, Start of data                                */
#define JC_EOC   0xD9   /* NN, End of codestream                            */
/* }}} */

/* {{{ exif_process_COM
   Process a COM marker.
   We want to print out the marker contents as legible text;
   we must guard against random junk and varying newline representations.
*/
static void exif_process_COM (image_info_type *image_info, char *value, size_t length TSRMLS_DC)
{
	exif_iif_add_tag(image_info, SECTION_COMMENT, "Comment", TAG_COMPUTED_VALUE, TAG_FMT_STRING, length-2, value+2 TSRMLS_CC);
}
/* }}} */

/* {{{ exif_process_CME
   Process a CME marker.
   We want to print out the marker contents as legible text;
   we must guard against random junk and varying newline representations.
*/
#ifdef EXIF_JPEG2000
static void exif_process_CME (image_info_type *image_info, char *value, size_t length TSRMLS_DC)
{
	if (length>3) {
		switch(value[2]) {
			case 0:
				exif_iif_add_tag(image_info, SECTION_COMMENT, "Comment", TAG_COMPUTED_VALUE, TAG_FMT_UNDEFINED, length, value TSRMLS_CC);
				break;
			case 1:
				exif_iif_add_tag(image_info, SECTION_COMMENT, "Comment", TAG_COMPUTED_VALUE, TAG_FMT_STRING, length, value);
				break;
			default:
				php_error_docref(NULL TSRMLS_CC, E_NOTICE, "Undefined JPEG2000 comment encoding");
				break;
		}
	} else {
		exif_iif_add_tag(image_info, SECTION_COMMENT, "Comment", TAG_COMPUTED_VALUE, TAG_FMT_UNDEFINED, 0, NULL);
		php_error_docref(NULL TSRMLS_CC, E_NOTICE, "JPEG2000 comment section too small");
	}
}
#endif
/* }}} */

/* {{{ exif_process_SOFn
 * Process a SOFn marker.  This is useful for the image dimensions */
static void exif_process_SOFn (uchar *Data, int marker, jpeg_sof_info *result)
{
/* 0xFF SOSn SectLen(2) Bits(1) Height(2) Width(2) Channels(1)  3*Channels (1)  */
	result->bits_per_sample = Data[2];
	result->height          = php_jpg_get16(Data+3);
	result->width           = php_jpg_get16(Data+5);
	result->num_components  = Data[7];

/*	switch (marker) {
		case M_SOF0:  process = "Baseline";  break;
		case M_SOF1:  process = "Extended sequential";  break;
		case M_SOF2:  process = "Progressive";  break;
		case M_SOF3:  process = "Lossless";  break;
		case M_SOF5:  process = "Differential sequential";  break;
		case M_SOF6:  process = "Differential progressive";  break;
		case M_SOF7:  process = "Differential lossless";  break;
		case M_SOF9:  process = "Extended sequential, arithmetic coding";  break;
		case M_SOF10: process = "Progressive, arithmetic coding";  break;
		case M_SOF11: process = "Lossless, arithmetic coding";  break;
		case M_SOF13: process = "Differential sequential, arithmetic coding";  break;
		case M_SOF14: process = "Differential progressive, arithmetic coding"; break;
		case M_SOF15: process = "Differential lossless, arithmetic coding";  break;
		default:      process = "Unknown";  break;
	}*/
}
/* }}} */

/* forward declarations */
static int exif_process_IFD_in_JPEG(image_info_type *ImageInfo, char *dir_start, char *offset_base, size_t IFDlength, size_t displacement, int section_index TSRMLS_DC);
static int exif_process_IFD_TAG(    image_info_type *ImageInfo, char *dir_entry, char *offset_base, size_t IFDlength, size_t displacement, int section_index, int ReadNextIFD, tag_table_type tag_table TSRMLS_DC);

/* {{{ exif_get_markername
	Get name of marker */
#ifdef EXIF_DEBUG
static char * exif_get_markername(int marker)
{
	switch(marker) {
		case 0xC0: return "SOF0";
		case 0xC1: return "SOF1";
		case 0xC2: return "SOF2";
		case 0xC3: return "SOF3";
		case 0xC4: return "DHT";
		case 0xC5: return "SOF5";
		case 0xC6: return "SOF6";
		case 0xC7: return "SOF7";
		case 0xC9: return "SOF9";
		case 0xCA: return "SOF10";
		case 0xCB: return "SOF11";
		case 0xCD: return "SOF13";
		case 0xCE: return "SOF14";
		case 0xCF: return "SOF15";
		case 0xD8: return "SOI";
		case 0xD9: return "EOI";
		case 0xDA: return "SOS";
		case 0xDB: return "DQT";
		case 0xDC: return "DNL";
		case 0xDD: return "DRI";
		case 0xDE: return "DHP";
		case 0xDF: return "EXP";
		case 0xE0: return "APP0";
		case 0xE1: return "EXIF";
		case 0xE2: return "FPIX";
		case 0xE3: return "APP3";
		case 0xE4: return "APP4";
		case 0xE5: return "APP5";
		case 0xE6: return "APP6";
		case 0xE7: return "APP7";
		case 0xE8: return "APP8";
		case 0xE9: return "APP9";
		case 0xEA: return "APP10";
		case 0xEB: return "APP11";
		case 0xEC: return "APP12";
		case 0xED: return "APP13";
		case 0xEE: return "APP14";
		case 0xEF: return "APP15";
		case 0xF0: return "JPG0";
		case 0xFD: return "JPG13";
		case 0xFE: return "COM";
		case 0x01: return "TEM";
	}
	return "Unknown";
}
#endif
/* }}} */

/* {{{ proto string exif_tagname(index)
	Get headername for index or false if not defined */
PHP_FUNCTION(exif_tagname)
{
	long tag;
	char *szTemp;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "l", &tag) == FAILURE) {
		return;
	}

	szTemp = exif_get_tagname(tag, NULL, 0, tag_table_IFD TSRMLS_CC);

	if (tag < 0 || !szTemp || !szTemp[0]) {
		RETURN_FALSE;
	}

	RETURN_STRING(szTemp, 1)
}
/* }}} */

/* {{{ exif_ifd_make_value
 * Create a value for an ifd from an info_data pointer */
static void* exif_ifd_make_value(image_info_data *info_data, int motorola_intel TSRMLS_DC) {
	size_t  byte_count;
	char    *value_ptr, *data_ptr;
	size_t  i;

	image_info_value  *info_value;

	byte_count = php_tiff_bytes_per_format[info_data->format] * info_data->length;
	value_ptr = safe_emalloc(max(byte_count, 4), 1, 0);
	memset(value_ptr, 0, 4);
	if (!info_data->length) {
		return value_ptr;
	}
	if (info_data->format == TAG_FMT_UNDEFINED || info_data->format == TAG_FMT_STRING
	  || (byte_count>1 && (info_data->format == TAG_FMT_BYTE || info_data->format == TAG_FMT_SBYTE))
	) {
		memmove(value_ptr, info_data->value.s, byte_count);
		return value_ptr;
	} else if (info_data->format == TAG_FMT_BYTE) {
		*value_ptr = info_data->value.u;
		return value_ptr;
	} else if (info_data->format == TAG_FMT_SBYTE) {
		*value_ptr = info_data->value.i;
		return value_ptr;
	} else {
		data_ptr = value_ptr;
		for(i=0; i<info_data->length; i++) {
			if (info_data->length==1) {
				info_value = &info_data->value;
			} else {
				info_value = &info_data->value.list[i];
			}
			switch(info_data->format) {
				case TAG_FMT_USHORT:
					php_ifd_set16u(data_ptr, info_value->u, motorola_intel);
					data_ptr += 2;
					break;
				case TAG_FMT_ULONG:
					php_ifd_set32u(data_ptr, info_value->u, motorola_intel);
					data_ptr += 4;
					break;
				case TAG_FMT_SSHORT:
					php_ifd_set16u(data_ptr, info_value->i, motorola_intel);
					data_ptr += 2;
					break;
				case TAG_FMT_SLONG:
					php_ifd_set32u(data_ptr, info_value->i, motorola_intel);
					data_ptr += 4;
					break;
				case TAG_FMT_URATIONAL:
					php_ifd_set32u(data_ptr,   info_value->sr.num, motorola_intel);
					php_ifd_set32u(data_ptr+4, info_value->sr.den, motorola_intel);
					data_ptr += 8;
					break;
				case TAG_FMT_SRATIONAL:
					php_ifd_set32u(data_ptr,   info_value->ur.num, motorola_intel);
					php_ifd_set32u(data_ptr+4, info_value->ur.den, motorola_intel);
					data_ptr += 8;
					break;
				case TAG_FMT_SINGLE:
					memmove(data_ptr, &info_value->f, 4);
					data_ptr += 4;
					break;
				case TAG_FMT_DOUBLE:
					memmove(data_ptr, &info_value->d, 8);
					data_ptr += 8;
					break;
			}
		}
	}
	return value_ptr;
}
/* }}} */

/* {{{ exif_thumbnail_build
 * Check and build thumbnail */
static void exif_thumbnail_build(image_info_type *ImageInfo TSRMLS_DC) {
	size_t            new_size, new_move, new_value;
	char              *new_data;
	void              *value_ptr;
	int               i, byte_count;
	image_info_list   *info_list;
	image_info_data   *info_data;
#ifdef EXIF_DEBUG
	char              tagname[64];
#endif

	if (!ImageInfo->read_thumbnail || !ImageInfo->Thumbnail.offset || !ImageInfo->Thumbnail.size) {
		return; /* ignore this call */
	}
#ifdef EXIF_DEBUG
	exif_error_docref(NULL EXIFERR_CC, ImageInfo, E_NOTICE, "Thumbnail: filetype = %d", ImageInfo->Thumbnail.filetype);
#endif
	switch(ImageInfo->Thumbnail.filetype) {
		default:
		case IMAGE_FILETYPE_JPEG:
			/* done */
			break;
		case IMAGE_FILETYPE_TIFF_II:
		case IMAGE_FILETYPE_TIFF_MM:
			info_list = &ImageInfo->info_list[SECTION_THUMBNAIL];
			new_size  = 8 + 2 + info_list->count * 12 + 4;
#ifdef EXIF_DEBUG
			exif_error_docref(NULL EXIFERR_CC, ImageInfo, E_NOTICE, "Thumbnail: size of signature + directory(%d): 0x%02X", info_list->count, new_size);
#endif
			new_value= new_size; /* offset for ifd values outside ifd directory */
			for (i=0; i<info_list->count; i++) {
				info_data  = &info_list->list[i];
				byte_count = php_tiff_bytes_per_format[info_data->format] * info_data->length;
				if (byte_count > 4) {
					new_size += byte_count;
				}
			}
			new_move = new_size;
			new_data = safe_erealloc(ImageInfo->Thumbnail.data, 1, ImageInfo->Thumbnail.size, new_size);
			ImageInfo->Thumbnail.data = new_data;
			memmove(ImageInfo->Thumbnail.data + new_move, ImageInfo->Thumbnail.data, ImageInfo->Thumbnail.size);
			ImageInfo->Thumbnail.size += new_size;
			/* fill in data */
			if (ImageInfo->motorola_intel) {
				memmove(new_data, "MM\x00\x2a\x00\x00\x00\x08", 8);
			} else {
				memmove(new_data, "II\x2a\x00\x08\x00\x00\x00", 8);
			}
			new_data += 8;
			php_ifd_set16u(new_data, info_list->count, ImageInfo->motorola_intel);
			new_data += 2;
			for (i=0; i<info_list->count; i++) {
				info_data  = &info_list->list[i];
				byte_count = php_tiff_bytes_per_format[info_data->format] * info_data->length;
#ifdef EXIF_DEBUG
				exif_error_docref(NULL EXIFERR_CC, ImageInfo, E_NOTICE, "Thumbnail: process tag(x%04X=%s): %s%s (%d bytes)", info_data->tag, exif_get_tagname(info_data->tag, tagname, -12, tag_table_IFD TSRMLS_CC), (info_data->length>1)&&info_data->format!=TAG_FMT_UNDEFINED&&info_data->format!=TAG_FMT_STRING?"ARRAY OF ":"", exif_get_tagformat(info_data->format), byte_count);
#endif
				if (info_data->tag==TAG_STRIP_OFFSETS || info_data->tag==TAG_JPEG_INTERCHANGE_FORMAT) {
					php_ifd_set16u(new_data + 0, info_data->tag,    ImageInfo->motorola_intel);
					php_ifd_set16u(new_data + 2, TAG_FMT_ULONG,     ImageInfo->motorola_intel);
					php_ifd_set32u(new_data + 4, 1,                 ImageInfo->motorola_intel);
					php_ifd_set32u(new_data + 8, new_move,          ImageInfo->motorola_intel);
				} else {
					php_ifd_set16u(new_data + 0, info_data->tag,    ImageInfo->motorola_intel);
					php_ifd_set16u(new_data + 2, info_data->format, ImageInfo->motorola_intel);
					php_ifd_set32u(new_data + 4, info_data->length, ImageInfo->motorola_intel);
					value_ptr  = exif_ifd_make_value(info_data, ImageInfo->motorola_intel TSRMLS_CC);
					if (byte_count <= 4) {
						memmove(new_data+8, value_ptr, 4);
					} else {
						php_ifd_set32u(new_data+8, new_value, ImageInfo->motorola_intel);
#ifdef EXIF_DEBUG
						exif_error_docref(NULL EXIFERR_CC, ImageInfo, E_NOTICE, "Thumbnail: writing with value offset: 0x%04X + 0x%02X", new_value, byte_count);
#endif
						memmove(ImageInfo->Thumbnail.data+new_value, value_ptr, byte_count);
						new_value += byte_count;
					}
					efree(value_ptr);
				}
				new_data += 12;
			}
			memset(new_data, 0, 4); /* next ifd pointer */
#ifdef EXIF_DEBUG
			exif_error_docref(NULL EXIFERR_CC, ImageInfo, E_NOTICE, "Thumbnail: created");
#endif
			break;
	}
}
/* }}} */

/* {{{ exif_thumbnail_extract
 * Grab the thumbnail, corrected */
static void exif_thumbnail_extract(image_info_type *ImageInfo, char *offset, size_t length TSRMLS_DC) {
	if (ImageInfo->Thumbnail.data) {
		exif_error_docref("exif_read_data#error_mult_thumb" EXIFERR_CC, ImageInfo, E_WARNING, "Multiple possible thumbnails");
		return; /* Should not happen */
	}
	if (!ImageInfo->read_thumbnail)	{
		return; /* ignore this call */
	}
	/* according to exif2.1, the thumbnail is not supposed to be greater than 64K */
	if (ImageInfo->Thumbnail.size >= 65536
	 || ImageInfo->Thumbnail.size <= 0
	 || ImageInfo->Thumbnail.offset <= 0
	) {
		exif_error_docref(NULL EXIFERR_CC, ImageInfo, E_WARNING, "Illegal thumbnail size/offset");
		return;
	}
	/* Check to make sure we are not going to go past the ExifLength */
	if ((ImageInfo->Thumbnail.offset + ImageInfo->Thumbnail.size) > length) {
		EXIF_ERRLOG_THUMBEOF(ImageInfo)
		return;
	}
	ImageInfo->Thumbnail.data = estrndup(offset + ImageInfo->Thumbnail.offset, ImageInfo->Thumbnail.size);
	exif_thumbnail_build(ImageInfo TSRMLS_CC);
}
/* }}} */

/* {{{ exif_process_undefined
 * Copy a string/buffer in Exif header to a character string and return length of allocated buffer if any. */
static int exif_process_undefined(char **result, char *value, size_t byte_count TSRMLS_DC) {
	/* we cannot use strlcpy - here the problem is that we have to copy NUL
	 * chars up to byte_count, we also have to add a single NUL character to
	 * force end of string.
	 * estrndup does not return length
	 */
	if (byte_count) {
		(*result) = estrndup(value, byte_count); /* NULL @ byte_count!!! */
		return byte_count+1;
	}
	return 0;
}
/* }}} */

/* {{{ exif_process_string_raw
 * Copy a string in Exif header to a character string returns length of allocated buffer if any. */
static int exif_process_string_raw(char **result, char *value, size_t byte_count) {
	/* we cannot use strlcpy - here the problem is that we have to copy NUL
	 * chars up to byte_count, we also have to add a single NUL character to
	 * force end of string.
	 */
	if (byte_count) {
		(*result) = safe_emalloc(byte_count, 1, 1);
		memcpy(*result, value, byte_count);
		(*result)[byte_count] = '\0';
		return byte_count+1;
	}
	return 0;
}
/* }}} */

/* {{{ exif_process_string
 * Copy a string in Exif header to a character string and return length of allocated buffer if any.
 * In contrast to exif_process_string this function does always return a string buffer */
static int exif_process_string(char **result, char *value, size_t byte_count TSRMLS_DC) {
	/* we cannot use strlcpy - here the problem is that we cannot use strlen to
	 * determin length of string and we cannot use strlcpy with len=byte_count+1
	 * because then we might get into an EXCEPTION if we exceed an allocated
	 * memory page...so we use php_strnlen in conjunction with memcpy and add the NUL
	 * char.
	 * estrdup would sometimes allocate more memory and does not return length
	 */
	if ((byte_count=php_strnlen(value, byte_count)) > 0) {
		return exif_process_undefined(result, value, byte_count TSRMLS_CC);
	}
	(*result) = estrndup("", 1); /* force empty string */
	return byte_count+1;
}
/* }}} */

/* {{{ exif_process_user_comment
 * Process UserComment in IFD. */
static int exif_process_user_comment(image_info_type *ImageInfo, char **pszInfoPtr, char **pszEncoding, char *szValuePtr, int ByteCount TSRMLS_DC)
{
	int   a;
	char  *decode;
	size_t len;;

	*pszEncoding = NULL;
	/* Copy the comment */
	if (ByteCount>=8) {
		if (!memcmp(szValuePtr, "UNICODE\0", 8)) {
			*pszEncoding = estrdup((const char*)szValuePtr);
			szValuePtr = szValuePtr+8;
			ByteCount -= 8;
			/* First try to detect BOM: ZERO WIDTH NOBREAK SPACE (FEFF 16)
			 * since we have no encoding support for the BOM yet we skip that.
			 */
			if (!memcmp(szValuePtr, "\xFE\xFF", 2)) {
				decode = "UCS-2BE";
				szValuePtr = szValuePtr+2;
				ByteCount -= 2;
			} else if (!memcmp(szValuePtr, "\xFF\xFE", 2)) {
				decode = "UCS-2LE";
				szValuePtr = szValuePtr+2;
				ByteCount -= 2;
			} else if (ImageInfo->motorola_intel) {
				decode = ImageInfo->decode_unicode_be;
			} else {
				decode = ImageInfo->decode_unicode_le;
			}
			/* XXX this will fail again if encoding_converter returns on error something different than SIZE_MAX   */
			if (zend_multibyte_encoding_converter(
					(unsigned char**)pszInfoPtr,
					&len,
					(unsigned char*)szValuePtr,
					ByteCount,
					zend_multibyte_fetch_encoding(ImageInfo->encode_unicode TSRMLS_CC),
					zend_multibyte_fetch_encoding(decode TSRMLS_CC)
					TSRMLS_CC) == (size_t)-1) {
				len = exif_process_string_raw(pszInfoPtr, szValuePtr, ByteCount);
			}
			return len;
		} else if (!memcmp(szValuePtr, "ASCII\0\0\0", 8)) {
			*pszEncoding = estrdup((const char*)szValuePtr);
			szValuePtr = szValuePtr+8;
			ByteCount -= 8;
		} else if (!memcmp(szValuePtr, "JIS\0\0\0\0\0", 8)) {
			/* JIS should be tanslated to MB or we leave it to the user - leave it to the user */
			*pszEncoding = estrdup((const char*)szValuePtr);
			szValuePtr = szValuePtr+8;
			ByteCount -= 8;
			/* XXX this will fail again if encoding_converter returns on error something different than SIZE_MAX   */
			if (zend_multibyte_encoding_converter(
					(unsigned char**)pszInfoPtr,
					&len,
					(unsigned char*)szValuePtr,
					ByteCount,
					zend_multibyte_fetch_encoding(ImageInfo->encode_jis TSRMLS_CC),
					zend_multibyte_fetch_encoding(ImageInfo->motorola_intel ? ImageInfo->decode_jis_be : ImageInfo->decode_jis_le TSRMLS_CC)
					TSRMLS_CC) == (size_t)-1) {
				len = exif_process_string_raw(pszInfoPtr, szValuePtr, ByteCount);
			}
			return len;
		} else if (!memcmp(szValuePtr, "\0\0\0\0\0\0\0\0", 8)) {
			/* 8 NULL means undefined and should be ASCII... */
			*pszEncoding = estrdup("UNDEFINED");
			szValuePtr = szValuePtr+8;
			ByteCount -= 8;
		}
	}

	/* Olympus has this padded with trailing spaces.  Remove these first. */
	if (ByteCount>0) {
		for (a=ByteCount-1;a && szValuePtr[a]==' ';a--) {
			(szValuePtr)[a] = '\0';
		}
	}

	/* normal text without encoding */
	exif_process_string(pszInfoPtr, szValuePtr, ByteCount TSRMLS_CC);
	return strlen(*pszInfoPtr);
}
/* }}} */

/* {{{ exif_process_unicode
 * Process unicode field in IFD. */
static int exif_process_unicode(image_info_type *ImageInfo, xp_field_type *xp_field, int tag, char *szValuePtr, int ByteCount TSRMLS_DC)
{
	xp_field->tag = tag;
	xp_field->value = NULL;
	/* XXX this will fail again if encoding_converter returns on error something different than SIZE_MAX   */
	if (zend_multibyte_encoding_converter(
			(unsigned char**)&xp_field->value,
			&xp_field->size,
			(unsigned char*)szValuePtr,
			ByteCount,
			zend_multibyte_fetch_encoding(ImageInfo->encode_unicode TSRMLS_CC),
			zend_multibyte_fetch_encoding(ImageInfo->motorola_intel ? ImageInfo->decode_unicode_be : ImageInfo->decode_unicode_le TSRMLS_CC)
			TSRMLS_CC) == (size_t)-1) {
		xp_field->size = exif_process_string_raw(&xp_field->value, szValuePtr, ByteCount);
	}
	return xp_field->size;
}
/* }}} */

/* {{{ exif_process_IFD_in_MAKERNOTE
 * Process nested IFDs directories in Maker Note. */
static int exif_process_IFD_in_MAKERNOTE(image_info_type *ImageInfo, char * value_ptr, int value_len, char *offset_base, size_t IFDlength, size_t displacement TSRMLS_DC)
{
	int de, i=0, section_index = SECTION_MAKERNOTE;
	int NumDirEntries, old_motorola_intel, offset_diff;
	const maker_note_type *maker_note;
	char *dir_start;

	for (i=0; i<=sizeof(maker_note_array)/sizeof(maker_note_type); i++) {
		if (i==sizeof(maker_note_array)/sizeof(maker_note_type))
			return FALSE;
		maker_note = maker_note_array+i;

		/*exif_error_docref(NULL EXIFERR_CC, ImageInfo, E_NOTICE, "check (%s,%s)", maker_note->make?maker_note->make:"", maker_note->model?maker_note->model:"");*/
		if (maker_note->make && (!ImageInfo->make || strcmp(maker_note->make, ImageInfo->make)))
			continue;
		if (maker_note->model && (!ImageInfo->model || strcmp(maker_note->model, ImageInfo->model)))
			continue;
		if (maker_note->id_string && strncmp(maker_note->id_string, value_ptr, maker_note->id_string_len))
			continue;
		break;
	}

	dir_start = value_ptr + maker_note->offset;

#ifdef EXIF_DEBUG
	exif_error_docref(NULL EXIFERR_CC, ImageInfo, E_NOTICE, "Process %s @x%04X + 0x%04X=%d: %s", exif_get_sectionname(section_index), (int)dir_start-(int)offset_base+maker_note->offset+displacement, value_len, value_len, exif_char_dump(value_ptr, value_len, (int)dir_start-(int)offset_base+maker_note->offset+displacement));
#endif

	ImageInfo->sections_found |= FOUND_MAKERNOTE;

	old_motorola_intel = ImageInfo->motorola_intel;
	switch (maker_note->byte_order) {
		case MN_ORDER_INTEL:
			ImageInfo->motorola_intel = 0;
			break;
		case MN_ORDER_MOTOROLA:
			ImageInfo->motorola_intel = 1;
			break;
		default:
		case MN_ORDER_NORMAL:
			break;
	}

	NumDirEntries = php_ifd_get16u(dir_start, ImageInfo->motorola_intel);

	switch (maker_note->offset_mode) {
		case MN_OFFSET_MAKER:
			offset_base = value_ptr;
			break;
		case MN_OFFSET_GUESS:
			offset_diff = 2 + NumDirEntries*12 + 4 - php_ifd_get32u(dir_start+10, ImageInfo->motorola_intel);
#ifdef EXIF_DEBUG
			exif_error_docref(NULL EXIFERR_CC, ImageInfo, E_NOTICE, "Using automatic offset correction: 0x%04X", ((int)dir_start-(int)offset_base+maker_note->offset+displacement) + offset_diff);
#endif
			offset_base = value_ptr + offset_diff;
			break;
		default:
		case MN_OFFSET_NORMAL:
			break;
	}

	if ((2+NumDirEntries*12) > value_len) {
		exif_error_docref("exif_read_data#error_ifd" EXIFERR_CC, ImageInfo, E_WARNING, "Illegal IFD size: 2 + x%04X*12 = x%04X > x%04X", NumDirEntries, 2+NumDirEntries*12, value_len);
		return FALSE;
	}

	for (de=0;de<NumDirEntries;de++) {
		if (!exif_process_IFD_TAG(ImageInfo, dir_start + 2 + 12 * de,
								  offset_base, IFDlength, displacement, section_index, 0, maker_note->tag_table TSRMLS_CC)) {
			return FALSE;
		}
	}
	ImageInfo->motorola_intel = old_motorola_intel;
/*	NextDirOffset (must be NULL) = php_ifd_get32u(dir_start+2+12*de, ImageInfo->motorola_intel);*/
#ifdef EXIF_DEBUG
	exif_error_docref(NULL EXIFERR_CC, ImageInfo, E_NOTICE, "Subsection %s done", exif_get_sectionname(SECTION_MAKERNOTE));
#endif
	return TRUE;
}
/* }}} */

/* {{{ exif_process_IFD_TAG
 * Process one of the nested IFDs directories. */
static int exif_process_IFD_TAG(image_info_type *ImageInfo, char *dir_entry, char *offset_base, size_t IFDlength, size_t displacement, int section_index, int ReadNextIFD, tag_table_type tag_table TSRMLS_DC)
{
	size_t length;
	int tag, format, components;
	char *value_ptr, tagname[64], cbuf[32], *outside=NULL;
	size_t byte_count, offset_val, fpos, fgot;
	int64_t byte_count_signed;
	xp_field_type *tmp_xp;
#ifdef EXIF_DEBUG
	char *dump_data;
	int dump_free;
#endif /* EXIF_DEBUG */

	/* Protect against corrupt headers */
	if (ImageInfo->ifd_nesting_level > MAX_IFD_NESTING_LEVEL) {
		exif_error_docref("exif_read_data#error_ifd" EXIFERR_CC, ImageInfo, E_WARNING, "corrupt EXIF header: maximum directory nesting level reached");
		return FALSE;
	}
	ImageInfo->ifd_nesting_level++;

	tag = php_ifd_get16u(dir_entry, ImageInfo->motorola_intel);
	format = php_ifd_get16u(dir_entry+2, ImageInfo->motorola_intel);
	components = php_ifd_get32u(dir_entry+4, ImageInfo->motorola_intel);

	if (!format || format > NUM_FORMATS) {
		/* (-1) catches illegal zero case as unsigned underflows to positive large. */
		exif_error_docref("exif_read_data#error_ifd" EXIFERR_CC, ImageInfo, E_WARNING, "Process tag(x%04X=%s): Illegal format code 0x%04X, suppose BYTE", tag, exif_get_tagname(tag, tagname, -12, tag_table TSRMLS_CC), format);
		format = TAG_FMT_BYTE;
		/*return TRUE;*/
	}

	if (components < 0) {
		exif_error_docref("exif_read_data#error_ifd" EXIFERR_CC, ImageInfo, E_WARNING, "Process tag(x%04X=%s): Illegal components(%ld)", tag, exif_get_tagname(tag, tagname, -12, tag_table TSRMLS_CC), components);
		return FALSE;
	}

	byte_count_signed = (int64_t)components * php_tiff_bytes_per_format[format];

	if (byte_count_signed < 0 || (byte_count_signed > INT32_MAX)) {
		exif_error_docref("exif_read_data#error_ifd" EXIFERR_CC, ImageInfo, E_WARNING, "Process tag(x%04X=%s): Illegal byte_count", tag, exif_get_tagname(tag, tagname, -12, tag_table TSRMLS_CC));
		return FALSE;
	}

	byte_count = (size_t)byte_count_signed;

	if (byte_count > 4) {
		offset_val = php_ifd_get32u(dir_entry+8, ImageInfo->motorola_intel);
		/* If its bigger than 4 bytes, the dir entry contains an offset. */
		value_ptr = offset_base+offset_val;
        /*
            dir_entry is ImageInfo->file.list[sn].data+2+i*12
            offset_base is ImageInfo->file.list[sn].data-dir_offset
            dir_entry - offset_base is dir_offset+2+i*12
        */
		if (byte_count > IFDlength || offset_val > IFDlength-byte_count || value_ptr < dir_entry || offset_val < (size_t)(dir_entry-offset_base)) {
			/* It is important to check for IMAGE_FILETYPE_TIFF
			 * JPEG does not use absolute pointers instead its pointers are
			 * relative to the start of the TIFF header in APP1 section. */
			if (byte_count > ImageInfo->FileSize || offset_val>ImageInfo->FileSize-byte_count || (ImageInfo->FileType!=IMAGE_FILETYPE_TIFF_II && ImageInfo->FileType!=IMAGE_FILETYPE_TIFF_MM && ImageInfo->FileType!=IMAGE_FILETYPE_JPEG)) {
				if (value_ptr < dir_entry) {
					/* we can read this if offset_val > 0 */
					/* some files have their values in other parts of the file */
					exif_error_docref("exif_read_data#error_ifd" EXIFERR_CC, ImageInfo, E_WARNING, "Process tag(x%04X=%s): Illegal pointer offset(x%04X < x%04X)", tag, exif_get_tagname(tag, tagname, -12, tag_table TSRMLS_CC), offset_val, dir_entry);
				} else {
					/* this is for sure not allowed */
					/* exception are IFD pointers */
					exif_error_docref("exif_read_data#error_ifd" EXIFERR_CC, ImageInfo, E_WARNING, "Process tag(x%04X=%s): Illegal pointer offset(x%04X + x%04X = x%04X > x%04X)", tag, exif_get_tagname(tag, tagname, -12, tag_table TSRMLS_CC), offset_val, byte_count, offset_val+byte_count, IFDlength);
				}
				return FALSE;
			}
			if (byte_count>sizeof(cbuf)) {
				/* mark as outside range and get buffer */
				value_ptr = safe_emalloc(byte_count, 1, 0);
				outside = value_ptr;
			} else {
				/* In most cases we only access a small range so
				 * it is faster to use a static buffer there
				 * BUT it offers also the possibility to have
				 * pointers read without the need to free them
				 * explicitley before returning. */
				memset(&cbuf, 0, sizeof(cbuf));
				value_ptr = cbuf;
			}

			fpos = php_stream_tell(ImageInfo->infile);
			php_stream_seek(ImageInfo->infile, offset_val, SEEK_SET);
			fgot = php_stream_tell(ImageInfo->infile);
			if (fgot!=offset_val) {
				EFREE_IF(outside);
				exif_error_docref(NULL EXIFERR_CC, ImageInfo, E_WARNING, "Wrong file pointer: 0x%08X != 0x%08X", fgot, offset_val);
				return FALSE;
			}
			fgot = php_stream_read(ImageInfo->infile, value_ptr, byte_count);
			php_stream_seek(ImageInfo->infile, fpos, SEEK_SET);
			if (fgot<byte_count) {
				EFREE_IF(outside);
				EXIF_ERRLOG_FILEEOF(ImageInfo)
				return FALSE;
			}
		}
	} else {
		/* 4 bytes or less and value is in the dir entry itself */
		value_ptr = dir_entry+8;
		offset_val= value_ptr-offset_base;
	}

	ImageInfo->sections_found |= FOUND_ANY_TAG;
#ifdef EXIF_DEBUG
	dump_data = exif_dump_data(&dump_free, format, components, length, ImageInfo->motorola_intel, value_ptr TSRMLS_CC);
	exif_error_docref(NULL EXIFERR_CC, ImageInfo, E_NOTICE, "Process tag(x%04X=%s,@x%04X + x%04X(=%d)): %s%s %s", tag, exif_get_tagname(tag, tagname, -12, tag_table TSRMLS_CC), offset_val+displacement, byte_count, byte_count, (components>1)&&format!=TAG_FMT_UNDEFINED&&format!=TAG_FMT_STRING?"ARRAY OF ":"", exif_get_tagformat(format), dump_data);
	if (dump_free) {
		efree(dump_data);
	}
#endif

	if (section_index==SECTION_THUMBNAIL) {
		if (!ImageInfo->Thumbnail.data) {
			switch(tag) {
				case TAG_IMAGEWIDTH:
				case TAG_COMP_IMAGE_WIDTH:
					ImageInfo->Thumbnail.width = exif_convert_any_to_int(value_ptr, format, ImageInfo->motorola_intel TSRMLS_CC);
					break;

				case TAG_IMAGEHEIGHT:
				case TAG_COMP_IMAGE_HEIGHT:
					ImageInfo->Thumbnail.height = exif_convert_any_to_int(value_ptr, format, ImageInfo->motorola_intel TSRMLS_CC);
					break;

				case TAG_STRIP_OFFSETS:
				case TAG_JPEG_INTERCHANGE_FORMAT:
					/* accept both formats */
					ImageInfo->Thumbnail.offset = exif_convert_any_to_int(value_ptr, format, ImageInfo->motorola_intel TSRMLS_CC);
					break;

				case TAG_STRIP_BYTE_COUNTS:
					if (ImageInfo->FileType == IMAGE_FILETYPE_TIFF_II || ImageInfo->FileType == IMAGE_FILETYPE_TIFF_MM) {
						ImageInfo->Thumbnail.filetype = ImageInfo->FileType;
					} else {
						/* motorola is easier to read */
						ImageInfo->Thumbnail.filetype = IMAGE_FILETYPE_TIFF_MM;
					}
					ImageInfo->Thumbnail.size = exif_convert_any_to_int(value_ptr, format, ImageInfo->motorola_intel TSRMLS_CC);
					break;

				case TAG_JPEG_INTERCHANGE_FORMAT_LEN:
					if (ImageInfo->Thumbnail.filetype == IMAGE_FILETYPE_UNKNOWN) {
						ImageInfo->Thumbnail.filetype = IMAGE_FILETYPE_JPEG;
						ImageInfo->Thumbnail.size = exif_convert_any_to_int(value_ptr, format, ImageInfo->motorola_intel TSRMLS_CC);
					}
					break;
			}
		}
	} else {
		if (section_index==SECTION_IFD0 || section_index==SECTION_EXIF)
		switch(tag) {
			case TAG_COPYRIGHT:
				/* check for "<photographer> NUL <editor> NUL" */
				if (byte_count>1 && (length=php_strnlen(value_ptr, byte_count)) > 0) {
					if (length<byte_count-1) {
                                                /* When there are any characters after the first NUL */
                                                ImageInfo->CopyrightPhotographer  = estrdup(value_ptr);
                                                ImageInfo->CopyrightEditor        = estrndup(value_ptr+length+1, byte_count-length-1);
                                               spprintf(&ImageInfo->Copyright, 0, "%s, %s", ImageInfo->CopyrightPhotographer, ImageInfo->CopyrightEditor);
                                                /* format = TAG_FMT_UNDEFINED; this musn't be ASCII         */
                                                /* but we are not supposed to change this                   */
                                                /* keep in mind that image_info does not store editor value */
					} else {
						ImageInfo->Copyright = estrndup(value_ptr, byte_count);
					}
				}
				break;

			case TAG_USERCOMMENT:
				ImageInfo->UserCommentLength = exif_process_user_comment(ImageInfo, &(ImageInfo->UserComment), &(ImageInfo->UserCommentEncoding), value_ptr, byte_count TSRMLS_CC);
				break;

			case TAG_XP_TITLE:
			case TAG_XP_COMMENTS:
			case TAG_XP_AUTHOR:
			case TAG_XP_KEYWORDS:
			case TAG_XP_SUBJECT:
				tmp_xp = (xp_field_type*)safe_erealloc(ImageInfo->xp_fields.list, (ImageInfo->xp_fields.count+1), sizeof(xp_field_type), 0);
				ImageInfo->sections_found |= FOUND_WINXP;
				ImageInfo->xp_fields.list = tmp_xp;
				ImageInfo->xp_fields.count++;
				exif_process_unicode(ImageInfo, &(ImageInfo->xp_fields.list[ImageInfo->xp_fields.count-1]), tag, value_ptr, byte_count TSRMLS_CC);
				break;

			case TAG_FNUMBER:
				/* Simplest way of expressing aperture, so I trust it the most.
				   (overwrite previously computed value if there is one) */
				ImageInfo->ApertureFNumber = (float)exif_convert_any_format(value_ptr, format, ImageInfo->motorola_intel TSRMLS_CC);
				break;

			case TAG_APERTURE:
			case TAG_MAX_APERTURE:
				/* More relevant info always comes earlier, so only use this field if we don't
				   have appropriate aperture information yet. */
				if (ImageInfo->ApertureFNumber == 0) {
					ImageInfo->ApertureFNumber
						= (float)exp(exif_convert_any_format(value_ptr, format, ImageInfo->motorola_intel TSRMLS_CC)*log(2)*0.5);
				}
				break;

			case TAG_SHUTTERSPEED:
				/* More complicated way of expressing exposure time, so only use
				   this value if we don't already have it from somewhere else.
				   SHUTTERSPEED comes after EXPOSURE TIME
				  */
				if (ImageInfo->ExposureTime == 0) {
					ImageInfo->ExposureTime
						= (float)(1/exp(exif_convert_any_format(value_ptr, format, ImageInfo->motorola_intel TSRMLS_CC)*log(2)));
				}
				break;
			case TAG_EXPOSURETIME:
				ImageInfo->ExposureTime = -1;
				break;

			case TAG_COMP_IMAGE_WIDTH:
				ImageInfo->ExifImageWidth = exif_convert_any_to_int(value_ptr, format, ImageInfo->motorola_intel TSRMLS_CC);
				break;

			case TAG_FOCALPLANE_X_RES:
				ImageInfo->FocalplaneXRes = exif_convert_any_format(value_ptr, format, ImageInfo->motorola_intel TSRMLS_CC);
				break;

			case TAG_SUBJECT_DISTANCE:
				/* Inidcates the distacne the autofocus camera is focused to.
				   Tends to be less accurate as distance increases. */
				ImageInfo->Distance = (float)exif_convert_any_format(value_ptr, format, ImageInfo->motorola_intel TSRMLS_CC);
				break;

			case TAG_FOCALPLANE_RESOLUTION_UNIT:
				switch((int)exif_convert_any_format(value_ptr, format, ImageInfo->motorola_intel TSRMLS_CC)) {
					case 1: ImageInfo->FocalplaneUnits = 25.4; break; /* inch */
					case 2:
						/* According to the information I was using, 2 measn meters.
						   But looking at the Cannon powershot's files, inches is the only
						   sensible value. */
						ImageInfo->FocalplaneUnits = 25.4;
						break;

					case 3: ImageInfo->FocalplaneUnits = 10;   break;  /* centimeter */
					case 4: ImageInfo->FocalplaneUnits = 1;    break;  /* milimeter  */
					case 5: ImageInfo->FocalplaneUnits = .001; break;  /* micrometer */
				}
				break;

			case TAG_SUB_IFD:
				if (format==TAG_FMT_IFD) {
					/* If this is called we are either in a TIFFs thumbnail or a JPEG where we cannot handle it */
					/* TIFF thumbnail: our data structure cannot store a thumbnail of a thumbnail */
					/* JPEG do we have the data area and what to do with it */
					exif_error_docref(NULL EXIFERR_CC, ImageInfo, E_NOTICE, "Skip SUB IFD");
				}
				break;

			case TAG_MAKE:
				ImageInfo->make = estrndup(value_ptr, byte_count);
				break;
			case TAG_MODEL:
				ImageInfo->model = estrndup(value_ptr, byte_count);
				break;

			case TAG_MAKER_NOTE:
				exif_process_IFD_in_MAKERNOTE(ImageInfo, value_ptr, byte_count, offset_base, IFDlength, displacement TSRMLS_CC);
				break;

			case TAG_EXIF_IFD_POINTER:
			case TAG_GPS_IFD_POINTER:
			case TAG_INTEROP_IFD_POINTER:
				if (ReadNextIFD) {
					char *Subdir_start;
					int sub_section_index = 0;
					switch(tag) {
						case TAG_EXIF_IFD_POINTER:
#ifdef EXIF_DEBUG
							exif_error_docref(NULL EXIFERR_CC, ImageInfo, E_NOTICE, "Found EXIF");
#endif
							ImageInfo->sections_found |= FOUND_EXIF;
							sub_section_index = SECTION_EXIF;
							break;
						case TAG_GPS_IFD_POINTER:
#ifdef EXIF_DEBUG
							exif_error_docref(NULL EXIFERR_CC, ImageInfo, E_NOTICE, "Found GPS");
#endif
							ImageInfo->sections_found |= FOUND_GPS;
							sub_section_index = SECTION_GPS;
							break;
						case TAG_INTEROP_IFD_POINTER:
#ifdef EXIF_DEBUG
							exif_error_docref(NULL EXIFERR_CC, ImageInfo, E_NOTICE, "Found INTEROPERABILITY");
#endif
							ImageInfo->sections_found |= FOUND_INTEROP;
							sub_section_index = SECTION_INTEROP;
							break;
					}
					Subdir_start = offset_base + php_ifd_get32u(value_ptr, ImageInfo->motorola_intel);
					if (Subdir_start < offset_base || Subdir_start > offset_base+IFDlength) {
						exif_error_docref("exif_read_data#error_ifd" EXIFERR_CC, ImageInfo, E_WARNING, "Illegal IFD Pointer");
						return FALSE;
					}
					if (!exif_process_IFD_in_JPEG(ImageInfo, Subdir_start, offset_base, IFDlength, displacement, sub_section_index TSRMLS_CC)) {
						return FALSE;
					}
#ifdef EXIF_DEBUG
					exif_error_docref(NULL EXIFERR_CC, ImageInfo, E_NOTICE, "Subsection %s done", exif_get_sectionname(sub_section_index));
#endif
				}
		}
	}
	exif_iif_add_tag(ImageInfo, section_index, exif_get_tagname(tag, tagname, sizeof(tagname), tag_table TSRMLS_CC), tag, format, components, value_ptr TSRMLS_CC);
	EFREE_IF(outside);
	return TRUE;
}
/* }}} */

/* {{{ exif_process_IFD_in_JPEG
 * Process one of the nested IFDs directories. */
static int exif_process_IFD_in_JPEG(image_info_type *ImageInfo, char *dir_start, char *offset_base, size_t IFDlength, size_t displacement, int section_index TSRMLS_DC)
{
	int de;
	int NumDirEntries;
	int NextDirOffset;

#ifdef EXIF_DEBUG
	exif_error_docref(NULL EXIFERR_CC, ImageInfo, E_NOTICE, "Process %s (x%04X(=%d))", exif_get_sectionname(section_index), IFDlength, IFDlength);
#endif
 
        ImageInfo->sections_found |= FOUND_IFD0;
 
       if ((dir_start + 2) >= (offset_base+IFDlength)) {
               exif_error_docref("exif_read_data#error_ifd" EXIFERR_CC, ImageInfo, E_WARNING, "Illegal IFD size");
               return FALSE;
       }

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
       if ((dir_start+2+12*de + 4) >= (offset_base+IFDlength)) {
               exif_error_docref("exif_read_data#error_ifd" EXIFERR_CC, ImageInfo, E_WARNING, "Illegal IFD size");
               return FALSE;
       }
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
/* }}} */

/* {{{ exif_process_TIFF_in_JPEG
   Process a TIFF header in a JPEG file
*/
static void exif_process_TIFF_in_JPEG(image_info_type *ImageInfo, char *CharBuf, size_t length, size_t displacement TSRMLS_DC)
{
	unsigned exif_value_2a, offset_of_ifd;

	/* set the thumbnail stuff to nothing so we can test to see if they get set up */
	if (memcmp(CharBuf, "II", 2) == 0) {
		ImageInfo->motorola_intel = 0;
	} else if (memcmp(CharBuf, "MM", 2) == 0) {
		ImageInfo->motorola_intel = 1;
	} else {
		exif_error_docref(NULL EXIFERR_CC, ImageInfo, E_WARNING, "Invalid TIFF alignment marker");
		return;
	}

	/* Check the next two values for correctness. */
	exif_value_2a = php_ifd_get16u(CharBuf+2, ImageInfo->motorola_intel);
	offset_of_ifd = php_ifd_get32u(CharBuf+4, ImageInfo->motorola_intel);
	if ( exif_value_2a != 0x2a || offset_of_ifd < 0x08) {
		exif_error_docref(NULL EXIFERR_CC, ImageInfo, E_WARNING, "Invalid TIFF start (1)");
		return;
	}
        }
 
        /* Check the next two values for correctness. */
       if (length < 8) {
               exif_error_docref(NULL EXIFERR_CC, ImageInfo, E_WARNING, "Invalid TIFF start (1)");
               return;
       }
        exif_value_2a = php_ifd_get16u(CharBuf+2, ImageInfo->motorola_intel);
        offset_of_ifd = php_ifd_get32u(CharBuf+4, ImageInfo->motorola_intel);
       if (exif_value_2a != 0x2a || offset_of_ifd < 0x08) {
                exif_error_docref(NULL EXIFERR_CC, ImageInfo, E_WARNING, "Invalid TIFF start (1)");
                return;
        }
	/* Compute the CCD width, in milimeters. */
	if (ImageInfo->FocalplaneXRes != 0) {
		ImageInfo->CCDWidth = (float)(ImageInfo->ExifImageWidth * ImageInfo->FocalplaneUnits / ImageInfo->FocalplaneXRes);
	}
}
/* }}} */
