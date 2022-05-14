SegmentInfo::~SegmentInfo()
{
    delete[] m_pMuxingAppAsUTF8;
    m_pMuxingAppAsUTF8 = NULL;
    delete[] m_pWritingAppAsUTF8;
    m_pWritingAppAsUTF8 = NULL;
    delete[] m_pTitleAsUTF8;
    m_pTitleAsUTF8 = NULL;
}
