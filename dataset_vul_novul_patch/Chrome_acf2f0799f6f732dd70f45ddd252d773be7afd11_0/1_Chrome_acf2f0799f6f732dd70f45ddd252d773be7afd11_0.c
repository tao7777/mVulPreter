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
 
  SetLayoutManager(new views::BoxLayout(views::BoxLayout::kHorizontal,
                                        gfx::Insets(kSpacing), kSpacing));
   set_margins(gfx::Insets());
  if (ChromeLayoutProvider::Get()->ShouldShowWindowIcon()) {
    views::ImageView* icon_view = new NonAccessibleImageView();
    ui::ResourceBundle& rb = ui::ResourceBundle::GetSharedInstance();
    icon_view->SetImage(rb.GetImageSkiaNamed(icon));
    AddChildView(icon_view);
  }
 
  views::Label* label = new views::Label(l10n_util::GetStringUTF16(text));
  label->SetMultiLine(true);
  label->SetAllowCharacterBreak(true);
  label->SetHorizontalAlignment(gfx::ALIGN_LEFT);
  AddChildView(label);
 
   views::BubbleDialogDelegateView::CreateBubble(this);
 }
