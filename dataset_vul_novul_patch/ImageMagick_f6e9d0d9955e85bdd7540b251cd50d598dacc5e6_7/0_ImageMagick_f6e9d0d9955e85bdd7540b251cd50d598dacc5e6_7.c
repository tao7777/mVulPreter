static Image *ReadCUTImage(const ImageInfo *image_info,ExceptionInfo *exception)
{
  Image *image,*palette;
  ImageInfo *clone_info;
  MagickBooleanType status;

  MagickOffsetType
    offset;

  size_t EncodedByte;
  unsigned char RunCount,RunValue,RunCountMasked;
  CUTHeader  Header;
  CUTPalHeader PalHeader;
  ssize_t depth;
  ssize_t i,j;
  ssize_t ldblk;
  unsigned char *BImgBuff=NULL,*ptrB;
  PixelPacket *q;

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
    Read CUT image.
  */
  palette=NULL;
  clone_info=NULL;
  Header.Width=ReadBlobLSBShort(image);
  Header.Height=ReadBlobLSBShort(image);
  Header.Reserved=ReadBlobLSBShort(image);

  if (Header.Width==0 || Header.Height==0 || Header.Reserved!=0)
    CUT_KO:  ThrowReaderException(CorruptImageError,"ImproperImageHeader");

  /*---This code checks first line of image---*/
  EncodedByte=ReadBlobLSBShort(image);
  RunCount=(unsigned char) ReadBlobByte(image);
  RunCountMasked=RunCount & 0x7F;
  ldblk=0;
  while((int) RunCountMasked!=0)  /*end of line?*/
    {
      i=1;
      if((int) RunCount<0x80) i=(ssize_t) RunCountMasked;
      offset=SeekBlob(image,TellBlob(image)+i,SEEK_SET);
      if (offset < 0)
        ThrowReaderException(CorruptImageError,"ImproperImageHeader");
      if(EOFBlob(image) != MagickFalse) goto CUT_KO;  /*wrong data*/
      EncodedByte-=i+1;
      ldblk+=(ssize_t) RunCountMasked;

      RunCount=(unsigned char) ReadBlobByte(image);
      if(EOFBlob(image) != MagickFalse)  goto CUT_KO;  /*wrong data: unexpected eof in line*/
      RunCountMasked=RunCount & 0x7F;
    }
  if(EncodedByte!=1) goto CUT_KO;  /*wrong data: size incorrect*/
  i=0;        /*guess a number of bit planes*/
  if(ldblk==(int) Header.Width)   i=8;
  if(2*ldblk==(int) Header.Width) i=4;
  if(8*ldblk==(int) Header.Width) i=1;
  if(i==0) goto CUT_KO;    /*wrong data: incorrect bit planes*/
  depth=i;

  image->columns=Header.Width;
  image->rows=Header.Height;
   image->depth=8;
   image->colors=(size_t) (GetQuantumRange(1UL*i)+1);
 
  if (image_info->ping != MagickFalse) goto Finish;
  status=SetImageExtent(image,image->columns,image->rows);
  if (status == MagickFalse)
    {
      InheritException(exception,&image->exception);
      return(DestroyImageList(image));
    }
 
   /* ----- Do something with palette ----- */
   if ((clone_info=CloneImageInfo(image_info)) == NULL) goto NoPalette;


  i=(ssize_t) strlen(clone_info->filename);
  j=i;
  while(--i>0)
    {
      if(clone_info->filename[i]=='.')
        {
          break;
        }
      if(clone_info->filename[i]=='/' || clone_info->filename[i]=='\\' ||
         clone_info->filename[i]==':' )
        {
          i=j;
          break;
        }
    }

  (void) CopyMagickString(clone_info->filename+i,".PAL",(size_t)
    (MaxTextExtent-i));
  if((clone_info->file=fopen_utf8(clone_info->filename,"rb"))==NULL)
    {
      (void) CopyMagickString(clone_info->filename+i,".pal",(size_t)
        (MaxTextExtent-i));
      if((clone_info->file=fopen_utf8(clone_info->filename,"rb"))==NULL)
        {
          clone_info->filename[i]='\0';
          if((clone_info->file=fopen_utf8(clone_info->filename,"rb"))==NULL)
            {
              clone_info=DestroyImageInfo(clone_info);
              clone_info=NULL;
              goto NoPalette;
            }
        }
    }

  if( (palette=AcquireImage(clone_info))==NULL ) goto NoPalette;
  status=OpenBlob(clone_info,palette,ReadBinaryBlobMode,exception);
  if (status == MagickFalse)
    {
    ErasePalette:
      palette=DestroyImage(palette);
      palette=NULL;
      goto NoPalette;
    }


  if(palette!=NULL)
    {
      (void) ReadBlob(palette,2,(unsigned char *) PalHeader.FileId);
      if(strncmp(PalHeader.FileId,"AH",2) != 0) goto ErasePalette;
      PalHeader.Version=ReadBlobLSBShort(palette);
      PalHeader.Size=ReadBlobLSBShort(palette);
      PalHeader.FileType=(char) ReadBlobByte(palette);
      PalHeader.SubType=(char) ReadBlobByte(palette);
      PalHeader.BoardID=ReadBlobLSBShort(palette);
      PalHeader.GraphicsMode=ReadBlobLSBShort(palette);
      PalHeader.MaxIndex=ReadBlobLSBShort(palette);
      PalHeader.MaxRed=ReadBlobLSBShort(palette);
      PalHeader.MaxGreen=ReadBlobLSBShort(palette);
      PalHeader.MaxBlue=ReadBlobLSBShort(palette);
      (void) ReadBlob(palette,20,(unsigned char *) PalHeader.PaletteId);

      if(PalHeader.MaxIndex<1) goto ErasePalette;
      image->colors=PalHeader.MaxIndex+1;
      if (AcquireImageColormap(image,image->colors) == MagickFalse) goto NoMemory;

      if(PalHeader.MaxRed==0) PalHeader.MaxRed=(unsigned int) QuantumRange;  /*avoid division by 0*/
      if(PalHeader.MaxGreen==0) PalHeader.MaxGreen=(unsigned int) QuantumRange;
      if(PalHeader.MaxBlue==0) PalHeader.MaxBlue=(unsigned int) QuantumRange;

      for(i=0;i<=(int) PalHeader.MaxIndex;i++)
        {      /*this may be wrong- I don't know why is palette such strange*/
          j=(ssize_t) TellBlob(palette);
          if((j % 512)>512-6)
            {
              j=((j / 512)+1)*512;
              offset=SeekBlob(palette,j,SEEK_SET);
              if (offset < 0)
                ThrowReaderException(CorruptImageError,"ImproperImageHeader");
            }
          image->colormap[i].red=(Quantum) ReadBlobLSBShort(palette);
          if (QuantumRange != (Quantum) PalHeader.MaxRed)
            {
              image->colormap[i].red=ClampToQuantum(((double)
                image->colormap[i].red*QuantumRange+(PalHeader.MaxRed>>1))/
                PalHeader.MaxRed);
            }
          image->colormap[i].green=(Quantum) ReadBlobLSBShort(palette);
          if (QuantumRange != (Quantum) PalHeader.MaxGreen)
            {
              image->colormap[i].green=ClampToQuantum
                (((double) image->colormap[i].green*QuantumRange+(PalHeader.MaxGreen>>1))/PalHeader.MaxGreen);
            }
          image->colormap[i].blue=(Quantum) ReadBlobLSBShort(palette);
          if (QuantumRange != (Quantum) PalHeader.MaxBlue)
            {
              image->colormap[i].blue=ClampToQuantum
                (((double)image->colormap[i].blue*QuantumRange+(PalHeader.MaxBlue>>1))/PalHeader.MaxBlue);
            }

        }
    }



 NoPalette:
  if(palette==NULL)
    {

      image->colors=256;
      if (AcquireImageColormap(image,image->colors) == MagickFalse)
        {
        NoMemory:
          ThrowReaderException(ResourceLimitError,"MemoryAllocationFailed");
            }

      for (i=0; i < (ssize_t)image->colors; i++)
        {
          image->colormap[i].red=ScaleCharToQuantum((unsigned char) i);
          image->colormap[i].green=ScaleCharToQuantum((unsigned char) i);
          image->colormap[i].blue=ScaleCharToQuantum((unsigned char) i);
        }
    }


  /* ----- Load RLE compressed raster ----- */
  BImgBuff=(unsigned char *) AcquireQuantumMemory((size_t) ldblk,
    sizeof(*BImgBuff));  /*Ldblk was set in the check phase*/
  if(BImgBuff==NULL) goto NoMemory;

  offset=SeekBlob(image,6 /*sizeof(Header)*/,SEEK_SET);
  if (offset < 0)
    ThrowReaderException(CorruptImageError,"ImproperImageHeader");
  for (i=0; i < (int) Header.Height; i++)
  {
      EncodedByte=ReadBlobLSBShort(image);

      ptrB=BImgBuff;
      j=ldblk;

      RunCount=(unsigned char) ReadBlobByte(image);
      RunCountMasked=RunCount & 0x7F;

      while ((int) RunCountMasked != 0)
      {
          if((ssize_t) RunCountMasked>j)
            {    /*Wrong Data*/
              RunCountMasked=(unsigned char) j;
              if(j==0)
                {
                  break;
                }
            }

          if((int) RunCount>0x80)
            {
              RunValue=(unsigned char) ReadBlobByte(image);
              (void) ResetMagickMemory(ptrB,(int) RunValue,(size_t) RunCountMasked);
            }
          else {
            (void) ReadBlob(image,(size_t) RunCountMasked,ptrB);
          }

          ptrB+=(int) RunCountMasked;
          j-=(int) RunCountMasked;

          if (EOFBlob(image) != MagickFalse) goto Finish;  /* wrong data: unexpected eof in line */
          RunCount=(unsigned char) ReadBlobByte(image);
          RunCountMasked=RunCount & 0x7F;
        }

      InsertRow(depth,BImgBuff,i,image);
    }
  (void) SyncImage(image);


  /*detect monochrome image*/

  if(palette==NULL)
    {    /*attempt to detect binary (black&white) images*/
      if ((image->storage_class == PseudoClass) &&
          (IsGrayImage(image,&image->exception) != MagickFalse))
        {
          if(GetCutColors(image)==2)
            {
              for (i=0; i < (ssize_t)image->colors; i++)
                {
                  register Quantum
                    sample;
                  sample=ScaleCharToQuantum((unsigned char) i);
                  if(image->colormap[i].red!=sample) goto Finish;
                  if(image->colormap[i].green!=sample) goto Finish;
                  if(image->colormap[i].blue!=sample) goto Finish;
                }

              image->colormap[1].red=image->colormap[1].green=
                image->colormap[1].blue=QuantumRange;
              for (i=0; i < (ssize_t)image->rows; i++)
                {
                  q=QueueAuthenticPixels(image,0,i,image->columns,1,exception);
                  for (j=0; j < (ssize_t)image->columns; j++)
                    {
                      if (GetPixelRed(q) == ScaleCharToQuantum(1))
                        {
                          SetPixelRed(q,QuantumRange);
                          SetPixelGreen(q,QuantumRange);
                          SetPixelBlue(q,QuantumRange);
                        }
                      q++;
                    }
                  if (SyncAuthenticPixels(image,exception) == MagickFalse) goto Finish;
                }
            }
        }
    }

 Finish:
  if (BImgBuff != NULL)
    BImgBuff=(unsigned char *) RelinquishMagickMemory(BImgBuff);
  if (palette != NULL)
    palette=DestroyImage(palette);
  if (clone_info != NULL)
    clone_info=DestroyImageInfo(clone_info);
  if (EOFBlob(image) != MagickFalse)
    ThrowFileException(exception,CorruptImageError,"UnexpectedEndOfFile",
      image->filename);
  (void) CloseBlob(image);
  return(GetFirstImageInList(image));
}
