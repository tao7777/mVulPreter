InspectorResourceAgent::InspectorResourceAgent(InstrumentingAgents* instrumentingAgents, InspectorPageAgent* pageAgent, InspectorClient* client, InspectorCompositeState* state, InspectorOverlay* overlay)
InspectorResourceAgent::InspectorResourceAgent(InstrumentingAgents* instrumentingAgents, InspectorPageAgent* pageAgent, InspectorClient* client, InspectorCompositeState* state)
     : InspectorBaseAgent<InspectorResourceAgent>("Network", instrumentingAgents, state)
     , m_pageAgent(pageAgent)
     , m_client(client)
     , m_frontend(0)
     , m_resourcesData(adoptPtr(new NetworkResourcesData()))
     , m_isRecalculatingStyle(false)
{
}
