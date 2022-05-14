PageSerializer::PageSerializer(Vector<SerializedResource>* resources, LinkLocalPathMap* urls, String directory)
PageSerializer::PageSerializer(Vector<SerializedResource>* resources)
     : m_resources(resources)
     , m_blankFrameCounter(0)
 {
 }
