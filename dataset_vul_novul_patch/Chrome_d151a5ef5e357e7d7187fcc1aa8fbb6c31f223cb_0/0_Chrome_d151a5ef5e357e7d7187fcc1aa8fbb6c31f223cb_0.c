Section::Section(const string16& title,
                 bool state,
                 const string16& head_line,
                 const string16& description)
    : title_(title),
      state_(state),
      head_line_(head_line),
      description_(description) {
  if (!good_state_icon_) {
    ResourceBundle& rb = ResourceBundle::GetSharedInstance();
    good_state_icon_ = rb.GetBitmapNamed(IDR_PAGEINFO_GOOD);
    bad_state_icon_ = rb.GetBitmapNamed(IDR_PAGEINFO_BAD);
  }
  title_label_ = new views::Label(UTF16ToWideHack(title));
  title_label_->SetHorizontalAlignment(views::Label::ALIGN_LEFT);
  AddChildView(title_label_);

#if defined(OS_WIN)
  separator_ = new views::Separator();
  AddChildView(separator_);
#else
  NOTIMPLEMENTED();
#endif

  status_image_ = new views::ImageView();
  status_image_->SetImage(state ? good_state_icon_ : bad_state_icon_);
  AddChildView(status_image_);

  head_line_label_ = new views::Label(UTF16ToWideHack(head_line));
  head_line_label_->SetFont(
      head_line_label_->font().DeriveFont(0, gfx::Font::BOLD));
  head_line_label_->SetHorizontalAlignment(views::Label::ALIGN_LEFT);
  AddChildView(head_line_label_);

   description_label_ = new views::Label(UTF16ToWideHack(description));
   description_label_->SetMultiLine(true);
   description_label_->SetHorizontalAlignment(views::Label::ALIGN_LEFT);
  // Allow linebreaking in the middle of words if necessary, so that extremely
  // long hostnames (longer than one line) will still be completely shown.
  description_label_->SetAllowCharacterBreak(true);
   AddChildView(description_label_);
 }
