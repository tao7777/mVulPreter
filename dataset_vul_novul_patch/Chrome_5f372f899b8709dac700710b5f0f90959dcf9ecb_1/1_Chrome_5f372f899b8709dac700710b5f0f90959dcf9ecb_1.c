 AutoFillQueryXmlParser::AutoFillQueryXmlParser(
     std::vector<AutoFillFieldType>* field_types,
    UploadRequired* upload_required)
     : field_types_(field_types),
      upload_required_(upload_required) {
   DCHECK(upload_required_);
 }
