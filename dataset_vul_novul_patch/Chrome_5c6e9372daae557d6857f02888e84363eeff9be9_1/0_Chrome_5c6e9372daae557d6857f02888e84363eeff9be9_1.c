void MostVisitedSitesBridge::SetMostVisitedURLsObserver(
void MostVisitedSitesBridge::SetObserver(
     JNIEnv* env,
     const JavaParamRef<jobject>& obj,
     const JavaParamRef<jobject>& j_observer,
    jint num_sites) {
  java_observer_.reset(new JavaObserver(env, j_observer));
  most_visited_->SetMostVisitedURLsObserver(java_observer_.get(), num_sites);
}
