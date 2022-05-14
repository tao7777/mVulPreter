bool OutOfProcessInstance::Init(uint32_t argc,
                                 const char* argn[],
                                 const char* argv[]) {
  // the plugin to be loaded in the extension and print preview to avoid
  // exposing sensitive APIs directly to external websites.
   pp::Var document_url_var = pp::URLUtil_Dev::Get()->GetDocumentURL(this);
  if (!document_url_var.is_string())
    return false;
  std::string document_url = document_url_var.AsString();
   std::string extension_url = std::string(kChromeExtension);
  std::string print_preview_url = std::string(kChromePrint);
  if (!base::StringPiece(document_url).starts_with(kChromeExtension) &&
      !base::StringPiece(document_url).starts_with(kChromePrint)) {
    return false;
  }

  // Check if the plugin is full frame. This is passed in from JS.
  for (uint32_t i = 0; i < argc; ++i) {
    if (strcmp(argn[i], "full-frame") == 0) {
      full_ = true;
      break;
     }
   }
 
  if (full_)
    SetPluginToHandleFindRequests();

  pp::VarDictionary translated_strings;
  translated_strings.Set(kType, kJSSetTranslatedStringsType);
  translated_strings.Set(kJSGetPasswordString,
      GetLocalizedString(PP_RESOURCESTRING_PDFGETPASSWORD));
  translated_strings.Set(kJSLoadingString,
      GetLocalizedString(PP_RESOURCESTRING_PDFLOADING));
  translated_strings.Set(kJSLoadFailedString,
      GetLocalizedString(PP_RESOURCESTRING_PDFLOAD_FAILED));
  PostMessage(translated_strings);

  text_input_.reset(new pp::TextInput_Dev(this));

  const char* stream_url = nullptr;
  const char* original_url = nullptr;
  const char* headers = nullptr;
  bool is_material = false;
  for (uint32_t i = 0; i < argc; ++i) {
    if (strcmp(argn[i], "src") == 0)
      original_url = argv[i];
    else if (strcmp(argn[i], "stream-url") == 0)
      stream_url = argv[i];
    else if (strcmp(argn[i], "headers") == 0)
      headers = argv[i];
    else if (strcmp(argn[i], "is-material") == 0)
      is_material = true;
    else if (strcmp(argn[i], "top-toolbar-height") == 0)
      base::StringToInt(argv[i], &top_toolbar_height_);
  }

  if (is_material)
    background_color_ = kBackgroundColorMaterial;
  else
    background_color_ = kBackgroundColor;

  if (!original_url)
    return false;

  if (!stream_url)
    stream_url = original_url;

  if (IsPrintPreviewUrl(original_url))
    return true;

  LoadUrl(stream_url);
  url_ = original_url;
  return engine_->New(original_url, headers);
}
