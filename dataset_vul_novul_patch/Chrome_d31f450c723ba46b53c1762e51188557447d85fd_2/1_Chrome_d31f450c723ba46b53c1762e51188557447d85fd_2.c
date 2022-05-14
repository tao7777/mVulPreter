 PassOwnPtr<GraphicsContext> UpdateAtlas::beginPaintingOnAvailableBuffer(ShareableSurface::Handle& handle, const WebCore::IntSize& size, IntPoint& offset)
 {
     buildLayoutIfNeeded();
     IntRect rect = m_areaAllocator->allocate(size);
 
    if (rect.isEmpty())
        return PassOwnPtr<GraphicsContext>();

    if (!m_surface->createHandle(handle))
        return PassOwnPtr<WebCore::GraphicsContext>();

    offset = rect.location();
    OwnPtr<GraphicsContext> graphicsContext = m_surface->createGraphicsContext(rect);

    if (flags() & ShareableBitmap::SupportsAlpha) {
        graphicsContext->setCompositeOperation(CompositeCopy);
        graphicsContext->fillRect(IntRect(IntPoint::zero(), size), Color::transparent, ColorSpaceDeviceRGB);
        graphicsContext->setCompositeOperation(CompositeSourceOver);
    }

    return graphicsContext.release();
}
