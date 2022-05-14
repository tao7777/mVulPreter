void AwContents::UpdateScrollState(gfx::Vector2d max_scroll_offset,
                                   gfx::SizeF contents_size_dip,
                                    float page_scale_factor,
                                    float min_page_scale_factor,
                                    float max_page_scale_factor) {
  DCHECK_CURRENTLY_ON(BrowserThread::UI);
  JNIEnv* env = AttachCurrentThread();
  ScopedJavaLocalRef<jobject> obj = java_ref_.get(env);
  if (obj.is_null())
    return;
  Java_AwContents_updateScrollState(env,
                                    obj.obj(),
                                    max_scroll_offset.x(),
                                    max_scroll_offset.y(),
                                    contents_size_dip.width(),
                                    contents_size_dip.height(),
                                    page_scale_factor,
                                    min_page_scale_factor,
                                     max_page_scale_factor);
 }
