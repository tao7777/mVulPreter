SendTabToSelfInfoBarDelegate::Create(const SendTabToSelfEntry* entry) {
  return base::WrapUnique(new SendTabToSelfInfoBarDelegate(entry));
 }
