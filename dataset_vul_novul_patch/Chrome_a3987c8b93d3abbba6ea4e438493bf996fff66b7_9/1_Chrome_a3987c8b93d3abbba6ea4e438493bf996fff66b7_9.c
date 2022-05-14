void MockCanvasSurfaceLayerBridgeClient::asyncRequestSurfaceCreation(const cc::SurfaceId& surfaceId)
{
    m_service->RequestSurfaceCreation(surfaceId);
}
