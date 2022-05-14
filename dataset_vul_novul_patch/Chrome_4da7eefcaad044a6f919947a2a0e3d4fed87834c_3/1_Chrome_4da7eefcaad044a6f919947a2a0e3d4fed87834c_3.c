    QWindow* get(Display* dpy)
    {
        if (!window) {
            window = new QWindow;
            window->setGeometry(QRect(-1, -1, 1, 1));
            window->create();
            XSetWindowAttributes attributes;
            attributes.override_redirect = true;
            XChangeWindowAttributes(dpy, window->handle()->winId(), X11OverrideRedirect, &attributes);
            window->show();
        }
        return window;
    }
