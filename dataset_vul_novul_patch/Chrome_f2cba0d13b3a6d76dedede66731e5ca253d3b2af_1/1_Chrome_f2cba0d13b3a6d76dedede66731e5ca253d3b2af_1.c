WebsiteSettingsPopupView::WebsiteSettingsPopupView(
    views::View* anchor_view,
    gfx::NativeView parent_window,
    Profile* profile,
     content::WebContents* web_contents,
     const GURL& url,
     const content::SSLStatus& ssl)
    : BubbleDelegateView(anchor_view, views::BubbleBorder::TOP_LEFT),
       web_contents_(web_contents),
       header_(nullptr),
       tabbed_pane_(nullptr),
      permissions_tab_(nullptr),
      site_data_content_(nullptr),
      cookie_dialog_link_(nullptr),
      permissions_content_(nullptr),
      connection_tab_(nullptr),
      identity_info_content_(nullptr),
      certificate_dialog_link_(nullptr),
      reset_decisions_button_(nullptr),
      help_center_content_(nullptr),
      cert_id_(0),
      help_center_link_(nullptr),
      connection_info_content_(nullptr),
      weak_factory_(this) {
  set_parent_window(parent_window);

  set_anchor_view_insets(gfx::Insets(kLocationIconVerticalMargin, 0,
                                     kLocationIconVerticalMargin, 0));

  views::GridLayout* layout = new views::GridLayout(this);
  SetLayoutManager(layout);
  const int content_column = 0;
  views::ColumnSet* column_set = layout->AddColumnSet(content_column);
  column_set->AddColumn(views::GridLayout::FILL,
                        views::GridLayout::FILL,
                        1,
                        views::GridLayout::USE_PREF,
                        0,
                        0);

  header_ = new PopupHeaderView(this);
  layout->StartRow(1, content_column);
  layout->AddView(header_);

  layout->AddPaddingRow(1, kHeaderMarginBottom);
  tabbed_pane_ = new views::TabbedPane();
  layout->StartRow(1, content_column);
  layout->AddView(tabbed_pane_);

  permissions_tab_ = CreatePermissionsTab();
  tabbed_pane_->AddTabAtIndex(
      TAB_ID_PERMISSIONS,
      l10n_util::GetStringUTF16(IDS_WEBSITE_SETTINGS_TAB_LABEL_PERMISSIONS),
      permissions_tab_);
  connection_tab_ = CreateConnectionTab();
  tabbed_pane_->AddTabAtIndex(
      TAB_ID_CONNECTION,
      l10n_util::GetStringUTF16(IDS_WEBSITE_SETTINGS_TAB_LABEL_CONNECTION),
      connection_tab_);
  DCHECK_EQ(tabbed_pane_->GetTabCount(), NUM_TAB_IDS);
  tabbed_pane_->set_listener(this);

  set_margins(gfx::Insets(kPopupMarginTop, kPopupMarginLeft,
                          kPopupMarginBottom, kPopupMarginRight));

   views::BubbleDelegateView::CreateBubble(this);
 
   presenter_.reset(new WebsiteSettings(
      this, profile,
      TabSpecificContentSettings::FromWebContents(web_contents),
      InfoBarService::FromWebContents(web_contents), url, ssl,
      content::CertStore::GetInstance()));
 }
