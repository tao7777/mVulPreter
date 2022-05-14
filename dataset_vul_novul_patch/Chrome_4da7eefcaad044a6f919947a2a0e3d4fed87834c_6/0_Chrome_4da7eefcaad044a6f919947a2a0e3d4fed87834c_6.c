    void swapBuffers()
    {
         if (m_xPixmap)
             return;
 
         GLXContext glContext = glXGetCurrentContext();
 
         if (m_surface && glContext) {
            GLint oldFBO;
            glGetIntegerv(GL_FRAMEBUFFER_BINDING, &oldFBO);
            pGlBindFramebuffer(GL_FRAMEBUFFER, 0);
             glXSwapBuffers(m_display, m_surface);
             pGlBindFramebuffer(GL_FRAMEBUFFER, oldFBO);
         }
     }
