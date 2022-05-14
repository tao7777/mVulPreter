  base::string16 GetCertificateButtonTitle() const {
    PageInfoBubbleView* page_info_bubble_view =
        static_cast<PageInfoBubbleView*>(
            PageInfoBubbleView::GetPageInfoBubble());
    return page_info_bubble_view->certificate_button_->title()->text();
  }
