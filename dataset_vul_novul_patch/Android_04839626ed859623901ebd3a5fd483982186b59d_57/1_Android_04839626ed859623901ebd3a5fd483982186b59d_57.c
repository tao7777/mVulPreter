const unsigned char* Track::GetCodecPrivate(size_t& size) const
{
    size = m_info.codecPrivateSize;
    return m_info.codecPrivate;
}
