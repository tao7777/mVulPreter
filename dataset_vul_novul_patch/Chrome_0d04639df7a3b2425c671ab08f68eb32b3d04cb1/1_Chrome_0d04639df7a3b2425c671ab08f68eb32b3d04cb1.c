AutoFillCCInfoBarDelegate::AutoFillCCInfoBarDelegate(TabContents* tab_contents,
                                                      AutoFillManager* host)
     : ConfirmInfoBarDelegate(tab_contents),
       host_(host) {
  if (tab_contents)
    tab_contents->AddInfoBar(this);
 }
