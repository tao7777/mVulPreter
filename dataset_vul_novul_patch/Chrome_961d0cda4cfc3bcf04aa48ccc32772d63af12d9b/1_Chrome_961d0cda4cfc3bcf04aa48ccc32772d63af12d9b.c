 PasswordGenerationDialogViewInterface::Create(
    PasswordAccessoryController* controller) {
   return std::make_unique<PasswordGenerationDialogViewAndroid>(controller);
 }
