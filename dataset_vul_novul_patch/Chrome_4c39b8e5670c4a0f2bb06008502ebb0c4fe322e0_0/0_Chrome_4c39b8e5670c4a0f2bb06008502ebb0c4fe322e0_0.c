InspectorResourceAgent::InspectorResourceAgent(InspectorPageAgent* pageAgent, InspectorClient* client)
InspectorResourceAgent::InspectorResourceAgent(InspectorPageAgent* pageAgent)
     : InspectorBaseAgent<InspectorResourceAgent>("Network")
     , m_pageAgent(pageAgent)
     , m_frontend(0)
     , m_resourcesData(adoptPtr(new NetworkResourcesData()))
     , m_isRecalculatingStyle(false)
{
}
