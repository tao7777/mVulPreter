void InspectorController::initializeDeferredAgents()
{
    if (m_deferredAgentsInitialized)
        return;
    m_deferredAgentsInitialized = true;

     InjectedScriptManager* injectedScriptManager = m_injectedScriptManager.get();
     InspectorOverlay* overlay = m_overlay.get();
 
    OwnPtr<InspectorResourceAgent> resourceAgentPtr(InspectorResourceAgent::create(m_pageAgent, m_inspectorClient));
     InspectorResourceAgent* resourceAgent = resourceAgentPtr.get();
     m_agents.append(resourceAgentPtr.release());
 
    m_agents.append(InspectorCSSAgent::create(m_domAgent, m_pageAgent, resourceAgent));

    m_agents.append(InspectorDOMStorageAgent::create(m_pageAgent));

    m_agents.append(InspectorMemoryAgent::create());

    m_agents.append(InspectorApplicationCacheAgent::create(m_pageAgent));

    PageScriptDebugServer* pageScriptDebugServer = &PageScriptDebugServer::shared();

    OwnPtr<InspectorDebuggerAgent> debuggerAgentPtr(PageDebuggerAgent::create(pageScriptDebugServer, m_pageAgent, injectedScriptManager, overlay));
    InspectorDebuggerAgent* debuggerAgent = debuggerAgentPtr.get();
    m_agents.append(debuggerAgentPtr.release());

    m_agents.append(InspectorDOMDebuggerAgent::create(m_domAgent, debuggerAgent));

    m_agents.append(InspectorProfilerAgent::create(injectedScriptManager, overlay));

    m_agents.append(InspectorHeapProfilerAgent::create(injectedScriptManager));

    m_agents.append(InspectorCanvasAgent::create(m_pageAgent, injectedScriptManager));

    m_agents.append(InspectorInputAgent::create(m_page, m_inspectorClient));
}
