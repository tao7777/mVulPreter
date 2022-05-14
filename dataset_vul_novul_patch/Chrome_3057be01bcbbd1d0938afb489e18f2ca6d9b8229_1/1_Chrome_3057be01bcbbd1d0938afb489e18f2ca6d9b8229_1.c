Platform::IntPoint InRegionScrollableArea::calculateMinimumScrollPosition(const Platform::IntSize& viewportSize, float overscrollLimitFactor) const
{
    ASSERT(!allowsOverscroll());
    return Platform::IntPoint(-(viewportSize.width() * overscrollLimitFactor),
                              -(viewportSize.height() * overscrollLimitFactor));
}
