 AutoFillQueryXmlParser::AutoFillQueryXmlParser(
     std::vector<AutoFillFieldType>* field_types,
    UploadRequired* upload_required,
    std::string* experiment_id)
     : field_types_(field_types),
      upload_required_(upload_required),
      experiment_id_(experiment_id) {
   DCHECK(upload_required_);
  DCHECK(experiment_id_);
 }
