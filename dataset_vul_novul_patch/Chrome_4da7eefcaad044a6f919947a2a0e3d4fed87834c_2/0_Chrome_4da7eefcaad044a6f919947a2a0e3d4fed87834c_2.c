     void doneCurrent()
     {
         if (m_detachedContext)
            glXMakeCurrent(m_display, m_detachedSurface, m_detachedContext);
         m_detachedContext = 0;
     }
