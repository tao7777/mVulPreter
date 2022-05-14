void FaviconWebUIHandler::OnFaviconDataAvailable(
    FaviconService::Handle request_handle,
    history::FaviconData favicon) {
   FaviconService* favicon_service =
       web_ui_->GetProfile()->GetFaviconService(Profile::EXPLICIT_ACCESS);
   int id = consumer_.GetClientData(favicon_service, request_handle);
  FundamentalValue id_value(id);
  scoped_ptr<StringValue> color_value;
 
   if (favicon.is_valid()) {
     color_utils::GridSampler sampler;
     SkColor color =
         color_utils::CalculateKMeanColorOfPNG(favicon.image_data, 100, 665,
                                              sampler);
    std::string css_color = base::StringPrintf("rgb(%d, %d, %d)",
                                                SkColorGetR(color),
                                                SkColorGetG(color),
                                                SkColorGetB(color));
    color_value.reset(new StringValue(css_color));
  } else {
    color_value.reset(new StringValue("#919191"));
   }

  web_ui_->CallJavascriptFunction("ntp4.setFaviconDominantColor",
                                  id_value, *color_value);
 }
