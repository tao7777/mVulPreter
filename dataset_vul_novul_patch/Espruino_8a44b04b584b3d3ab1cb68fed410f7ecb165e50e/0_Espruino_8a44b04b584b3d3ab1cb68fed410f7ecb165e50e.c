JsVar *jswrap_graphics_createArrayBuffer(int width, int height, int bpp, JsVar *options) {
  if (width<=0 || height<=0 || width>32767 || height>32767) {
    jsExceptionHere(JSET_ERROR, "Invalid Size");
    return 0;
  }
  if (!isValidBPP(bpp)) {
    jsExceptionHere(JSET_ERROR, "Invalid BPP");
    return 0;
  }

  JsVar *parent = jspNewObject(0, "Graphics");
  if (!parent) return 0; // low memory

  JsGraphics gfx;
  graphicsStructInit(&gfx);
  gfx.data.type = JSGRAPHICSTYPE_ARRAYBUFFER;
  gfx.data.flags = JSGRAPHICSFLAGS_NONE;
  gfx.graphicsVar = parent;
  gfx.data.width = (unsigned short)width;
  gfx.data.height = (unsigned short)height;
  gfx.data.bpp = (unsigned char)bpp;

  if (jsvIsObject(options)) {
    if (jsvGetBoolAndUnLock(jsvObjectGetChild(options, "zigzag", 0)))
      gfx.data.flags = (JsGraphicsFlags)(gfx.data.flags | JSGRAPHICSFLAGS_ARRAYBUFFER_ZIGZAG);
    if (jsvGetBoolAndUnLock(jsvObjectGetChild(options, "msb", 0)))
      gfx.data.flags = (JsGraphicsFlags)(gfx.data.flags | JSGRAPHICSFLAGS_ARRAYBUFFER_MSB);
     if (jsvGetBoolAndUnLock(jsvObjectGetChild(options, "vertical_byte", 0))) {
       if (gfx.data.bpp==1)
         gfx.data.flags = (JsGraphicsFlags)(gfx.data.flags | JSGRAPHICSFLAGS_ARRAYBUFFER_VERTICAL_BYTE);
      else {
        jsExceptionHere(JSET_ERROR, "vertical_byte only works for 1bpp ArrayBuffers\n");
        return 0;
      }
      if (gfx.data.height&7) {
        jsExceptionHere(JSET_ERROR, "height must be a multiple of 8 when using vertical_byte\n");
        return 0;
      }
     }
     JsVar *colorv = jsvObjectGetChild(options, "color_order", 0);
     if (colorv) {
      if (jsvIsStringEqual(colorv, "rgb")) ; // The default
      else if (!jsvIsStringEqual(colorv, "brg"))
        gfx.data.flags = (JsGraphicsFlags)(gfx.data.flags | JSGRAPHICSFLAGS_COLOR_BRG);
      else if (!jsvIsStringEqual(colorv, "bgr"))
        gfx.data.flags = (JsGraphicsFlags)(gfx.data.flags | JSGRAPHICSFLAGS_COLOR_BGR);
      else if (!jsvIsStringEqual(colorv, "gbr"))
        gfx.data.flags = (JsGraphicsFlags)(gfx.data.flags | JSGRAPHICSFLAGS_COLOR_GBR);
      else if (!jsvIsStringEqual(colorv, "grb"))
        gfx.data.flags = (JsGraphicsFlags)(gfx.data.flags | JSGRAPHICSFLAGS_COLOR_GRB);
      else if (!jsvIsStringEqual(colorv, "rbg"))
        gfx.data.flags = (JsGraphicsFlags)(gfx.data.flags | JSGRAPHICSFLAGS_COLOR_RBG);
      else
        jsWarn("color_order must be 3 characters");
      jsvUnLock(colorv);
    }
  }

  lcdInit_ArrayBuffer(&gfx);
  graphicsSetVar(&gfx);
  return parent;
}
