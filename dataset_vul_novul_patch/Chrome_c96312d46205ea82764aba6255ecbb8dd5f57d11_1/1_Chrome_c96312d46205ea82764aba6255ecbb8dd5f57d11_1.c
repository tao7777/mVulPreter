 void TestResizeBuffersWithDepthBuffer() {
   int32_t attribs[] = {
       PP_GRAPHICS3DATTRIB_WIDTH, kWidth,
       PP_GRAPHICS3DATTRIB_HEIGHT, kHeight,
      PP_GRAPHICS3DATTRIB_DEPTH_SIZE, 32,
       PP_GRAPHICS3DATTRIB_NONE};
   TestResizeBuffers(attribs);
 }
