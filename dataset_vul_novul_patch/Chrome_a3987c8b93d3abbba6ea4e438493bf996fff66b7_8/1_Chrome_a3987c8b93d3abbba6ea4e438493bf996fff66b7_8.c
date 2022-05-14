void FakeOffscreenCanvasSurfaceImpl::RequestSurfaceCreation(const cc::SurfaceId& surfaceId)
{
    m_fakeSurfaceMap.append(surfaceId);
}
