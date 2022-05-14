AppModalDialog::AppModalDialog(WebContents* web_contents, const string16& title)
     : valid_(true),
       native_dialog_(NULL),
       title_(title),
      web_contents_(web_contents) {
 }
