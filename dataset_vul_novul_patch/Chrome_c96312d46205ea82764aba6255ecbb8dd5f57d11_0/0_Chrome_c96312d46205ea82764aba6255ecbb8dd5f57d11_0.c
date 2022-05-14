 void TestResizeAndSwapCallback(void* user_data, int32_t result) {
  const int kMinSize = 1;
   EXPECT(result == PP_OK);
   RenderInfo* info = static_cast<RenderInfo *>(user_data);
   int32_t new_width = kWidth - info->frame_counter * 2;
   int32_t new_height = kHeight - info->frame_counter * 2;
  if (new_width < kMinSize) new_width = kMinSize;
  if (new_height < kMinSize) new_height = kMinSize;
  // Normally ResizeBuffers() would be in response to DidChangeView(...),
  // This test calls it to see if any unexpected crashes or return values occur.
   int32_t resize_result = PPBGraphics3D()->ResizeBuffers(info->graphics3d_id,
       new_width, new_height);
   EXPECT(resize_result == PP_OK);
  glViewport(0, 0, kWidth, kHeight);
   float green = float(info->frame_counter) / float(info->frame_end);
   glClearColor(0.0f, green, 0.0f, 1.0f);
   glClear(GL_COLOR_BUFFER_BIT);
  info->frame_counter += info->frame_increment;
  if (info->frame_counter < info->frame_end) {
    PP_CompletionCallback cc =
        PP_MakeCompletionCallback(TestResizeAndSwapCallback, info);
    int32_t result = PPBGraphics3D()->SwapBuffers(info->graphics3d_id, cc);
    CHECK(PP_OK_COMPLETIONPENDING == result);
  } else {
    PPBCore()->ReleaseResource(info->graphics3d_id);
    delete info;
    TEST_PASSED;
  }
  glSetCurrentContextPPAPI(0);
 }
