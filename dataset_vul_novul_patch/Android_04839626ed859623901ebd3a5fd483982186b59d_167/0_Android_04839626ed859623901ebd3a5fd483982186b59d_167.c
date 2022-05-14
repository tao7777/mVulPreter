long long EBMLHeader::Parse(
  if (status < 0)  // error
    return status;
 
  pos = 0;
  long long end = (available >= 1024) ? 1024 : available;
 
  for (;;) {
    unsigned char b = 0;
 
    while (pos < end) {
      status = pReader->Read(pos, 1, &b);

      if (status < 0)  // error
         return status;
 
      if (b == 0x1A)
        break;
 
      ++pos;
     }
 
    if (b != 0x1A) {
      if (pos >= 1024)
        return E_FILE_FORMAT_INVALID;  // don't bother looking anymore
 
      if ((total >= 0) && ((total - available) < 5))
        return E_FILE_FORMAT_INVALID;
 
      return available + 5;  // 5 = 4-byte ID + 1st byte of size
    }

    if ((total >= 0) && ((total - pos) < 5))
      return E_FILE_FORMAT_INVALID;

    if ((available - pos) < 5)
      return pos + 5;  // try again later

    long len;

    const long long result = ReadUInt(pReader, pos, len);

    if (result < 0)  // error
      return result;

    if (result == 0x0A45DFA3) {  // EBML Header ID
      pos += len;  // consume ID
      break;
    }

    ++pos;  // throw away just the 0x1A byte, and try again
  }

  // pos designates start of size field

  // get length of size field

  long len;
  long long result = GetUIntLength(pReader, pos, len);

  if (result < 0)  // error
    return result;

  if (result > 0)  // need more data
    return result;

  assert(len > 0);
  assert(len <= 8);

  if ((total >= 0) && ((total - pos) < len))
    return E_FILE_FORMAT_INVALID;

  if ((available - pos) < len)
    return pos + len;  // try again later

  // get the EBML header size

  result = ReadUInt(pReader, pos, len);

  if (result < 0)  // error
    return result;

  pos += len;  // consume size field

  // pos now designates start of payload

  if ((total >= 0) && ((total - pos) < result))
    return E_FILE_FORMAT_INVALID;

  if ((available - pos) < result)
    return pos + result;

  end = pos + result;

  Init();

  while (pos < end) {
    long long id, size;

    status = ParseElementHeader(pReader, pos, end, id, size);

    if (status < 0)  // error
      return status;

    if (size == 0)  // weird
      return E_FILE_FORMAT_INVALID;

    if (id == 0x0286) {  // version
      m_version = UnserializeUInt(pReader, pos, size);

      if (m_version <= 0)
        return E_FILE_FORMAT_INVALID;
    } else if (id == 0x02F7) {  // read version
      m_readVersion = UnserializeUInt(pReader, pos, size);

      if (m_readVersion <= 0)
        return E_FILE_FORMAT_INVALID;
    } else if (id == 0x02F2) {  // max id length
      m_maxIdLength = UnserializeUInt(pReader, pos, size);

      if (m_maxIdLength <= 0)
        return E_FILE_FORMAT_INVALID;
    } else if (id == 0x02F3) {  // max size length
      m_maxSizeLength = UnserializeUInt(pReader, pos, size);

      if (m_maxSizeLength <= 0)
        return E_FILE_FORMAT_INVALID;
    } else if (id == 0x0282) {  // doctype
      if (m_docType)
        return E_FILE_FORMAT_INVALID;

      status = UnserializeString(pReader, pos, size, m_docType);

      if (status)  // error
        return status;
    } else if (id == 0x0287) {  // doctype version
      m_docTypeVersion = UnserializeUInt(pReader, pos, size);

      if (m_docTypeVersion <= 0)
        return E_FILE_FORMAT_INVALID;
    } else if (id == 0x0285) {  // doctype read version
      m_docTypeReadVersion = UnserializeUInt(pReader, pos, size);

      if (m_docTypeReadVersion <= 0)
        return E_FILE_FORMAT_INVALID;
    }

    pos += size;
  }

  assert(pos == end);
  return 0;
}

