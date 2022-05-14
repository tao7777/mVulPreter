static MagickBooleanType EncodeImage(const ImageInfo *image_info,Image *image,
  const size_t data_size)
{
#define MaxCode(number_bits)  ((one << (number_bits))-1)
#define MaxHashTable  5003
#define MaxGIFBits  12UL
#define MaxGIFTable  (1UL << MaxGIFBits)
#define GIFOutputCode(code) \
{ \
  /*  \
    Emit a code. \
  */ \
  if (bits > 0) \
    datum|=(size_t) (code) << bits; \
  else \
    datum=(size_t) (code); \
  bits+=number_bits; \
  while (bits >= 8) \
  { \
    /*  \
      Add a character to current packet. \
    */ \
    packet[length++]=(unsigned char) (datum & 0xff); \
    if (length >= 254) \
      { \
        (void) WriteBlobByte(image,(unsigned char) length); \
        (void) WriteBlob(image,length,packet); \
        length=0; \
      } \
    datum>>=8; \
    bits-=8; \
  } \
  if (free_code > max_code)  \
    { \
      number_bits++; \
      if (number_bits == MaxGIFBits) \
        max_code=MaxGIFTable; \
      else \
        max_code=MaxCode(number_bits); \
    } \
}

  IndexPacket
    index;

  short
    *hash_code,
    *hash_prefix,
    waiting_code;

  size_t
    bits,
    clear_code,
    datum,
    end_of_information_code,
    free_code,
    length,
    max_code,
    next_pixel,
    number_bits,
    one,
    pass;

  ssize_t
    displacement,
    offset,
    k,
    y;

  unsigned char
    *packet,
    *hash_suffix;

  /*
    Allocate encoder tables.
  */
  assert(image != (Image *) NULL);
  one=1;
  packet=(unsigned char *) AcquireQuantumMemory(256,sizeof(*packet));
  hash_code=(short *) AcquireQuantumMemory(MaxHashTable,sizeof(*hash_code));
  hash_prefix=(short *) AcquireQuantumMemory(MaxHashTable,sizeof(*hash_prefix));
  hash_suffix=(unsigned char *) AcquireQuantumMemory(MaxHashTable,
    sizeof(*hash_suffix));
  if ((packet == (unsigned char *) NULL) || (hash_code == (short *) NULL) ||
      (hash_prefix == (short *) NULL) ||
      (hash_suffix == (unsigned char *) NULL))
    {
      if (packet != (unsigned char *) NULL)
        packet=(unsigned char *) RelinquishMagickMemory(packet);
      if (hash_code != (short *) NULL)
        hash_code=(short *) RelinquishMagickMemory(hash_code);
      if (hash_prefix != (short *) NULL)
        hash_prefix=(short *) RelinquishMagickMemory(hash_prefix);
      if (hash_suffix != (unsigned char *) NULL)
        hash_suffix=(unsigned char *) RelinquishMagickMemory(hash_suffix);
      return(MagickFalse);
    }
  /*
    Initialize GIF encoder.
  */
  (void) memset(packet,0,256*sizeof(*packet));
  (void) memset(hash_code,0,MaxHashTable*sizeof(*hash_code));
  (void) memset(hash_prefix,0,MaxHashTable*sizeof(*hash_prefix));
  (void) memset(hash_suffix,0,MaxHashTable*sizeof(*hash_suffix));
  number_bits=data_size;
  max_code=MaxCode(number_bits);
  clear_code=((short) one << (data_size-1));
  end_of_information_code=clear_code+1;
  free_code=clear_code+2;
  length=0;
  datum=0;
  bits=0;
  GIFOutputCode(clear_code);
  /*
    Encode pixels.
  */
  offset=0;
  pass=0;
  waiting_code=0;
  for (y=0; y < (ssize_t) image->rows; y++)
  {
    register const IndexPacket
      *magick_restrict indexes;

    register const PixelPacket
      *magick_restrict p;

    register ssize_t
      x;

    p=GetVirtualPixels(image,0,offset,image->columns,1,&image->exception);
    if (p == (const PixelPacket *) NULL)
      break;
    indexes=GetVirtualIndexQueue(image);
    if (y == 0)
      {
        waiting_code=(short) (*indexes);
        p++;
      }
    for (x=(ssize_t) (y == 0 ? 1 : 0); x < (ssize_t) image->columns; x++)
    {
       /*
         Probe hash table.
       */
      next_pixel=MagickFalse;
      displacement=1;
       index=(IndexPacket) ((size_t) GetPixelIndex(indexes+x) & 0xff);
       p++;
       k=(ssize_t) (((size_t) index << (MaxGIFBits-8))+waiting_code);
       if (k >= MaxHashTable)
         k-=MaxHashTable;
      if (k < 0)
        continue;
       if (hash_code[k] > 0)
         {
           if ((hash_prefix[k] == waiting_code) &&
              (hash_suffix[k] == (unsigned char) index))
            {
              waiting_code=hash_code[k];
              continue;
            }
          if (k != 0)
            displacement=MaxHashTable-k;
          for ( ; ; )
          {
            k-=displacement;
            if (k < 0)
              k+=MaxHashTable;
            if (hash_code[k] == 0)
              break;
            if ((hash_prefix[k] == waiting_code) &&
                (hash_suffix[k] == (unsigned char) index))
              {
                waiting_code=hash_code[k];
                next_pixel=MagickTrue;
                break;
              }
          }
          if (next_pixel != MagickFalse)
            continue;
        }
      GIFOutputCode(waiting_code);
      if (free_code < MaxGIFTable)
        {
          hash_code[k]=(short) free_code++;
          hash_prefix[k]=waiting_code;
          hash_suffix[k]=(unsigned char) index;
        }
      else
        {
          /*
            Fill the hash table with empty entries.
          */
          for (k=0; k < MaxHashTable; k++)
            hash_code[k]=0;
          /*
            Reset compressor and issue a clear code.
          */
          free_code=clear_code+2;
          GIFOutputCode(clear_code);
          number_bits=data_size;
          max_code=MaxCode(number_bits);
        }
      waiting_code=(short) index;
    }
    if (image_info->interlace == NoInterlace)
      offset++;
    else
      switch (pass)
      {
        case 0:
        default:
        {
          offset+=8;
          if (offset >= (ssize_t) image->rows)
            {
              pass++;
              offset=4;
            }
          break;
        }
        case 1:
        {
          offset+=8;
          if (offset >= (ssize_t) image->rows)
            {
              pass++;
              offset=2;
            }
          break;
        }
        case 2:
        {
          offset+=4;
          if (offset >= (ssize_t) image->rows)
            {
              pass++;
              offset=1;
            }
          break;
        }
        case 3:
        {
          offset+=2;
          break;
        }
      }
  }
  /*
    Flush out the buffered code.
  */
  GIFOutputCode(waiting_code);
  GIFOutputCode(end_of_information_code);
  if (bits > 0)
    {
      /*
        Add a character to current packet.
      */
      packet[length++]=(unsigned char) (datum & 0xff);
      if (length >= 254)
        {
          (void) WriteBlobByte(image,(unsigned char) length);
          (void) WriteBlob(image,length,packet);
          length=0;
        }
    }
  /*
    Flush accumulated data.
  */
  if (length > 0)
    {
      (void) WriteBlobByte(image,(unsigned char) length);
      (void) WriteBlob(image,length,packet);
    }
  /*
    Free encoder memory.
  */
  hash_suffix=(unsigned char *) RelinquishMagickMemory(hash_suffix);
  hash_prefix=(short *) RelinquishMagickMemory(hash_prefix);
  hash_code=(short *) RelinquishMagickMemory(hash_code);
  packet=(unsigned char *) RelinquishMagickMemory(packet);
  return(MagickTrue);
}
