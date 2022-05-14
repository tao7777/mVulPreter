 bool TranslateInfoBarDelegate::IsTranslatableLanguageByPrefs() {
  Profile* profile =
      Profile::FromBrowserContext(GetWebContents()->GetBrowserContext());
  Profile* original_profile = profile->GetOriginalProfile();
   scoped_ptr<TranslatePrefs> translate_prefs(
      TranslateTabHelper::CreateTranslatePrefs(original_profile->GetPrefs()));
   TranslateAcceptLanguages* accept_languages =
      TranslateTabHelper::GetTranslateAcceptLanguages(original_profile);
   return translate_prefs->CanTranslateLanguage(accept_languages,
                                                original_language_code());
 }
