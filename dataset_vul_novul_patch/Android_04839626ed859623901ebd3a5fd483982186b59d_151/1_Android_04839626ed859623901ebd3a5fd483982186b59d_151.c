void EBMLHeader::Init()
{
    m_version = 1;
    m_readVersion = 1;
    m_maxIdLength = 4;
    m_maxSizeLength = 8;
 
    if (m_docType)
    {
        delete[] m_docType;
        m_docType = NULL;
    }
 
    m_docTypeVersion = 1;
    m_docTypeReadVersion = 1;
}
