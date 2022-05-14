PageSerializer::PageSerializer(Vector<SerializedResource>* resources, LinkLocalPathMap* urls, String directory)
     : m_resources(resources)
    , m_URLs(urls)
    , m_directory(directory)
     , m_blankFrameCounter(0)
 {
 }
