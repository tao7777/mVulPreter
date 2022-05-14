static MagickBooleanType WritePDBImage(const ImageInfo *image_info,Image *image)
{
  const char
    *comment;

  int
    bits;

  MagickBooleanType
    status;

  PDBImage
    pdb_image;

  PDBInfo
    pdb_info;

  QuantumInfo
    *quantum_info;

  register const PixelPacket
    *p;

  register ssize_t
    x;

  register unsigned char
    *q;

  size_t
    bits_per_pixel,
    literal,
    packets,
    packet_size,
    repeat;

  ssize_t
    y;

  unsigned char
    *buffer,
    *runlength,
    *scanline;

  /*
    Open output image file.
  */
  assert(image_info != (const ImageInfo *) NULL);
  assert(image_info->signature == MagickSignature);
  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);
  if (image->debug != MagickFalse)
    (void) LogMagickEvent(TraceEvent,GetMagickModule(),"%s",image->filename);
  status=OpenBlob(image_info,image,WriteBinaryBlobMode,&image->exception);
  if (status == MagickFalse)
    return(status);
  (void) TransformImageColorspace(image,sRGBColorspace);
  if ((image -> colors <= 2 ) ||
      (GetImageType(image,&image->exception ) == BilevelType)) {
    bits_per_pixel=1;
  } else if (image->colors <= 4) {
    bits_per_pixel=2;
  } else if (image->colors <= 8) {
    bits_per_pixel=3;
  } else {
    bits_per_pixel=4;
  }
  (void) ResetMagickMemory(&pdb_info,0,sizeof(pdb_info));
  (void) CopyMagickString(pdb_info.name,image_info->filename,
    sizeof(pdb_info.name));
  pdb_info.attributes=0;
  pdb_info.version=0;
  pdb_info.create_time=time(NULL);
  pdb_info.modify_time=pdb_info.create_time;
  pdb_info.archive_time=0;
  pdb_info.modify_number=0;
  pdb_info.application_info=0;
  pdb_info.sort_info=0;
  (void) CopyMagickMemory(pdb_info.type,"vIMG",4);
  (void) CopyMagickMemory(pdb_info.id,"View",4);
  pdb_info.seed=0;
  pdb_info.next_record=0;
  comment=GetImageProperty(image,"comment");
  pdb_info.number_records=(comment == (const char *) NULL ? 1 : 2);
  (void) WriteBlob(image,sizeof(pdb_info.name),(unsigned char *) pdb_info.name);
  (void) WriteBlobMSBShort(image,(unsigned short) pdb_info.attributes);
  (void) WriteBlobMSBShort(image,(unsigned short) pdb_info.version);
  (void) WriteBlobMSBLong(image,(unsigned int) pdb_info.create_time);
  (void) WriteBlobMSBLong(image,(unsigned int) pdb_info.modify_time);
  (void) WriteBlobMSBLong(image,(unsigned int) pdb_info.archive_time);
  (void) WriteBlobMSBLong(image,(unsigned int) pdb_info.modify_number);
  (void) WriteBlobMSBLong(image,(unsigned int) pdb_info.application_info);
  (void) WriteBlobMSBLong(image,(unsigned int) pdb_info.sort_info);
  (void) WriteBlob(image,4,(unsigned char *) pdb_info.type);
  (void) WriteBlob(image,4,(unsigned char *) pdb_info.id);
  (void) WriteBlobMSBLong(image,(unsigned int) pdb_info.seed);
  (void) WriteBlobMSBLong(image,(unsigned int) pdb_info.next_record);
  (void) WriteBlobMSBShort(image,(unsigned short) pdb_info.number_records);
  (void) CopyMagickString(pdb_image.name,pdb_info.name,sizeof(pdb_image.name));
  pdb_image.version=1;  /* RLE Compressed */
  switch (bits_per_pixel)
  {
    case 1: pdb_image.type=(unsigned char) 0xff; break;  /* monochrome */
    case 2: pdb_image.type=(unsigned char) 0x00; break;  /* 2 bit gray */
    default: pdb_image.type=(unsigned char) 0x02;  /* 4 bit gray */
  }
  pdb_image.reserved_1=0;
  pdb_image.note=0;
  pdb_image.x_last=0;
  pdb_image.y_last=0;
  pdb_image.reserved_2=0;
  pdb_image.x_anchor=(unsigned short) 0xffff;
  pdb_image.y_anchor=(unsigned short) 0xffff;
  pdb_image.width=(short) image->columns;
  if (image->columns % 16)
    pdb_image.width=(short) (16*(image->columns/16+1));
  pdb_image.height=(short) image->rows;
  packets=((bits_per_pixel*image->columns+7)/8);
  runlength=(unsigned char *) AcquireQuantumMemory(9UL*packets,
    image->rows*sizeof(*runlength));
  if (runlength == (unsigned char *) NULL)
    ThrowWriterException(ResourceLimitError,"MemoryAllocationFailed");
   buffer=(unsigned char *) AcquireQuantumMemory(512,sizeof(*buffer));
   if (buffer == (unsigned char *) NULL)
     ThrowWriterException(ResourceLimitError,"MemoryAllocationFailed");
  packet_size=(size_t) (image->depth > 8 ? 2 : 1);
   scanline=(unsigned char *) AcquireQuantumMemory(image->columns,packet_size*
     sizeof(*scanline));
   if (scanline == (unsigned char *) NULL)
    ThrowWriterException(ResourceLimitError,"MemoryAllocationFailed");
  if (IssRGBCompatibleColorspace(image->colorspace) == MagickFalse)
    (void) TransformImageColorspace(image,sRGBColorspace);
  /*
    Convert to GRAY raster scanline.
  */
   quantum_info=AcquireQuantumInfo(image_info,image);
   if (quantum_info == (QuantumInfo *) NULL)
     ThrowWriterException(ResourceLimitError,"MemoryAllocationFailed");
  status=SetQuantumDepth(image,quantum_info,image->depth > 8 ? 16 : 8);
   bits=8/(int) bits_per_pixel-1;  /* start at most significant bits */
   literal=0;
   repeat=0;
  q=runlength;
  buffer[0]=0x00;
  for (y=0; y < (ssize_t) image->rows; y++)
  {
    p=GetVirtualPixels(image,0,y,image->columns,1,&image->exception);
    if (p == (const PixelPacket *) NULL)
      break;
    (void) ExportQuantumPixels(image,(const CacheView *) NULL,quantum_info,
      GrayQuantum,scanline,&image->exception);
    for (x=0; x < (ssize_t) pdb_image.width; x++)
    {
      if (x < (ssize_t) image->columns)
        buffer[literal+repeat]|=(0xff-scanline[x*packet_size]) >>
          (8-bits_per_pixel) << bits*bits_per_pixel;
      bits--;
      if (bits < 0)
        {
          if (((literal+repeat) > 0) &&
              (buffer[literal+repeat] == buffer[literal+repeat-1]))
            {
              if (repeat == 0)
                {
                  literal--;
                  repeat++;
                }
              repeat++;
              if (0x7f < repeat)
                {
                  q=EncodeRLE(q,buffer,literal,repeat);
                  literal=0;
                  repeat=0;
                }
            }
          else
            {
              if (repeat >= 2)
                literal+=repeat;
              else
                {
                  q=EncodeRLE(q,buffer,literal,repeat);
                  buffer[0]=buffer[literal+repeat];
                  literal=0;
                }
              literal++;
              repeat=0;
              if (0x7f < literal)
                {
                  q=EncodeRLE(q,buffer,(literal < 0x80 ? literal : 0x80),0);
                  (void) CopyMagickMemory(buffer,buffer+literal+repeat,0x80);
                  literal-=0x80;
                }
            }
        bits=8/(int) bits_per_pixel-1;
        buffer[literal+repeat]=0x00;
      }
    }
    status=SetImageProgress(image,SaveImageTag,(MagickOffsetType) y,
      image->rows);
    if (status == MagickFalse)
      break;
  }
  q=EncodeRLE(q,buffer,literal,repeat);
  scanline=(unsigned char *) RelinquishMagickMemory(scanline);
  buffer=(unsigned char *) RelinquishMagickMemory(buffer);
  quantum_info=DestroyQuantumInfo(quantum_info);
  /*
    Write the Image record header.
  */
  (void) WriteBlobMSBLong(image,(unsigned int) (TellBlob(image)+8*
    pdb_info.number_records));
  (void) WriteBlobByte(image,0x40);
  (void) WriteBlobByte(image,0x6f);
  (void) WriteBlobByte(image,0x80);
  (void) WriteBlobByte(image,0);
  if (pdb_info.number_records > 1)
    {
      /*
        Write the comment record header.
      */
      (void) WriteBlobMSBLong(image,(unsigned int) (TellBlob(image)+8+58+q-
        runlength));
      (void) WriteBlobByte(image,0x40);
      (void) WriteBlobByte(image,0x6f);
      (void) WriteBlobByte(image,0x80);
      (void) WriteBlobByte(image,1);
    }
  /*
    Write the Image data.
  */
  (void) WriteBlob(image,sizeof(pdb_image.name),(unsigned char *)
    pdb_image.name);
  (void) WriteBlobByte(image,(unsigned char) pdb_image.version);
  (void) WriteBlobByte(image,pdb_image.type);
  (void) WriteBlobMSBLong(image,(unsigned int) pdb_image.reserved_1);
  (void) WriteBlobMSBLong(image,(unsigned int) pdb_image.note);
  (void) WriteBlobMSBShort(image,(unsigned short) pdb_image.x_last);
  (void) WriteBlobMSBShort(image,(unsigned short) pdb_image.y_last);
  (void) WriteBlobMSBLong(image,(unsigned int) pdb_image.reserved_2);
  (void) WriteBlobMSBShort(image,pdb_image.x_anchor);
  (void) WriteBlobMSBShort(image,pdb_image.y_anchor);
  (void) WriteBlobMSBShort(image,(unsigned short) pdb_image.width);
  (void) WriteBlobMSBShort(image,(unsigned short) pdb_image.height);
  (void) WriteBlob(image,(size_t) (q-runlength),runlength);
  runlength=(unsigned char *) RelinquishMagickMemory(runlength);
  if (pdb_info.number_records > 1)
    (void) WriteBlobString(image,comment);
  (void) CloseBlob(image);
  return(MagickTrue);
}
