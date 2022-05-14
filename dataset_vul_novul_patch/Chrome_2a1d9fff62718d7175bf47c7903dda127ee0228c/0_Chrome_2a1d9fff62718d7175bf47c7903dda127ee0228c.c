void SendTabToSelfInfoBar::ShowInfoBar(content::WebContents* web_contents,
void SendTabToSelfInfoBar::ShowInfoBar(
    content::WebContents* web_contents,
    std::unique_ptr<SendTabToSelfInfoBarDelegate> delegate) {
  InfoBarService* service = InfoBarService::FromWebContents(web_contents);
  service->AddInfoBar(
      base::WrapUnique(new SendTabToSelfInfoBar(std::move(delegate))));
 }
