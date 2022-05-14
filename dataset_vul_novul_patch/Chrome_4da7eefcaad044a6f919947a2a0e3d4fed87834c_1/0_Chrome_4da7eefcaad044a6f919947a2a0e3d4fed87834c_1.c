     uint32_t createSurface(const IntSize& size)
     {
         XVisualInfo* visualInfo = glXGetVisualFromFBConfig(m_display, m_fbConfigs[0]);
         if (!visualInfo)
             return 0;

        Colormap cmap = XCreateColormap(m_display, *m_offScreenWindow.get(m_display), visualInfo->visual, AllocNone);
        XSetWindowAttributes a;
        a.background_pixel = WhitePixel(m_display, 0);
        a.border_pixel = BlackPixel(m_display, 0);
        a.colormap = cmap;
        m_surface = XCreateWindow(m_display, *m_offScreenWindow.get(m_display), 0, 0, size.width(), size.height(),
            0, visualInfo->depth, InputOutput, visualInfo->visual,
            CWBackPixel | CWBorderPixel | CWColormap, &a);
        XSetWindowBackgroundPixmap(m_display, m_surface, 0);
        XCompositeRedirectWindow(m_display, m_surface, CompositeRedirectManual);

        int eventBasep, errorBasep;
        if (!XRenderQueryExtension(m_display, &eventBasep, &errorBasep))
            return 0;
 
         XMapWindow(m_display, m_surface);
         return m_surface;
     }
