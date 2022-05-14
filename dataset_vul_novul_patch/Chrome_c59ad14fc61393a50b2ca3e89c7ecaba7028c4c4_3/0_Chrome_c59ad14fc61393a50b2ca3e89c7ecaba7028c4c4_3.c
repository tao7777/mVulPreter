std::unique_ptr<base::DictionaryValue> ParsePrintSettings(
    int command_id,
    const base::DictionaryValue* params,
    HeadlessPrintSettings* settings) {
  if (const base::Value* landscape_value = params->FindKey("landscape"))
    settings->landscape = landscape_value->GetBool();

  if (const base::Value* display_header_footer_value =
          params->FindKey("displayHeaderFooter")) {
    settings->display_header_footer = display_header_footer_value->GetBool();
  }

  if (const base::Value* should_print_backgrounds_value =
          params->FindKey("printBackground")) {
    settings->should_print_backgrounds =
        should_print_backgrounds_value->GetBool();
  }
  if (const base::Value* scale_value = params->FindKey("scale"))
    settings->scale = scale_value->GetDouble();
  if (settings->scale > kScaleMaxVal / 100 ||
      settings->scale < kScaleMinVal / 100)
    return CreateInvalidParamResponse(command_id, "scale");
  if (const base::Value* page_ranges_value = params->FindKey("pageRanges"))
    settings->page_ranges = page_ranges_value->GetString();

  if (const base::Value* ignore_invalid_page_ranges_value =
          params->FindKey("ignoreInvalidPageRanges")) {
    settings->ignore_invalid_page_ranges =
        ignore_invalid_page_ranges_value->GetBool();
  }

  double paper_width_in_inch = printing::kLetterWidthInch;

  if (const base::Value* paper_width_value = params->FindKey("paperWidth"))
    paper_width_in_inch = paper_width_value->GetDouble();

  double paper_height_in_inch = printing::kLetterHeightInch;

  if (const base::Value* paper_height_value = params->FindKey("paperHeight"))
    paper_height_in_inch = paper_height_value->GetDouble();
  if (paper_width_in_inch <= 0)
    return CreateInvalidParamResponse(command_id, "paperWidth");
  if (paper_height_in_inch <= 0)
    return CreateInvalidParamResponse(command_id, "paperHeight");
  settings->paper_size_in_points =
      gfx::Size(paper_width_in_inch * printing::kPointsPerInch,
                paper_height_in_inch * printing::kPointsPerInch);

  double default_margin_in_inch = 1000.0 / printing::kHundrethsMMPerInch;
  double margin_top_in_inch = default_margin_in_inch;
  double margin_bottom_in_inch = default_margin_in_inch;
  double margin_left_in_inch = default_margin_in_inch;
  double margin_right_in_inch = default_margin_in_inch;

  if (const base::Value* margin_top_value = params->FindKey("marginTop"))
    margin_top_in_inch = margin_top_value->GetDouble();

  if (const base::Value* margin_bottom_value = params->FindKey("marginBottom"))
    margin_bottom_in_inch = margin_bottom_value->GetDouble();

  if (const base::Value* margin_left_value = params->FindKey("marginLeft"))
    margin_left_in_inch = margin_left_value->GetDouble();

   if (const base::Value* margin_right_value = params->FindKey("marginRight"))
     margin_right_in_inch = margin_right_value->GetDouble();
 
  if (const base::Value* header_template_value =
          params->FindKey("headerTemplate")) {
    settings->header_template = header_template_value->GetString();
  }
  if (const base::Value* footer_template_value =
          params->FindKey("footerTemplate")) {
    settings->footer_template = footer_template_value->GetString();
  }

   if (margin_top_in_inch < 0)
     return CreateInvalidParamResponse(command_id, "marginTop");
   if (margin_bottom_in_inch < 0)
    return CreateInvalidParamResponse(command_id, "marginBottom");
  if (margin_left_in_inch < 0)
    return CreateInvalidParamResponse(command_id, "marginLeft");
  if (margin_right_in_inch < 0)
    return CreateInvalidParamResponse(command_id, "marginRight");
  settings->margins_in_points.top =
      margin_top_in_inch * printing::kPointsPerInch;
  settings->margins_in_points.bottom =
      margin_bottom_in_inch * printing::kPointsPerInch;
  settings->margins_in_points.left =
      margin_left_in_inch * printing::kPointsPerInch;
  settings->margins_in_points.right =
      margin_right_in_inch * printing::kPointsPerInch;

  return nullptr;
}
