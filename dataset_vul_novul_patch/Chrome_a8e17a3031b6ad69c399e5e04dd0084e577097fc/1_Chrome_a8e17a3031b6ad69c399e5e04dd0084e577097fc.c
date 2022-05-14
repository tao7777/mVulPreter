 void HTMLFormControlElement::updateVisibleValidationMessage() {
   Page* page = document().page();
  if (!page)
     return;
   String message;
   if (layoutObject() && willValidate())
    message = validationMessage().stripWhiteSpace();

  m_hasValidationMessage = true;
  ValidationMessageClient* client = &page->validationMessageClient();
  TextDirection messageDir = LTR;
  TextDirection subMessageDir = LTR;
  String subMessage = validationSubMessage().stripWhiteSpace();
  if (message.isEmpty())
    client->hideValidationMessage(*this);
  else
    findCustomValidationMessageTextDirection(message, messageDir, subMessage,
                                             subMessageDir);
  client->showValidationMessage(*this, message, messageDir, subMessage,
                                subMessageDir);
}
