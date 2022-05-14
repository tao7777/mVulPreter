void AwContents::ScrollContainerViewTo(gfx::Vector2d new_value) {
void AwContents::ScrollContainerViewTo(const gfx::Vector2d& new_value) {
   DCHECK_CURRENTLY_ON(BrowserThread::UI);
   JNIEnv* env = AttachCurrentThread();
   ScopedJavaLocalRef<jobject> obj = java_ref_.get(env);
  if (obj.is_null())
    return;
  Java_AwContents_scrollContainerViewTo(
       env, obj.obj(), new_value.x(), new_value.y());
 }
