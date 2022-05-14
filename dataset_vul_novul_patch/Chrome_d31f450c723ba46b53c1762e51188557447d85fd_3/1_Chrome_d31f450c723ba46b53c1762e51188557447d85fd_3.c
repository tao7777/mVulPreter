 void UpdateAtlas::didSwapBuffers()
 {
     m_areaAllocator.clear();
    buildLayoutIfNeeded();
 }
