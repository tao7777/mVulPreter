 PasswordGenerationDialogViewInterface::Create(
    PasswordGenerationController* controller) {
   return std::make_unique<PasswordGenerationDialogViewAndroid>(controller);
 }
