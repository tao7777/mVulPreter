  InstalledBubbleContent(Browser* browser,
                         const Extension* extension,
                         ExtensionInstalledBubble::BubbleType type,
                         SkBitmap* icon,
                         ExtensionInstalledBubble* bubble)
      : browser_(browser),
        extension_id_(extension->id()),
        bubble_(bubble),
        type_(type),
        info_(NULL) {
    ResourceBundle& rb = ResourceBundle::GetSharedInstance();
    const gfx::Font& font = rb.GetFont(ResourceBundle::BaseFont);

    gfx::Size size(icon->width(), icon->height());
    if (size.width() > kIconSize || size.height() > kIconSize)
      size = gfx::Size(kIconSize, kIconSize);
    icon_ = new views::ImageView();
    icon_->SetImageSize(size);
    icon_->SetImage(*icon);
    AddChildView(icon_);

     string16 extension_name = UTF8ToUTF16(extension->name());
     base::i18n::AdjustStringForLocaleDirection(&extension_name);
     heading_ = new views::Label(l10n_util::GetStringFUTF16(
        IDS_EXTENSION_INSTALLED_HEADING,
        extension_name,
        l10n_util::GetStringUTF16(IDS_SHORT_PRODUCT_NAME)));
     heading_->SetFont(rb.GetFont(ResourceBundle::MediumFont));
     heading_->SetMultiLine(true);
     heading_->SetHorizontalAlignment(views::Label::ALIGN_LEFT);
    AddChildView(heading_);

    switch (type_) {
      case ExtensionInstalledBubble::PAGE_ACTION: {
        info_ = new views::Label(l10n_util::GetStringUTF16(
            IDS_EXTENSION_INSTALLED_PAGE_ACTION_INFO));
        info_->SetFont(font);
        info_->SetMultiLine(true);
        info_->SetHorizontalAlignment(views::Label::ALIGN_LEFT);
        AddChildView(info_);
        break;
      }
      case ExtensionInstalledBubble::OMNIBOX_KEYWORD: {
        info_ = new views::Label(l10n_util::GetStringFUTF16(
            IDS_EXTENSION_INSTALLED_OMNIBOX_KEYWORD_INFO,
            UTF8ToUTF16(extension->omnibox_keyword())));
        info_->SetFont(font);
        info_->SetMultiLine(true);
        info_->SetHorizontalAlignment(views::Label::ALIGN_LEFT);
        AddChildView(info_);
        break;
      }
      case ExtensionInstalledBubble::APP: {
        views::Link* link = new views::Link(
            l10n_util::GetStringUTF16(IDS_EXTENSION_INSTALLED_APP_INFO));
        link->set_listener(this);
        manage_ = link;
        manage_->SetFont(font);
        manage_->SetMultiLine(true);
        manage_->SetHorizontalAlignment(views::Label::ALIGN_LEFT);
        AddChildView(manage_);
        break;
      }
      default:
        break;
    }

    if (type_ != ExtensionInstalledBubble::APP) {
      manage_ = new views::Label(
          l10n_util::GetStringUTF16(IDS_EXTENSION_INSTALLED_MANAGE_INFO));
      manage_->SetFont(font);
      manage_->SetMultiLine(true);
      manage_->SetHorizontalAlignment(views::Label::ALIGN_LEFT);
      AddChildView(manage_);
    }

    close_button_ = new views::ImageButton(this);
    close_button_->SetImage(views::CustomButton::BS_NORMAL,
        rb.GetBitmapNamed(IDR_CLOSE_BAR));
    close_button_->SetImage(views::CustomButton::BS_HOT,
        rb.GetBitmapNamed(IDR_CLOSE_BAR_H));
    close_button_->SetImage(views::CustomButton::BS_PUSHED,
        rb.GetBitmapNamed(IDR_CLOSE_BAR_P));
    AddChildView(close_button_);
  }
