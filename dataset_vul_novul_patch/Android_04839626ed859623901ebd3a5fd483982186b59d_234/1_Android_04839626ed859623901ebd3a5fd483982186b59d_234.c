Track::~Track()
{
    Info& info = const_cast<Info&>(m_info);
    info.Clear();
 
    ContentEncoding** i = content_encoding_entries_;
    ContentEncoding** const j = content_encoding_entries_end_;
 
    while (i != j) {
        ContentEncoding* const encoding = *i++;
        delete encoding;
    }
 
    delete [] content_encoding_entries_;
 }
