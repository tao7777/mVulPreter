LinkChangeSerializerMarkupAccumulator::LinkChangeSerializerMarkupAccumulator(PageSerializer* serializer, Document* document, Vector<Node*>* nodes, LinkLocalPathMap* links, String directoryName)
    : SerializerMarkupAccumulator(serializer, document, nodes)
    , m_replaceLinks(links)
    , m_directoryName(directoryName)
{
}
