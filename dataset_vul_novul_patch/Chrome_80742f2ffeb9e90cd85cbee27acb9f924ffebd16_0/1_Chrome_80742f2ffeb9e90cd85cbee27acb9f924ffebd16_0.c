 void AutofillManager::OnLoadedAutofillHeuristics(
     const std::string& heuristic_xml) {
  UploadRequired upload_required;
   FormStructure::ParseQueryResponse(heuristic_xml,
                                     form_structures_.get(),
                                    &upload_required,
                                     *metric_logger_);
 }
