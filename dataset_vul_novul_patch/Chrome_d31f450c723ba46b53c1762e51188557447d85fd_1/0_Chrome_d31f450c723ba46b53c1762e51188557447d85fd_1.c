 UpdateAtlas::UpdateAtlas(int dimension, ShareableBitmap::Flags flags)
     : m_flags(flags)
    , m_inactivityInSeconds(0)
 {
     IntSize size = nextPowerOfTwo(IntSize(dimension, dimension));
     m_surface = ShareableSurface::create(size, flags, ShareableSurface::SupportsGraphicsSurface);
}
