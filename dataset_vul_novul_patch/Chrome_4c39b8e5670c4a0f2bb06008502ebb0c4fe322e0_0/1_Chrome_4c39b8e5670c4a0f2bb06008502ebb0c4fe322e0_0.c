InspectorResourceAgent::InspectorResourceAgent(InspectorPageAgent* pageAgent, InspectorClient* client)
     : InspectorBaseAgent<InspectorResourceAgent>("Network")
     , m_pageAgent(pageAgent)
    , m_client(client)
     , m_frontend(0)
     , m_resourcesData(adoptPtr(new NetworkResourcesData()))
     , m_isRecalculatingStyle(false)
{
}
