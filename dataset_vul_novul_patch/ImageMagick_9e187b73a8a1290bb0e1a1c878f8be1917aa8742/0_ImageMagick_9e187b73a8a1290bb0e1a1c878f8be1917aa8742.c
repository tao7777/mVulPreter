static void WriteProfile(j_compress_ptr jpeg_info,Image *image)
{
  const char
    *name;

  const StringInfo
    *profile;

  MagickBooleanType
    iptc;

  register ssize_t
    i;

  size_t
    length,
    tag_length;

  StringInfo
    *custom_profile;

  /*
    Save image profile as a APP marker.
  */
  iptc=MagickFalse;
  custom_profile=AcquireStringInfo(65535L);
  ResetImageProfileIterator(image);
  for (name=GetNextImageProfile(image); name != (const char *) NULL; )
  {
    register unsigned char
      *p;

     profile=GetImageProfile(image,name);
     p=GetStringInfoDatum(custom_profile);
     if (LocaleCompare(name,"EXIF") == 0)
       {
        length=GetStringInfoLength(profile);
        if (length > 65533L)
          {
            (void) ThrowMagickException(&image->exception,GetMagickModule(),
              CoderWarning,"ExifProfileSizeExceedsLimit",image->filename);
            length=65533L;
          }
        jpeg_write_marker(jpeg_info,XML_MARKER,GetStringInfoDatum(profile),
           (unsigned int) length);
       }
     if (LocaleCompare(name,"ICC") == 0)
      {
        register unsigned char
          *p;

        tag_length=strlen(ICC_PROFILE);
        p=GetStringInfoDatum(custom_profile);
        (void) CopyMagickMemory(p,ICC_PROFILE,tag_length);
        p[tag_length]='\0';
        for (i=0; i < (ssize_t) GetStringInfoLength(profile); i+=65519L)
        {
          length=MagickMin(GetStringInfoLength(profile)-i,65519L);
          p[12]=(unsigned char) ((i/65519L)+1);
          p[13]=(unsigned char) (GetStringInfoLength(profile)/65519L+1);
          (void) CopyMagickMemory(p+tag_length+3,GetStringInfoDatum(profile)+i,
            length);
          jpeg_write_marker(jpeg_info,ICC_MARKER,GetStringInfoDatum(
            custom_profile),(unsigned int) (length+tag_length+3));
        }
      }
    if (((LocaleCompare(name,"IPTC") == 0) ||
        (LocaleCompare(name,"8BIM") == 0)) && (iptc == MagickFalse))
      {
        size_t
          roundup;

        iptc=MagickTrue;
        for (i=0; i < (ssize_t) GetStringInfoLength(profile); i+=65500L)
        {
          length=MagickMin(GetStringInfoLength(profile)-i,65500L);
          roundup=(size_t) (length & 0x01);
          if (LocaleNCompare((char *) GetStringInfoDatum(profile),"8BIM",4) == 0)
            {
              (void) memcpy(p,"Photoshop 3.0 ",14);
              tag_length=14;
            }
          else
            {
              (void) CopyMagickMemory(p,"Photoshop 3.0 8BIM\04\04\0\0\0\0",24);
              tag_length=26;
              p[24]=(unsigned char) (length >> 8);
              p[25]=(unsigned char) (length & 0xff);
            }
          p[13]=0x00;
          (void) memcpy(p+tag_length,GetStringInfoDatum(profile)+i,length);
          if (roundup != 0)
            p[length+tag_length]='\0';
          jpeg_write_marker(jpeg_info,IPTC_MARKER,GetStringInfoDatum(
            custom_profile),(unsigned int) (length+tag_length+roundup));
        }
      }
    if (LocaleCompare(name,"XMP") == 0)
      {
        StringInfo
          *xmp_profile;

        /*
          Add namespace to XMP profile.
        */
        xmp_profile=StringToStringInfo("http://ns.adobe.com/xap/1.0/ ");
        if (xmp_profile != (StringInfo *) NULL)
          {
            if (profile != (StringInfo *) NULL)
              ConcatenateStringInfo(xmp_profile,profile);
            GetStringInfoDatum(xmp_profile)[28]='\0';
            for (i=0; i < (ssize_t) GetStringInfoLength(xmp_profile); i+=65533L)
            {
              length=MagickMin(GetStringInfoLength(xmp_profile)-i,65533L);
              jpeg_write_marker(jpeg_info,XML_MARKER,
                GetStringInfoDatum(xmp_profile)+i,(unsigned int) length);
            }
            xmp_profile=DestroyStringInfo(xmp_profile);
          }
      }
    (void) LogMagickEvent(CoderEvent,GetMagickModule(),
      "%s profile: %.20g bytes",name,(double) GetStringInfoLength(profile));
    name=GetNextImageProfile(image);
  }
  custom_profile=DestroyStringInfo(custom_profile);
}
