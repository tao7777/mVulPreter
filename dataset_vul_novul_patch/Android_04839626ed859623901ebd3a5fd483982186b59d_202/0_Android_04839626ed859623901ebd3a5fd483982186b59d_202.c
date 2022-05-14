void Block::SetKey(bool bKey)
Block::Lacing Block::GetLacing() const {
  const int value = int(m_flags & 0x06) >> 1;
  return static_cast<Lacing>(value);
 }
