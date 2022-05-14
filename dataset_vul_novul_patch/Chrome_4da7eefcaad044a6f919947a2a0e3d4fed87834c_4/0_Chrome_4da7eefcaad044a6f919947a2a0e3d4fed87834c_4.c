     void makeCurrent()
     {
         m_detachedContext = glXGetCurrentContext();
         m_detachedSurface = glXGetCurrentDrawable();
         if (m_surface && m_glContext)
             glXMakeCurrent(m_display, m_surface, m_glContext);
     }
