 static inline quint32 swapBgrToRgb(quint32 pixel)
 {
    return ((pixel << 16) & 0xff0000) | ((pixel >> 16) & 0xff) | (pixel & 0xff00ff00);
 }
