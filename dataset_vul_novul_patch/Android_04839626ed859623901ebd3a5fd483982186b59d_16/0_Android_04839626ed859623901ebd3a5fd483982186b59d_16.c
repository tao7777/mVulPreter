int Track::Info::CopyStr(char* Info::*str, Info& dst_) const
  if (src == NULL)
     return 0;

  const size_t len = strlen(src);

  dst = new (std::nothrow) char[len + 1];

  if (dst == NULL)
    return -1;

  strcpy(dst, src);

  return 0;
 }
