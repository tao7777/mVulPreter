void InspectorOverlay::drawOverridesMessage()
{
    RefPtr<JSONObject> data = JSONObject::create();
    if (m_drawViewSize || m_highlightNode || m_highlightQuad)
        data->setBoolean("hidden", true);
    data->setNumber("overrides", m_overrides);
    data->setNumber("topOffset", m_overridesTopOffset);
    evaluateInOverlay("drawOverridesMessage", data.release());
}
