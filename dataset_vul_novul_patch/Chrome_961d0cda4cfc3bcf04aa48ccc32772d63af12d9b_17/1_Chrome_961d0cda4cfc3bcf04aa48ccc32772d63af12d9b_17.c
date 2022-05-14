void PasswordAccessoryControllerTest::InitializeGeneration(
    const base::string16& password) {
  ON_CALL(*(mock_password_manager_driver_.get()),
          GetPasswordGenerationManager())
      .WillByDefault(Return(mock_generation_manager_.get()));
  EXPECT_CALL(mock_manual_filling_controller_,
              OnAutomaticGenerationStatusChanged(true));
  controller()->OnAutomaticGenerationStatusChanged(
      true, GetTestGenerationUIData1(),
      mock_password_manager_driver_->AsWeakPtr());
  ON_CALL(*(mock_generation_manager_.get()), GeneratePassword(_, _, _, _, _))
      .WillByDefault(Return(password));
  ON_CALL(mock_dialog_factory(), Run)
      .WillByDefault(Return(ByMove(std::move(mock_dialog_))));
}
