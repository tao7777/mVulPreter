 TranslateInfoBarDelegate* TranslateManager::GetTranslateInfoBarDelegate(
     TabContents* tab) {
   for (int i = 0; i < tab->infobar_delegate_count(); ++i) {
    TranslateInfoBarDelegate* delegate =
        tab->GetInfoBarDelegateAt(i)->AsTranslateInfoBarDelegate();
    if (delegate)
      return delegate;
   }
   return NULL;
 }
