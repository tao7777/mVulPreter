long Cluster::CreateBlockGroup(long long start_offset, long long size,
 long long discard_padding) {
  assert(m_entries);
  assert(m_entries_size > 0);
  assert(m_entries_count >= 0);
  assert(m_entries_count < m_entries_size);

 IMkvReader* const pReader = m_pSegment->m_pReader;

 long long pos = start_offset;
 const long long stop = start_offset + size;


 long long prev = 1; // nonce
 long long next = 0; // nonce
 long long duration = -1; // really, this is unsigned

 long long bpos = -1;
 long long bsize = -1;

 
   while (pos < stop) {
     long len;
    const long long id = ReadUInt(pReader, pos, len);
    assert(id >= 0);  // TODO
    assert((pos + len) <= stop);
 
     pos += len;  // consume ID
 
 const long long size = ReadUInt(pReader, pos, len);
    assert(size >= 0); // TODO
    assert((pos + len) <= stop);

    pos += len; // consume size

 if (id == 0x21) { // Block ID
 if (bpos < 0) { // Block ID
        bpos = pos;

         bsize = size;
       }
     } else if (id == 0x1B) {  // Duration ID
      assert(size <= 8);
 
       duration = UnserializeUInt(pReader, pos, size);
      assert(duration >= 0);  // TODO
     } else if (id == 0x7B) {  // ReferenceBlock
      assert(size <= 8);
       const long size_ = static_cast<long>(size);
 
       long long time;

 long status = UnserializeInt(pReader, pos, size_, time);
      assert(status == 0);
 if (status != 0)
 return -1;

 if (time <= 0) // see note above
        prev = time;
 else // weird
        next = time;

     }
 
     pos += size;  // consume payload
    assert(pos <= stop);
   }
 
  assert(pos == stop);
  assert(bpos >= 0);
   assert(bsize >= 0);
 
   const long idx = m_entries_count;

 BlockEntry** const ppEntry = m_entries + idx;
 BlockEntry*& pEntry = *ppEntry;

  pEntry = new (std::nothrow)
 BlockGroup(this, idx, bpos, bsize, prev, next, duration, discard_padding);

 if (pEntry == NULL)
 return -1; // generic error

 BlockGroup* const p = static_cast<BlockGroup*>(pEntry);

 const long status = p->Parse();

 if (status == 0) { // success
 ++m_entries_count;
 return 0;
 }

 delete pEntry;
  pEntry = 0;

 return status;
}
