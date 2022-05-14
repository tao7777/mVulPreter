void FaviconWebUIHandler::OnFaviconDataAvailable(
    FaviconService::Handle request_handle,
    history::FaviconData favicon) {
   FaviconService* favicon_service =
       web_ui_->GetProfile()->GetFaviconService(Profile::EXPLICIT_ACCESS);
   int id = consumer_.GetClientData(favicon_service, request_handle);
 
   if (favicon.is_valid()) {
    FundamentalValue id_value(id);
     color_utils::GridSampler sampler;
     SkColor color =
         color_utils::CalculateKMeanColorOfPNG(favicon.image_data, 100, 665,
                                              sampler);
    std::string css_color = base::StringPrintf("rgb(%d, %d, %d)",
                                                SkColorGetR(color),
                                                SkColorGetG(color),
                                                SkColorGetB(color));
    StringValue color_value(css_color);
    web_ui_->CallJavascriptFunction("ntp4.setFaviconDominantColor",
                                    id_value, color_value);
   }
 }
