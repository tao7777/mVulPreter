    void swapBuffers()
    {
         if (m_xPixmap)
             return;
 
#if PLATFORM(QT)
        if (!m_surface->isVisible())
            return;
        while (!m_surface->isExposed())
            QCoreApplication::processEvents();
        QOpenGLContext* glContext = QOpenGLContext::currentContext();
        if (m_surface && glContext) {
            GLint oldFBO;
            glGetIntegerv(GL_FRAMEBUFFER_BINDING, &oldFBO);
            pGlBindFramebuffer(GL_FRAMEBUFFER, glContext->defaultFramebufferObject());
            glContext->swapBuffers(m_surface.get());
            pGlBindFramebuffer(GL_FRAMEBUFFER, oldFBO);
        }
#elif PLATFORM(EFL)
         GLXContext glContext = glXGetCurrentContext();
 
         if (m_surface && glContext) {
            GLint oldFBO;
            glGetIntegerv(GL_FRAMEBUFFER_BINDING, &oldFBO);
            pGlBindFramebuffer(GL_FRAMEBUFFER, 0);
             glXSwapBuffers(m_display, m_surface);
             pGlBindFramebuffer(GL_FRAMEBUFFER, oldFBO);
         }
#endif
     }
