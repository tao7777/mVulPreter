     ~OffScreenRootWindow()
     {
         if (!--refCount) {
             XUnmapWindow(display, window);
             XDestroyWindow(display, window);
         }
     }
