 static inline quint32 swapBgrToRgb(quint32 pixel)
 {
    return (((pixel << 16) | (pixel >> 16)) & 0x00ff00ff) | (pixel & 0xff00ff00);
 }
