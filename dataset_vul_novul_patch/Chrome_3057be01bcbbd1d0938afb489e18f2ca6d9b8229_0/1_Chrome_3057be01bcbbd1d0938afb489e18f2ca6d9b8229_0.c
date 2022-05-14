Platform::IntPoint InRegionScrollableArea::calculateMaximumScrollPosition(const Platform::IntSize& viewportSize, const Platform::IntSize& contentsSize, float overscrollLimitFactor) const
{
    ASSERT(!allowsOverscroll());
    return Platform::IntPoint(std::max(contentsSize.width() - viewportSize.width(), 0) + overscrollLimitFactor,
                              std::max(contentsSize.height() - viewportSize.height(), 0) + overscrollLimitFactor);
}
