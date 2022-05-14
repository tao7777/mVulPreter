InspectorController::InspectorController(Page* page, InspectorClient* inspectorClient)
    : m_instrumentingAgents(InstrumentingAgents::create())
    , m_injectedScriptManager(InjectedScriptManager::createForPage())
    , m_state(adoptPtr(new InspectorCompositeState(inspectorClient)))
    , m_overlay(InspectorOverlay::create(page, inspectorClient))
    , m_page(page)
    , m_inspectorClient(inspectorClient)
    , m_isUnderTest(false)
{
    m_agents.append(InspectorAgent::create(page, m_injectedScriptManager.get(), m_instrumentingAgents.get(), m_state.get()));

    OwnPtr<InspectorPageAgent> pageAgentPtr(InspectorPageAgent::create(m_instrumentingAgents.get(), page, m_state.get(), m_injectedScriptManager.get(), inspectorClient, m_overlay.get()));
    InspectorPageAgent* pageAgent = pageAgentPtr.get();
    m_agents.append(pageAgentPtr.release());

    OwnPtr<InspectorDOMAgent> domAgentPtr(InspectorDOMAgent::create(m_instrumentingAgents.get(), pageAgent, m_state.get(), m_injectedScriptManager.get(), m_overlay.get(), inspectorClient));
     InspectorDOMAgent* domAgent = domAgentPtr.get();
     m_agents.append(domAgentPtr.release());
 
    OwnPtr<InspectorResourceAgent> resourceAgentPtr(InspectorResourceAgent::create(m_instrumentingAgents.get(), pageAgent, inspectorClient, m_state.get()));
     InspectorResourceAgent* resourceAgent = resourceAgentPtr.get();
     m_agents.append(resourceAgentPtr.release());
 
    m_agents.append(InspectorCSSAgent::create(m_instrumentingAgents.get(), m_state.get(), domAgent, pageAgent, resourceAgent));

    m_agents.append(InspectorDatabaseAgent::create(m_instrumentingAgents.get(), m_state.get()));

    m_agents.append(InspectorIndexedDBAgent::create(m_instrumentingAgents.get(), m_state.get(), m_injectedScriptManager.get(), pageAgent));

    m_agents.append(InspectorFileSystemAgent::create(m_instrumentingAgents.get(), pageAgent, m_state.get()));

    m_agents.append(InspectorDOMStorageAgent::create(m_instrumentingAgents.get(), pageAgent, m_state.get()));

    OwnPtr<InspectorMemoryAgent> memoryAgentPtr(InspectorMemoryAgent::create(m_instrumentingAgents.get(), m_state.get()));
    m_memoryAgent = memoryAgentPtr.get();
    m_agents.append(memoryAgentPtr.release());

    OwnPtr<InspectorTimelineAgent> timelineAgentPtr(InspectorTimelineAgent::create(m_instrumentingAgents.get(), pageAgent, m_memoryAgent, domAgent, m_state.get(),
        InspectorTimelineAgent::PageInspector, inspectorClient));
    m_timelineAgent = timelineAgentPtr.get();
    m_agents.append(timelineAgentPtr.release());

    m_agents.append(InspectorApplicationCacheAgent::create(m_instrumentingAgents.get(), m_state.get(), pageAgent));

    PageScriptDebugServer* pageScriptDebugServer = &PageScriptDebugServer::shared();

    m_agents.append(PageRuntimeAgent::create(m_instrumentingAgents.get(), m_state.get(), m_injectedScriptManager.get(), pageScriptDebugServer, page, pageAgent));

    OwnPtr<InspectorConsoleAgent> consoleAgentPtr(PageConsoleAgent::create(m_instrumentingAgents.get(), m_state.get(), m_injectedScriptManager.get(), domAgent, m_timelineAgent));
    InspectorConsoleAgent* consoleAgent = consoleAgentPtr.get();
    m_agents.append(consoleAgentPtr.release());

    OwnPtr<InspectorDebuggerAgent> debuggerAgentPtr(PageDebuggerAgent::create(m_instrumentingAgents.get(), m_state.get(), pageScriptDebugServer, pageAgent, m_injectedScriptManager.get(), m_overlay.get()));
    InspectorDebuggerAgent* debuggerAgent = debuggerAgentPtr.get();
    m_agents.append(debuggerAgentPtr.release());

    m_agents.append(InspectorDOMDebuggerAgent::create(m_instrumentingAgents.get(), m_state.get(), domAgent, debuggerAgent));

    m_agents.append(InspectorProfilerAgent::create(m_instrumentingAgents.get(), consoleAgent, m_state.get(), m_injectedScriptManager.get()));

    m_agents.append(InspectorHeapProfilerAgent::create(m_instrumentingAgents.get(), m_state.get(), m_injectedScriptManager.get()));


    m_agents.append(InspectorWorkerAgent::create(m_instrumentingAgents.get(), m_state.get()));

    m_agents.append(InspectorCanvasAgent::create(m_instrumentingAgents.get(), m_state.get(), pageAgent, m_injectedScriptManager.get()));

    m_agents.append(InspectorInputAgent::create(m_instrumentingAgents.get(), m_state.get(), page, inspectorClient));

    m_agents.append(InspectorLayerTreeAgent::create(m_instrumentingAgents.get(), m_state.get(), domAgent, page));

    ASSERT_ARG(inspectorClient, inspectorClient);
    m_injectedScriptManager->injectedScriptHost()->init(m_instrumentingAgents.get(), pageScriptDebugServer);
}
