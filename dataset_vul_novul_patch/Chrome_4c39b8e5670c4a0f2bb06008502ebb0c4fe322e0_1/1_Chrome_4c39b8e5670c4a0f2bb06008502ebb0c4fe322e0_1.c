void InspectorClientImpl::clearBrowserCache()
{
    if (WebDevToolsAgentImpl* agent = devToolsAgent())
        agent->clearBrowserCache();
}
