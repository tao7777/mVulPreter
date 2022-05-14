bool FakeOffscreenCanvasSurfaceImpl::isSurfaceInSurfaceMap(const cc::SurfaceId& surfaceId)
{
    return m_fakeSurfaceMap.contains(surfaceId);
}
