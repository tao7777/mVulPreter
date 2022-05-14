static Image *ReadPDBImage(const ImageInfo *image_info,ExceptionInfo *exception)
{
  unsigned char
    attributes,
    tag[3];

  Image
    *image;

  IndexPacket
    index;

  MagickBooleanType
    status;

  PDBImage
    pdb_image;

  PDBInfo
    pdb_info;

  register IndexPacket
    *indexes;

  register ssize_t
    x;

  register PixelPacket
    *q;

  register unsigned char
    *p;

  size_t
    bits_per_pixel,
    num_pad_bytes,
    one,
    packets;

  ssize_t
    count,
    img_offset,
    comment_offset = 0,
    y;

  unsigned char
    *pixels;

  /*
    Open image file.
  */
  assert(image_info != (const ImageInfo *) NULL);
  assert(image_info->signature == MagickSignature);
  if (image_info->debug != MagickFalse)
    (void) LogMagickEvent(TraceEvent,GetMagickModule(),"%s",
      image_info->filename);
  assert(exception != (ExceptionInfo *) NULL);
  assert(exception->signature == MagickSignature);
  image=AcquireImage(image_info);
  status=OpenBlob(image_info,image,ReadBinaryBlobMode,exception);
  if (status == MagickFalse)
    {
      image=DestroyImageList(image);
      return((Image *) NULL);
    }
  /*
    Determine if this a PDB image file.
  */
  count=ReadBlob(image,32,(unsigned char *) pdb_info.name);
  pdb_info.attributes=(short) ReadBlobMSBShort(image);
  pdb_info.version=(short) ReadBlobMSBShort(image);
  pdb_info.create_time=ReadBlobMSBLong(image);
  pdb_info.modify_time=ReadBlobMSBLong(image);
  pdb_info.archive_time=ReadBlobMSBLong(image);
  pdb_info.modify_number=ReadBlobMSBLong(image);
  pdb_info.application_info=ReadBlobMSBLong(image);
  pdb_info.sort_info=ReadBlobMSBLong(image);
  count=ReadBlob(image,4,(unsigned char *) pdb_info.type);
  count=ReadBlob(image,4,(unsigned char *) pdb_info.id);
  if ((count == 0) || (memcmp(pdb_info.type,"vIMG",4) != 0) ||
      (memcmp(pdb_info.id,"View",4) != 0))
    ThrowReaderException(CorruptImageError,"ImproperImageHeader");
  pdb_info.seed=ReadBlobMSBLong(image);
  pdb_info.next_record=ReadBlobMSBLong(image);
  pdb_info.number_records=(short) ReadBlobMSBShort(image);
  if (pdb_info.next_record != 0)
    ThrowReaderException(CoderError,"MultipleRecordListNotSupported");
  /*
    Read record header.
  */
  img_offset=(ssize_t) ((int) ReadBlobMSBLong(image));
  attributes=(unsigned char) ((int) ReadBlobByte(image));
  (void) attributes;
  count=ReadBlob(image,3,(unsigned char *) tag);
  if (count != 3  ||  memcmp(tag,"\x6f\x80\x00",3) != 0)
    ThrowReaderException(CorruptImageError,"CorruptImage");
  if (pdb_info.number_records > 1)
    {
      comment_offset=(ssize_t) ((int) ReadBlobMSBLong(image));
      attributes=(unsigned char) ((int) ReadBlobByte(image));
      count=ReadBlob(image,3,(unsigned char *) tag);
      if (count != 3  ||  memcmp(tag,"\x6f\x80\x01",3) != 0)
        ThrowReaderException(CorruptImageError,"CorruptImage");
    }
  num_pad_bytes = (size_t) (img_offset - TellBlob( image ));
  while (num_pad_bytes--) ReadBlobByte( image );
  /*
    Read image header.
  */
  count=ReadBlob(image,32,(unsigned char *) pdb_image.name);
  pdb_image.version=ReadBlobByte(image);
  pdb_image.type=(unsigned char) ReadBlobByte(image);
  pdb_image.reserved_1=ReadBlobMSBLong(image);
  pdb_image.note=ReadBlobMSBLong(image);
  pdb_image.x_last=(short) ReadBlobMSBShort(image);
  pdb_image.y_last=(short) ReadBlobMSBShort(image);
  pdb_image.reserved_2=ReadBlobMSBLong(image);
  pdb_image.x_anchor=ReadBlobMSBShort(image);
  pdb_image.y_anchor=ReadBlobMSBShort(image);
  pdb_image.width=(short) ReadBlobMSBShort(image);
  pdb_image.height=(short) ReadBlobMSBShort(image);
  /*
    Initialize image structure.
  */
  image->columns=(size_t) pdb_image.width;
  image->rows=(size_t) pdb_image.height;
  image->depth=8;
  image->storage_class=PseudoClass;
  bits_per_pixel=pdb_image.type == 0 ? 2UL : pdb_image.type == 2 ? 4UL : 1UL;
  one=1;
  if (AcquireImageColormap(image,one << bits_per_pixel) == MagickFalse)
    ThrowReaderException(ResourceLimitError,"MemoryAllocationFailed");
  if (image_info->ping != MagickFalse)
    {
       (void) CloseBlob(image);
       return(GetFirstImageInList(image));
     }
  status=SetImageExtent(image,image->columns,image->rows);
  if (status == MagickFalse)
    {
      InheritException(exception,&image->exception);
      return(DestroyImageList(image));
    }
   packets=(bits_per_pixel*image->columns+7)/8;
   pixels=(unsigned char *) AcquireQuantumMemory(packets+256UL,image->rows*
     sizeof(*pixels));
  if (pixels == (unsigned char *) NULL)
    ThrowReaderException(ResourceLimitError,"MemoryAllocationFailed");
  switch (pdb_image.version & 0x07)
  {
    case 0:
    {
      image->compression=NoCompression;
      count=(ssize_t) ReadBlob(image, packets * image -> rows, pixels);
      break;
    }
    case 1:
    {
      image->compression=RLECompression;
      if (!DecodeImage(image, pixels, packets * image -> rows))
        ThrowReaderException( CorruptImageError, "RLEDecoderError" );
      break;
    }
    default:
      ThrowReaderException(CorruptImageError,
         "UnrecognizedImageCompressionType" );
  }
  p=pixels;
  switch (bits_per_pixel)
  {
    case 1:
    {
      int
        bit;

      /*
        Read 1-bit PDB image.
      */
      for (y=0; y < (ssize_t) image->rows; y++)
      {
        q=QueueAuthenticPixels(image,0,y,image->columns,1,exception);
        if (q == (PixelPacket *) NULL)
          break;
        indexes=GetAuthenticIndexQueue(image);
        for (x=0; x < ((ssize_t) image->columns-7); x+=8)
        {
          for (bit=0; bit < 8; bit++)
          {
            index=(IndexPacket) (*p & (0x80 >> bit) ? 0x00 : 0x01);
            SetPixelIndex(indexes+x+bit,index);
          }
          p++;
        }
        if (SyncAuthenticPixels(image,exception) == MagickFalse)
          break;
        status=SetImageProgress(image,LoadImageTag,(MagickOffsetType) y,
          image->rows);
        if (status == MagickFalse)
          break;
      }
      (void) SyncImage(image);
      break;
    }
    case 2:
    {
      /*
        Read 2-bit PDB image.
      */
      for (y=0; y < (ssize_t) image->rows; y++)
      {
        q=QueueAuthenticPixels(image,0,y,image->columns,1,exception);
        if (q == (PixelPacket *) NULL)
          break;
        indexes=GetAuthenticIndexQueue(image);
        for (x=0; x < (ssize_t) image->columns; x+=4)
        {
          index=ConstrainColormapIndex(image,3UL-((*p >> 6) & 0x03));
          SetPixelIndex(indexes+x,index);
          index=ConstrainColormapIndex(image,3UL-((*p >> 4) & 0x03));
          SetPixelIndex(indexes+x+1,index);
          index=ConstrainColormapIndex(image,3UL-((*p >> 2) & 0x03));
          SetPixelIndex(indexes+x+2,index);
          index=ConstrainColormapIndex(image,3UL-((*p) & 0x03));
          SetPixelIndex(indexes+x+3,index);
          p++;
        }
        if (SyncAuthenticPixels(image,exception) == MagickFalse)
          break;
        status=SetImageProgress(image,LoadImageTag,(MagickOffsetType) y,
          image->rows);
        if (status == MagickFalse)
          break;
      }
      (void) SyncImage(image);
      break;
    }
    case 4:
    {
      /*
        Read 4-bit PDB image.
      */
      for (y=0; y < (ssize_t) image->rows; y++)
      {
        q=QueueAuthenticPixels(image,0,y,image->columns,1,exception);
        if (q == (PixelPacket *) NULL)
          break;
        indexes=GetAuthenticIndexQueue(image);
        for (x=0; x < (ssize_t) image->columns; x+=2)
        {
          index=ConstrainColormapIndex(image,15UL-((*p >> 4) & 0x0f));
          SetPixelIndex(indexes+x,index);
          index=ConstrainColormapIndex(image,15UL-((*p) & 0x0f));
          SetPixelIndex(indexes+x+1,index);
          p++;
        }
        if (SyncAuthenticPixels(image,exception) == MagickFalse)
          break;
        status=SetImageProgress(image,LoadImageTag,(MagickOffsetType) y,
          image->rows);
        if (status == MagickFalse)
          break;
      }
      (void) SyncImage(image);
      break;
    }
    default:
      ThrowReaderException(CorruptImageError,"ImproperImageHeader");
  }
  pixels=(unsigned char *) RelinquishMagickMemory(pixels);
  if (EOFBlob(image) != MagickFalse)
    ThrowFileException(exception,CorruptImageError,"UnexpectedEndOfFile",
      image->filename);
  if (pdb_info.number_records > 1)
    {
      char
        *comment;

      int
        c;

      register char
        *p;

      size_t
        length;

      num_pad_bytes = (size_t) (comment_offset - TellBlob( image ));
      while (num_pad_bytes--) ReadBlobByte( image );

      /*
        Read comment.
      */
      c=ReadBlobByte(image);
      length=MaxTextExtent;
      comment=AcquireString((char *) NULL);
      for (p=comment; c != EOF; p++)
      {
        if ((size_t) (p-comment+MaxTextExtent) >= length)
          {
            *p='\0';
            length<<=1;
            length+=MaxTextExtent;
            comment=(char *) ResizeQuantumMemory(comment,length+MaxTextExtent,
              sizeof(*comment));
            if (comment == (char *) NULL)
              break;
            p=comment+strlen(comment);
          }
        *p=c;
        c=ReadBlobByte(image);
      }
      *p='\0';
      if (comment == (char *) NULL)
        ThrowReaderException(ResourceLimitError,"MemoryAllocationFailed");
      (void) SetImageProperty(image,"comment",comment);
      comment=DestroyString(comment);
    }
  (void) CloseBlob(image);
  return(GetFirstImageInList(image));
}
