void PageHandler::PrintToPDF(Maybe<bool> landscape,
                             Maybe<bool> display_header_footer,
                             Maybe<bool> print_background,
                             Maybe<double> scale,
                             Maybe<double> paper_width,
                             Maybe<double> paper_height,
                             Maybe<double> margin_top,
                             Maybe<double> margin_bottom,
                             Maybe<double> margin_left,
                              Maybe<double> margin_right,
                              Maybe<String> page_ranges,
                              Maybe<bool> ignore_invalid_page_ranges,
                             Maybe<String> header_template,
                             Maybe<String> footer_template,
                              std::unique_ptr<PrintToPDFCallback> callback) {
   callback->sendFailure(Response::Error("PrintToPDF is not implemented"));
   return;
}
