AutoFillCCInfoBarDelegate::AutoFillCCInfoBarDelegate(TabContents* tab_contents,
                                                      AutoFillManager* host)
     : ConfirmInfoBarDelegate(tab_contents),
       host_(host) {
 }
