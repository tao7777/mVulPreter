 void AutofillManager::OnLoadedAutofillHeuristics(
     const std::string& heuristic_xml) {
   FormStructure::ParseQueryResponse(heuristic_xml,
                                     form_structures_.get(),
                                     *metric_logger_);
 }
