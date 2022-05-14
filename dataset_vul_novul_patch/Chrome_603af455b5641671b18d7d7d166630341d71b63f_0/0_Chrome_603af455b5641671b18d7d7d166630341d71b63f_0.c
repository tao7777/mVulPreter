 bool TranslateInfoBarDelegate::IsTranslatableLanguageByPrefs() {
  TranslateTabHelper* translate_tab_helper =
      TranslateTabHelper::FromWebContents(GetWebContents());
   scoped_ptr<TranslatePrefs> translate_prefs(
      TranslateTabHelper::CreateTranslatePrefs(
          translate_tab_helper->GetPrefs()));
   TranslateAcceptLanguages* accept_languages =
      translate_tab_helper->GetTranslateAcceptLanguages();
   return translate_prefs->CanTranslateLanguage(accept_languages,
                                                original_language_code());
 }
