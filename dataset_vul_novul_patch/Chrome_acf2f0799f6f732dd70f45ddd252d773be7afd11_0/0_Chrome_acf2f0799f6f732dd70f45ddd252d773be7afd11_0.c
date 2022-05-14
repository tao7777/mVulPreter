InternalPageInfoBubbleView::InternalPageInfoBubbleView(
    views::View* anchor_view,
    const gfx::Rect& anchor_rect,
    gfx::NativeView parent_window,
    const GURL& url)
    : BubbleDialogDelegateView(anchor_view, views::BubbleBorder::TOP_LEFT) {
  g_shown_bubble_type = PageInfoBubbleView::BUBBLE_INTERNAL_PAGE;
  g_page_info_bubble = this;
  set_parent_window(parent_window);
  if (!anchor_view)
    SetAnchorRect(anchor_rect);

  int text = IDS_PAGE_INFO_INTERNAL_PAGE;
  int icon = IDR_PRODUCT_LOGO_16;
  if (url.SchemeIs(extensions::kExtensionScheme)) {
    text = IDS_PAGE_INFO_EXTENSION_PAGE;
    icon = IDR_PLUGINS_FAVICON;
  } else if (url.SchemeIs(content::kViewSourceScheme)) {
    text = IDS_PAGE_INFO_VIEW_SOURCE_PAGE;
    icon = IDR_PRODUCT_LOGO_16;
  } else if (!url.SchemeIs(content::kChromeUIScheme) &&
             !url.SchemeIs(content::kChromeDevToolsScheme)) {
    NOTREACHED();
  }

   set_anchor_view_insets(gfx::Insets(
       GetLayoutConstant(LOCATION_BAR_BUBBLE_ANCHOR_VERTICAL_INSET), 0));
 
  // Title insets assume there is content (and thus have no bottom padding). Use
  // dialog insets to get the bottom margin back.
  set_title_margins(
      ChromeLayoutProvider::Get()->GetInsetsMetric(views::INSETS_DIALOG));
   set_margins(gfx::Insets());
 
  ui::ResourceBundle& rb = ui::ResourceBundle::GetSharedInstance();
  bubble_icon_ = rb.GetImageSkiaNamed(icon);
  title_text_ = l10n_util::GetStringUTF16(text);
 
   views::BubbleDialogDelegateView::CreateBubble(this);

  // Use a normal label's style for the title since there is no content.
  views::Label* title_label =
      static_cast<views::Label*>(GetBubbleFrameView()->title());
  title_label->SetFontList(views::Label::GetDefaultFontList());
  title_label->SetMultiLine(false);
  title_label->SetElideBehavior(gfx::NO_ELIDE);

  SizeToContents();
 }
