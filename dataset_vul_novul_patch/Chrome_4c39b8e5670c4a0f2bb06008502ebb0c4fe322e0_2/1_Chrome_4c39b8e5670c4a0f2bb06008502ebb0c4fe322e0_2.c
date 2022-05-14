void InspectorClientImpl::clearBrowserCookies()
{
    if (WebDevToolsAgentImpl* agent = devToolsAgent())
        agent->clearBrowserCookies();
}
