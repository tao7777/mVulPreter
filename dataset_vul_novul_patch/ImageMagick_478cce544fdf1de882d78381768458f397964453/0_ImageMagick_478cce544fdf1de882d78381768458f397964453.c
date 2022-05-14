static void WriteTo8BimProfile(Image *image,const char *name,
  const StringInfo *profile)
{

  const unsigned char
    *datum,
    *q;

  register const unsigned char
    *p;

  size_t
    length;

  StringInfo
    *profile_8bim;

  ssize_t
    count;

  unsigned char
    length_byte;

  unsigned int
    value;

  unsigned short
    id,
    profile_id;

  if (LocaleCompare(name,"icc") == 0)
    profile_id=0x040f;
  else
    if (LocaleCompare(name,"iptc") == 0)
      profile_id=0x0404;
    else
      if (LocaleCompare(name,"xmp") == 0)
        profile_id=0x0424;
      else
        return;
  profile_8bim=(StringInfo *) GetValueFromSplayTree((SplayTreeInfo *)
    image->profiles,"8bim");
  if (profile_8bim == (StringInfo *) NULL)
    return;
  datum=GetStringInfoDatum(profile_8bim);
  length=GetStringInfoLength(profile_8bim);
  for (p=datum; p < (datum+length-16); )
  {
    q=p;
    if (LocaleNCompare((char *) p,"8BIM",4) != 0)
      break;
    p+=4;
    p=ReadResourceShort(p,&id);
    p=ReadResourceByte(p,&length_byte);
    p+=length_byte;
    if (((length_byte+1) & 0x01) != 0)
      p++;
    if (p > (datum+length-4))
      break;
    p=ReadResourceLong(p,&value);
     count=(ssize_t) value;
     if ((count & 0x01) != 0)
       count++;
    if ((count < 0) || (p > (datum+length-count)) ||
        (count > (ssize_t) length))
       break;
     if (id != profile_id)
       p+=count;
    else
      {
        size_t
          extent,
          offset;

        ssize_t
          extract_extent;

        StringInfo
          *extract_profile;

        extract_extent=0;
        extent=(datum+length)-(p+count);
        if (profile == (StringInfo *) NULL)
          {
            offset=(q-datum);
            extract_profile=AcquireStringInfo(offset+extent);
            (void) CopyMagickMemory(extract_profile->datum,datum,offset);
          }
        else
          {
            offset=(p-datum);
            extract_extent=profile->length;
            if ((extract_extent & 0x01) != 0)
              extract_extent++;
            extract_profile=AcquireStringInfo(offset+extract_extent+extent);
            (void) CopyMagickMemory(extract_profile->datum,datum,offset-4);
            (void) WriteResourceLong(extract_profile->datum+offset-4,
              (unsigned int) profile->length);
            (void) CopyMagickMemory(extract_profile->datum+offset,
              profile->datum,profile->length);
          }
        (void) CopyMagickMemory(extract_profile->datum+offset+extract_extent,
          p+count,extent);
        (void) AddValueToSplayTree((SplayTreeInfo *) image->profiles,
          ConstantString("8bim"),CloneStringInfo(extract_profile));
        extract_profile=DestroyStringInfo(extract_profile);
        break;
      }
  }
}
