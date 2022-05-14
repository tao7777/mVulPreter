bool Block::IsKey() const
{
    return ((m_flags & static_cast<unsigned char>(1 << 7)) != 0);
 }
