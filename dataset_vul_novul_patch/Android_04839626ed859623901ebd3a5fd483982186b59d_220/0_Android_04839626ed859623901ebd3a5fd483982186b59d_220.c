Cluster::~Cluster()
    if (status < 0) {  // error or underflow
      len = 1;
      return status;
     }
 
    ++pos;  // consume flags byte
    assert(pos <= avail);

    if (pos >= block_stop)
      return E_FILE_FORMAT_INVALID;

    const int lacing = int(flags & 0x06) >> 1;

    if ((lacing != 0) && (block_stop > avail)) {
      len = static_cast<long>(block_stop - pos);
      return E_BUFFER_NOT_FULL;
    }

    pos = block_stop;  // consume block-part of block group
    assert(pos <= payload_stop);
  }

  assert(pos == payload_stop);

  status = CreateBlock(0x20,  // BlockGroup ID
                       payload_start, payload_size, discard_padding);
  if (status != 0)
    return status;

  m_pos = payload_stop;

  return 0;  // success
 }