Segment::Segment(IMkvReader* pReader, long long elem_start,
                 // long long elem_size,
                 long long start, long long size)
    : m_pReader(pReader),
      m_element_start(elem_start),
      // m_element_size(elem_size),
      m_start(start),
      m_size(size),
      m_pos(start),
      m_pUnknownSize(0),
      m_pSeekHead(NULL),
      m_pInfo(NULL),
      m_pTracks(NULL),
      m_pCues(NULL),
      m_pChapters(NULL),
      m_clusters(NULL),
      m_clusterCount(0),
      m_clusterPreloadCount(0),
      m_clusterSize(0) {}

Segment::~Segment() {
  const long count = m_clusterCount + m_clusterPreloadCount;

  Cluster** i = m_clusters;
  Cluster** j = m_clusters + count;

  while (i != j) {
    Cluster* const p = *i++;
    assert(p);

    delete p;
  }

  delete[] m_clusters;

  delete m_pTracks;
  delete m_pInfo;
  delete m_pCues;
  delete m_pChapters;
  delete m_pSeekHead;
}

long long Segment::CreateInstance(IMkvReader* pReader, long long pos,
                                  Segment*& pSegment) {
  assert(pReader);
  assert(pos >= 0);

  pSegment = NULL;

  long long total, available;

  const long status = pReader->Length(&total, &available);

  if (status < 0)  // error
    return status;

  if (available < 0)
    return -1;

  if ((total >= 0) && (available > total))
    return -1;

  // I would assume that in practice this loop would execute
  // exactly once, but we allow for other elements (e.g. Void)
  // to immediately follow the EBML header.  This is fine for
  // the source filter case (since the entire file is available),
  // but in the splitter case over a network we should probably
  // just give up early.  We could for example decide only to
  // execute this loop a maximum of, say, 10 times.
  // TODO:
  // There is an implied "give up early" by only parsing up
  // to the available limit.  We do do that, but only if the
  // total file size is unknown.  We could decide to always
  // use what's available as our limit (irrespective of whether
  // we happen to know the total file length).  This would have
  // as its sense "parse this much of the file before giving up",
  // which a slightly different sense from "try to parse up to
  // 10 EMBL elements before giving up".

  for (;;) {
    if ((total >= 0) && (pos >= total))
      return E_FILE_FORMAT_INVALID;

    // Read ID
     long len;
     long long result = GetUIntLength(pReader, pos, len);
 
    if (result)  // error, or too few available bytes
      return result;
 
    if ((total >= 0) && ((pos + len) > total))
      return E_FILE_FORMAT_INVALID;
 
    if ((pos + len) > available)
      return pos + len;
 
    const long long idpos = pos;
    const long long id = ReadUInt(pReader, pos, len);
 
    if (id < 0)  // error
      return id;
 
    pos += len;  // consume ID
 
    // Read Size
 
    result = GetUIntLength(pReader, pos, len);
 
    if (result)  // error, or too few available bytes
      return result;
 
    if ((total >= 0) && ((pos + len) > total))
      return E_FILE_FORMAT_INVALID;
 
    if ((pos + len) > available)
      return pos + len;
 
    long long size = ReadUInt(pReader, pos, len);
 
    if (size < 0)  // error
      return size;
 
    pos += len;  // consume length of size of element
 
    // Pos now points to start of payload
 
    // Handle "unknown size" for live streaming of webm files.
    const long long unknown_size = (1LL << (7 * len)) - 1;
 
    if (id == 0x08538067) {  // Segment ID
      if (size == unknown_size)
        size = -1;
 
      else if (total < 0)
        size = -1;
 
      else if ((pos + size) > total)
        size = -1;
 
      pSegment = new (std::nothrow) Segment(pReader, idpos,
                                            // elem_size
                                            pos, size);
 
      if (pSegment == 0)
        return -1;  // generic error
 
      return 0;  // success
     }
 
    if (size == unknown_size)
      return E_FILE_FORMAT_INVALID;

    if ((total >= 0) && ((pos + size) > total))
      return E_FILE_FORMAT_INVALID;

    if ((pos + size) > available)
      return pos + size;

    pos += size;  // consume payload
  }
 }
