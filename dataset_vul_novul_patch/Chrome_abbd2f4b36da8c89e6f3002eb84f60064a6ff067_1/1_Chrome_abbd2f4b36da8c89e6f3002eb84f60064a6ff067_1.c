 void FaviconWebUIHandler::HandleGetFaviconDominantColor(const ListValue* args) {
   std::string path;
   CHECK(args->GetString(0, &path));
  DCHECK(StartsWithASCII(path, "chrome://favicon/", false)) << "path is "
                                                            << path;
  path = path.substr(arraysize("chrome://favicon/") - 1);
 
   double id;
   CHECK(args->GetDouble(1, &id));

  FaviconService* favicon_service =
      web_ui_->GetProfile()->GetFaviconService(Profile::EXPLICIT_ACCESS);
  if (!favicon_service || path.empty())
    return;

  FaviconService::Handle handle = favicon_service->GetFaviconForURL(
      GURL(path),
      history::FAVICON,
      &consumer_,
      NewCallback(this, &FaviconWebUIHandler::OnFaviconDataAvailable));
  consumer_.SetClientData(favicon_service, handle, static_cast<int>(id));
}
