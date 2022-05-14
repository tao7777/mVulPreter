 void ExtensionInstallUI::Prompt::SetInlineInstallWebstoreData(
    std::string localized_user_count,
     double average_rating,
     int rating_count) {
   CHECK_EQ(INLINE_INSTALL_PROMPT, type_);
  localized_user_count_ = localized_user_count;
  average_rating_ = average_rating;
  rating_count_ = rating_count;
}
