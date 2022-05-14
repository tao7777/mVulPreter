    GraphicsSurfacePrivate(const PlatformGraphicsContext3D shareContext = 0)
        : m_display(0)
        , m_xPixmap(0)
        , m_glxPixmap(0)
        , m_glContext(0)
        , m_detachedContext(0)
        , m_detachedSurface(0)
         , m_textureIsYInverted(false)
         , m_hasAlpha(false)
     {
        GLXContext shareContextObject = 0;
 #if PLATFORM(QT)
        if (shareContext) {
            // This code path requires QXcbNativeInterface::nativeResourceForContext() which is not availble in Qt5 on the build bots yet.
            QPlatformNativeInterface* nativeInterface = QGuiApplication::platformNativeInterface();
            shareContextObject = static_cast<GLXContext>(nativeInterface->nativeResourceForContext(QByteArrayLiteral("glxcontext"), shareContext));
            if (!shareContextObject)
                return;
        }
#endif
         m_display = XOpenDisplay(0);
 
         int attributes[] = {
            GLX_LEVEL, 0,
            GLX_DRAWABLE_TYPE, GLX_WINDOW_BIT,
            GLX_RENDER_TYPE,   GLX_RGBA_BIT,
            GLX_RED_SIZE,      1,
            GLX_GREEN_SIZE,    1,
            GLX_BLUE_SIZE,     1,
            GLX_DOUBLEBUFFER,  True,
            None
        };

         int numReturned;
         m_fbConfigs = glXChooseFBConfig(m_display, DefaultScreen(m_display), attributes, &numReturned);
        m_glContext = glXCreateNewContext(m_display, m_fbConfigs[0], GLX_RGBA_TYPE, shareContextObject, true);
     }
