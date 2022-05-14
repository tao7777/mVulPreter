Block::Lacing Block::GetLacing() const
  const long status = pReader->Read(pos, len, buf);
  return status;
 }
