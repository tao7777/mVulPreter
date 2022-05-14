InspectorOverlay::InspectorOverlay(Page* page, InspectorClient* client)
    : m_page(page)
    , m_client(client)
    , m_inspectModeEnabled(false)
    , m_drawViewSize(false)
     , m_drawViewSizeWithGrid(false)
     , m_timer(this, &InspectorOverlay::onTimer)
     , m_overlayHost(InspectorOverlayHost::create())
    , m_overrides(0)
    , m_overridesTopOffset(0)
 {
 }
