XcursorCommentCreate (XcursorUInt comment_type, int length)
{
    XcursorComment  *comment;

    if (length > XCURSOR_COMMENT_MAX_LEN)
	return NULL;
 {
     XcursorComment  *comment;
 
    if (length < 0 || length > XCURSOR_COMMENT_MAX_LEN)
 	return NULL;
 
     comment = malloc (sizeof (XcursorComment) + length + 1);
    comment->comment[0] = '\0';
    return comment;
}

void
XcursorCommentDestroy (XcursorComment *comment)
{
    free (comment);
}

XcursorComments *
XcursorCommentsCreate (int size)
{
    XcursorComments *comments;

    comments = malloc (sizeof (XcursorComments) +
		       size * sizeof (XcursorComment *));
    if (!comments)
	return NULL;
    comments->ncomment = 0;
    comments->comments = (XcursorComment **) (comments + 1);
    return comments;
}

void
XcursorCommentsDestroy (XcursorComments *comments)
{
    int	n;

    if (!comments)
        return;

    for (n = 0; n < comments->ncomment; n++)
	XcursorCommentDestroy (comments->comments[n]);
    free (comments);
}

static XcursorBool
_XcursorReadUInt (XcursorFile *file, XcursorUInt *u)
{
    unsigned char   bytes[4];

    if (!file || !u)
        return XcursorFalse;

    if ((*file->read) (file, bytes, 4) != 4)
	return XcursorFalse;
    *u = ((bytes[0] << 0) |
	  (bytes[1] << 8) |
	  (bytes[2] << 16) |
	  (bytes[3] << 24));
    return XcursorTrue;
}

static XcursorBool
_XcursorReadBytes (XcursorFile *file, char *bytes, int length)
{
    if (!file || !bytes || (*file->read) (file, (unsigned char *) bytes, length) != length)
	return XcursorFalse;
    return XcursorTrue;
}

static XcursorBool
_XcursorWriteUInt (XcursorFile *file, XcursorUInt u)
{
    unsigned char   bytes[4];

    if (!file)
        return XcursorFalse;

    bytes[0] = u;
    bytes[1] = u >>  8;
    bytes[2] = u >> 16;
    bytes[3] = u >> 24;
    if ((*file->write) (file, bytes, 4) != 4)
	return XcursorFalse;
    return XcursorTrue;
}

static XcursorBool
_XcursorWriteBytes (XcursorFile *file, char *bytes, int length)
{
    if (!file || !bytes || (*file->write) (file, (unsigned char *) bytes, length) != length)
	return XcursorFalse;
    return XcursorTrue;
}

static void
_XcursorFileHeaderDestroy (XcursorFileHeader *fileHeader)
{
    free (fileHeader);
}

static XcursorFileHeader *
_XcursorFileHeaderCreate (XcursorUInt ntoc)
{
    XcursorFileHeader	*fileHeader;

    if (ntoc > 0x10000)
	return NULL;
    fileHeader = malloc (sizeof (XcursorFileHeader) +
			 ntoc * sizeof (XcursorFileToc));
    if (!fileHeader)
	return NULL;
    fileHeader->magic = XCURSOR_MAGIC;
    fileHeader->header = XCURSOR_FILE_HEADER_LEN;
    fileHeader->version = XCURSOR_FILE_VERSION;
    fileHeader->ntoc = ntoc;
    fileHeader->tocs = (XcursorFileToc *) (fileHeader + 1);
    return fileHeader;
}

static XcursorFileHeader *
_XcursorReadFileHeader (XcursorFile *file)
{
    XcursorFileHeader	head, *fileHeader;
    XcursorUInt		skip;
    int			n;

    if (!file)
        return NULL;

    if (!_XcursorReadUInt (file, &head.magic))
	return NULL;
    if (head.magic != XCURSOR_MAGIC)
	return NULL;
    if (!_XcursorReadUInt (file, &head.header))
	return NULL;
    if (!_XcursorReadUInt (file, &head.version))
	return NULL;
    if (!_XcursorReadUInt (file, &head.ntoc))
	return NULL;
    skip = head.header - XCURSOR_FILE_HEADER_LEN;
    if (skip)
	if ((*file->seek) (file, skip, SEEK_CUR) == EOF)
	    return NULL;
    fileHeader = _XcursorFileHeaderCreate (head.ntoc);
    if (!fileHeader)
	return NULL;
    fileHeader->magic = head.magic;
    fileHeader->header = head.header;
    fileHeader->version = head.version;
    fileHeader->ntoc = head.ntoc;
    for (n = 0; n < fileHeader->ntoc; n++)
    {
	if (!_XcursorReadUInt (file, &fileHeader->tocs[n].type))
	    break;
	if (!_XcursorReadUInt (file, &fileHeader->tocs[n].subtype))
	    break;
	if (!_XcursorReadUInt (file, &fileHeader->tocs[n].position))
	    break;
    }
    if (n != fileHeader->ntoc)
    {
	_XcursorFileHeaderDestroy (fileHeader);
	return NULL;
    }
    return fileHeader;
}

static XcursorUInt
_XcursorFileHeaderLength (XcursorFileHeader *fileHeader)
{
    return (XCURSOR_FILE_HEADER_LEN +
	    fileHeader->ntoc * XCURSOR_FILE_TOC_LEN);
}

static XcursorBool
_XcursorWriteFileHeader (XcursorFile *file, XcursorFileHeader *fileHeader)
{
    int	toc;

    if (!file || !fileHeader)
        return XcursorFalse;

    if (!_XcursorWriteUInt (file, fileHeader->magic))
	return XcursorFalse;
    if (!_XcursorWriteUInt (file, fileHeader->header))
	return XcursorFalse;
    if (!_XcursorWriteUInt (file, fileHeader->version))
	return XcursorFalse;
    if (!_XcursorWriteUInt (file, fileHeader->ntoc))
	return XcursorFalse;
    for (toc = 0; toc < fileHeader->ntoc; toc++)
    {
	if (!_XcursorWriteUInt (file, fileHeader->tocs[toc].type))
	    return XcursorFalse;
	if (!_XcursorWriteUInt (file, fileHeader->tocs[toc].subtype))
	    return XcursorFalse;
	if (!_XcursorWriteUInt (file, fileHeader->tocs[toc].position))
	    return XcursorFalse;
    }
    return XcursorTrue;
}

static XcursorBool
_XcursorSeekToToc (XcursorFile		*file,
		   XcursorFileHeader	*fileHeader,
		   int			toc)
{
    if (!file || !fileHeader || \
        (*file->seek) (file, fileHeader->tocs[toc].position, SEEK_SET) == EOF)
	return XcursorFalse;
    return XcursorTrue;
}

static XcursorBool
_XcursorFileReadChunkHeader (XcursorFile	*file,
			     XcursorFileHeader	*fileHeader,
			     int		toc,
			     XcursorChunkHeader	*chunkHeader)
{
    if (!file || !fileHeader || !chunkHeader)
        return XcursorFalse;
    if (!_XcursorSeekToToc (file, fileHeader, toc))
	return XcursorFalse;
    if (!_XcursorReadUInt (file, &chunkHeader->header))
	return XcursorFalse;
    if (!_XcursorReadUInt (file, &chunkHeader->type))
	return XcursorFalse;
    if (!_XcursorReadUInt (file, &chunkHeader->subtype))
	return XcursorFalse;
    if (!_XcursorReadUInt (file, &chunkHeader->version))
	return XcursorFalse;
    /* sanity check */
    if (chunkHeader->type != fileHeader->tocs[toc].type ||
	chunkHeader->subtype != fileHeader->tocs[toc].subtype)
	return XcursorFalse;
    return XcursorTrue;
}

static XcursorBool
_XcursorFileWriteChunkHeader (XcursorFile	    *file,
			      XcursorFileHeader	    *fileHeader,
			      int		    toc,
			      XcursorChunkHeader    *chunkHeader)
{
    if (!file || !fileHeader || !chunkHeader)
        return XcursorFalse;
    if (!_XcursorSeekToToc (file, fileHeader, toc))
	return XcursorFalse;
    if (!_XcursorWriteUInt (file, chunkHeader->header))
	return XcursorFalse;
    if (!_XcursorWriteUInt (file, chunkHeader->type))
	return XcursorFalse;
    if (!_XcursorWriteUInt (file, chunkHeader->subtype))
	return XcursorFalse;
    if (!_XcursorWriteUInt (file, chunkHeader->version))
	return XcursorFalse;
    return XcursorTrue;
}

#define dist(a,b)   ((a) > (b) ? (a) - (b) : (b) - (a))

static XcursorDim
_XcursorFindBestSize (XcursorFileHeader *fileHeader,
		      XcursorDim	size,
		      int		*nsizesp)
{
    int		n;
    int		nsizes = 0;
    XcursorDim	bestSize = 0;
    XcursorDim	thisSize;

    if (!fileHeader || !nsizesp)
        return 0;

    for (n = 0; n < fileHeader->ntoc; n++)
    {
	if (fileHeader->tocs[n].type != XCURSOR_IMAGE_TYPE)
	    continue;
	thisSize = fileHeader->tocs[n].subtype;
	if (!bestSize || dist (thisSize, size) < dist (bestSize, size))
	{
	    bestSize = thisSize;
	    nsizes = 1;
	}
	else if (thisSize == bestSize)
	    nsizes++;
    }
    *nsizesp = nsizes;
    return bestSize;
}

static int
_XcursorFindImageToc (XcursorFileHeader	*fileHeader,
		      XcursorDim	size,
		      int		count)
{
    int			toc;
    XcursorDim		thisSize;

    if (!fileHeader)
        return 0;

    for (toc = 0; toc < fileHeader->ntoc; toc++)
    {
	if (fileHeader->tocs[toc].type != XCURSOR_IMAGE_TYPE)
	    continue;
	thisSize = fileHeader->tocs[toc].subtype;
	if (thisSize != size)
	    continue;
	if (!count)
	    break;
	count--;
    }
    if (toc == fileHeader->ntoc)
	return -1;
    return toc;
}

static XcursorImage *
_XcursorReadImage (XcursorFile		*file,
		   XcursorFileHeader	*fileHeader,
		   int			toc)
{
    XcursorChunkHeader	chunkHeader;
    XcursorImage	head;
    XcursorImage	*image;
    int			n;
    XcursorPixel	*p;

    if (!file || !fileHeader)
        return NULL;

    if (!_XcursorFileReadChunkHeader (file, fileHeader, toc, &chunkHeader))
	return NULL;
    if (!_XcursorReadUInt (file, &head.width))
	return NULL;
    if (!_XcursorReadUInt (file, &head.height))
	return NULL;
    if (!_XcursorReadUInt (file, &head.xhot))
	return NULL;
    if (!_XcursorReadUInt (file, &head.yhot))
	return NULL;
    if (!_XcursorReadUInt (file, &head.delay))
	return NULL;
    /* sanity check data */
    if (head.width >= 0x10000 || head.height > 0x10000)
	return NULL;
     if (!_XcursorReadUInt (file, &head.delay))
 	return NULL;
     /* sanity check data */
    if (head.width > XCURSOR_IMAGE_MAX_SIZE  ||
	head.height > XCURSOR_IMAGE_MAX_SIZE)
 	return NULL;
     if (head.width == 0 || head.height == 0)
 	return NULL;
	image->version = chunkHeader.version;
    image->size = chunkHeader.subtype;
 
     /* Create the image and initialize it */
     image = XcursorImageCreate (head.width, head.height);
    if (image == NULL)
	return NULL;
     if (chunkHeader.version < image->version)
 	image->version = chunkHeader.version;
     image->size = chunkHeader.subtype;
	{
	    XcursorImageDestroy (image);
	    return NULL;
	}
	p++;
    }
    return image;
}
