SendTabToSelfInfoBarDelegate::Create(const SendTabToSelfEntry* entry) {
SendTabToSelfInfoBarDelegate::Create(content::WebContents* web_contents,
                                     const SendTabToSelfEntry* entry) {
  return base::WrapUnique(
      new SendTabToSelfInfoBarDelegate(web_contents, entry));
 }
