void AutoFillManager::LogMetricsAboutSubmittedForm(
    const FormData& form,
    const FormStructure* submitted_form) {
  FormStructure* cached_submitted_form;
  if (!FindCachedForm(form, &cached_submitted_form)) {
    NOTREACHED();
    return;
  }

  std::map<std::string, const AutoFillField*> cached_fields;
  for (size_t i = 0; i < cached_submitted_form->field_count(); ++i) {
    const AutoFillField* field = cached_submitted_form->field(i);
     cached_fields[field->FieldSignature()] = field;
   }
 
   for (size_t i = 0; i < submitted_form->field_count(); ++i) {
     const AutoFillField* field = submitted_form->field(i);
     FieldTypeSet field_types;
    personal_data_->GetPossibleFieldTypes(field->value(), &field_types);
    DCHECK(!field_types.empty());

    if (field->form_control_type() == ASCIIToUTF16("select-one")) {
      continue;
     }
 
    metric_logger_->Log(AutoFillMetrics::FIELD_SUBMITTED);
     if (field_types.find(EMPTY_TYPE) == field_types.end() &&
         field_types.find(UNKNOWN_TYPE) == field_types.end()) {
       if (field->is_autofilled()) {
        metric_logger_->Log(AutoFillMetrics::FIELD_AUTOFILLED);
       } else {
        metric_logger_->Log(AutoFillMetrics::FIELD_AUTOFILL_FAILED);
 
         AutoFillFieldType heuristic_type = UNKNOWN_TYPE;
         AutoFillFieldType server_type = NO_SERVER_DATA;
        std::map<std::string, const AutoFillField*>::const_iterator
            cached_field = cached_fields.find(field->FieldSignature());
        if (cached_field != cached_fields.end()) {
          heuristic_type = cached_field->second->heuristic_type();
           server_type = cached_field->second->server_type();
         }
 
        if (heuristic_type == UNKNOWN_TYPE)
          metric_logger_->Log(AutoFillMetrics::FIELD_HEURISTIC_TYPE_UNKNOWN);
        else if (field_types.count(heuristic_type))
          metric_logger_->Log(AutoFillMetrics::FIELD_HEURISTIC_TYPE_MATCH);
        else
          metric_logger_->Log(AutoFillMetrics::FIELD_HEURISTIC_TYPE_MISMATCH);
        if (server_type == NO_SERVER_DATA)
          metric_logger_->Log(AutoFillMetrics::FIELD_SERVER_TYPE_UNKNOWN);
        else if (field_types.count(server_type))
          metric_logger_->Log(AutoFillMetrics::FIELD_SERVER_TYPE_MATCH);
        else
          metric_logger_->Log(AutoFillMetrics::FIELD_SERVER_TYPE_MISMATCH);
       }
 
    }
  }
}
