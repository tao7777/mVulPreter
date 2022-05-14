     void copyFromTexture(uint32_t texture, const IntRect& sourceRect)
    {
        makeCurrent();
        int x = sourceRect.x();
        int y = sourceRect.y();
        int width = sourceRect.width();
        int height = sourceRect.height();

        glPushAttrib(GL_ALL_ATTRIB_BITS);
        GLint previousFBO;
        glGetIntegerv(GL_FRAMEBUFFER_BINDING, &previousFBO);

        GLuint originFBO;
        pGlGenFramebuffers(1, &originFBO);
        pGlBindFramebuffer(GL_READ_FRAMEBUFFER, originFBO);
         glBindTexture(GL_TEXTURE_2D, texture);
         pGlFramebufferTexture2D(GL_READ_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture, 0);
 
#if PLATFORM(QT)
        pGlBindFramebuffer(GL_DRAW_FRAMEBUFFER, glContext()->defaultFramebufferObject());
#elif PLATFORM(EFL)
         pGlBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
#endif
         pGlBlitFramebuffer(x, y, width, height, x, y, width, height, GL_COLOR_BUFFER_BIT, GL_LINEAR);
 
         pGlFramebufferTexture2D(GL_READ_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, 0, 0);
        glBindTexture(GL_TEXTURE_2D, 0);
        pGlBindFramebuffer(GL_FRAMEBUFFER, previousFBO);
        pGlDeleteFramebuffers(1, &originFBO);

        glPopAttrib();
        doneCurrent();
    }
