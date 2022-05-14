 void AutoFillQueryXmlParser::StartElement(buzz::XmlParseContext* context,
                                           const char* name,
                                           const char** attrs) {
   buzz::QName qname = context->ResolveQName(name, false);
  const std::string& element = qname.LocalPart();
   if (element.compare("autofillqueryresponse") == 0) {
    // We check for the upload required attribute below, but if it's not
    // present, we use the default upload rates. Likewise, by default we assume
    // an empty experiment id.
     *upload_required_ = USE_UPLOAD_RATES;
    *experiment_id_ = std::string();

    // |attrs| is a NULL-terminated list of (attribute, value) pairs.
    while (*attrs) {
       buzz::QName attribute_qname = context->ResolveQName(attrs[0], true);
      const std::string& attribute_name = attribute_qname.LocalPart();
       if (attribute_name.compare("uploadrequired") == 0) {
         if (strcmp(attrs[1], "true") == 0)
           *upload_required_ = UPLOAD_REQUIRED;
         else if (strcmp(attrs[1], "false") == 0)
           *upload_required_ = UPLOAD_NOT_REQUIRED;
      } else if (attribute_name.compare("experimentid") == 0) {
        *experiment_id_ = attrs[1];
       }

      // Advance to the next (attribute, value) pair.
      attrs += 2;
     }
   } else if (element.compare("field") == 0) {
     if (!attrs[0]) {
      context->RaiseError(XML_ERROR_ABORTED);
      return;
    }

     AutoFillFieldType field_type = UNKNOWN_TYPE;
     buzz::QName attribute_qname = context->ResolveQName(attrs[0], true);
    const std::string& attribute_name = attribute_qname.LocalPart();
 
     if (attribute_name.compare("autofilltype") == 0) {
       int value = GetIntValue(context, attrs[1]);
      field_type = static_cast<AutoFillFieldType>(value);
      if (field_type < 0 || field_type > MAX_VALID_FIELD_TYPE) {
        field_type = NO_SERVER_DATA;
      }
    }

    field_types_->push_back(field_type);
  }
}
