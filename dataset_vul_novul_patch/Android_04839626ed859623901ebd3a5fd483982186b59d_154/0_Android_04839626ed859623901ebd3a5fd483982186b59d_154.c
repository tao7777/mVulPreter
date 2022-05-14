bool Block::IsKey() const
void Block::SetKey(bool bKey) {
  if (bKey)
    m_flags |= static_cast<unsigned char>(1 << 7);
  else
    m_flags &= 0x7F;
 }
