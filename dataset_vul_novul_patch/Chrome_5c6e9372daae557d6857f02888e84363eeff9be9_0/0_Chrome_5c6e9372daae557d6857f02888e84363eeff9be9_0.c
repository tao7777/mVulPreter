void MostVisitedSitesBridge::JavaObserver::OnMostVisitedURLsAvailable(
    const NTPTilesVector& tiles) {
  JNIEnv* env = AttachCurrentThread();
  std::vector<base::string16> titles;
  std::vector<std::string> urls;
  std::vector<std::string> whitelist_icon_paths;
  std::vector<int> sources;

  titles.reserve(tiles.size());
  urls.reserve(tiles.size());
  whitelist_icon_paths.reserve(tiles.size());
  sources.reserve(tiles.size());
  for (const auto& tile : tiles) {
    titles.emplace_back(tile.title);
    urls.emplace_back(tile.url.spec());
     whitelist_icon_paths.emplace_back(tile.whitelist_icon_path.value());
     sources.emplace_back(static_cast<int>(tile.source));
   }
  Java_Observer_onMostVisitedURLsAvailable(
       env, observer_, ToJavaArrayOfStrings(env, titles),
       ToJavaArrayOfStrings(env, urls),
       ToJavaArrayOfStrings(env, whitelist_icon_paths),
      ToJavaIntArray(env, sources));
}
