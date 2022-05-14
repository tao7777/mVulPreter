void ExtensionInstallDialogView::InitView() {
  int left_column_width =
      (prompt_->ShouldShowPermissions() || prompt_->GetRetainedFileCount() > 0)
          ? kPermissionsLeftColumnWidth
          : kNoPermissionsLeftColumnWidth;
  if (is_external_install())
    left_column_width = kExternalInstallLeftColumnWidth;

  int column_set_id = 0;
  views::GridLayout* layout = CreateLayout(left_column_width, column_set_id);

  ui::ResourceBundle& rb = ui::ResourceBundle::GetSharedInstance();

  if (prompt_->has_webstore_data()) {
    layout->StartRow(0, column_set_id);
    views::View* rating = new views::View();
    rating->SetLayoutManager(new views::BoxLayout(
        views::BoxLayout::kHorizontal, 0, 0, 0));
    layout->AddView(rating);
    prompt_->AppendRatingStars(AddResourceIcon, rating);

    const gfx::FontList& small_font_list =
        rb.GetFontList(ui::ResourceBundle::SmallFont);
    views::Label* rating_count =
        new views::Label(prompt_->GetRatingCount(), small_font_list);
    rating_count->SetBorder(views::Border::CreateEmptyBorder(0, 2, 0, 0));
    rating->AddChildView(rating_count);

    layout->StartRow(0, column_set_id);
    views::Label* user_count =
        new views::Label(prompt_->GetUserCount(), small_font_list);
    user_count->SetAutoColorReadabilityEnabled(false);
    user_count->SetEnabledColor(SK_ColorGRAY);
    layout->AddView(user_count);

    layout->StartRow(0, column_set_id);
    views::Link* store_link = new views::Link(
        l10n_util::GetStringUTF16(IDS_EXTENSION_PROMPT_STORE_LINK));
    store_link->SetFontList(small_font_list);
    store_link->set_listener(this);
    layout->AddView(store_link);

    if (prompt_->ShouldShowPermissions()) {
      layout->AddPaddingRow(0, views::kRelatedControlVerticalSpacing);
      layout->StartRow(0, column_set_id);
      layout->AddView(new views::Separator(views::Separator::HORIZONTAL),
                      3,
                      1,
                      views::GridLayout::FILL,
                      views::GridLayout::FILL);
    }
  }

  int content_width = left_column_width + views::kPanelHorizMargin + kIconSize;

  CustomScrollableView* scrollable = new CustomScrollableView();
  views::GridLayout* scroll_layout = new views::GridLayout(scrollable);
  scrollable->SetLayoutManager(scroll_layout);

  views::ColumnSet* scrollable_column_set =
      scroll_layout->AddColumnSet(column_set_id);
  int scrollable_width = prompt_->has_webstore_data() ? content_width
                                                      : left_column_width;
  scrollable_column_set->AddColumn(views::GridLayout::LEADING,
                                   views::GridLayout::LEADING,
                                   0,  // no resizing
                                   views::GridLayout::USE_PREF,
                                   scrollable_width,
                                   scrollable_width);
  int padding_width =
      content_width + views::kButtonHEdgeMarginNew - scrollable_width;
  scrollable_column_set->AddPaddingColumn(0, padding_width);

  layout->StartRow(0, column_set_id);
  scroll_view_ = new views::ScrollView();
  scroll_view_->set_hide_horizontal_scrollbar(true);
  scroll_view_->SetContents(scrollable);
  layout->AddView(scroll_view_, 4, 1);

  if (is_bundle_install()) {
    BundleInstaller::ItemList items = prompt_->bundle()->GetItemsWithState(
        BundleInstaller::Item::STATE_PENDING);
    scroll_layout->AddPaddingRow(0, views::kRelatedControlSmallVerticalSpacing);
    for (const BundleInstaller::Item& item : items) {
      scroll_layout->StartRow(0, column_set_id);
      views::Label* extension_label =
          new views::Label(item.GetNameForDisplay());
      extension_label->SetMultiLine(true);
      extension_label->SetHorizontalAlignment(gfx::ALIGN_LEFT);
      extension_label->SizeToFit(
          scrollable_width - kSmallIconSize - kSmallIconPadding);
      gfx::ImageSkia image = gfx::ImageSkia::CreateFrom1xBitmap(item.icon);
      scroll_layout->AddView(new IconedView(extension_label, image));
    }
    scroll_layout->AddPaddingRow(0, views::kRelatedControlVerticalSpacing);
  }

  if (prompt_->ShouldShowPermissions()) {
    bool has_permissions =
        prompt_->GetPermissionCount(
            ExtensionInstallPrompt::PermissionsType::ALL_PERMISSIONS) > 0;
    if (has_permissions) {
      AddPermissions(
          scroll_layout,
          rb,
          column_set_id,
          scrollable_width,
          ExtensionInstallPrompt::PermissionsType::REGULAR_PERMISSIONS);
      AddPermissions(
          scroll_layout,
          rb,
          column_set_id,
          scrollable_width,
          ExtensionInstallPrompt::PermissionsType::WITHHELD_PERMISSIONS);
    } else {
      scroll_layout->AddPaddingRow(0, views::kRelatedControlVerticalSpacing);
      scroll_layout->StartRow(0, column_set_id);
      views::Label* permission_label = new views::Label(
          l10n_util::GetStringUTF16(IDS_EXTENSION_NO_SPECIAL_PERMISSIONS));
      permission_label->SetMultiLine(true);
      permission_label->SetHorizontalAlignment(gfx::ALIGN_LEFT);
      permission_label->SizeToFit(scrollable_width);
      scroll_layout->AddView(permission_label);
    }
  }

  if (prompt_->GetRetainedFileCount()) {
    scroll_layout->AddPaddingRow(0, views::kRelatedControlVerticalSpacing);

    scroll_layout->StartRow(0, column_set_id);
    views::Label* retained_files_header =
        new views::Label(prompt_->GetRetainedFilesHeading());
    retained_files_header->SetMultiLine(true);
    retained_files_header->SetHorizontalAlignment(gfx::ALIGN_LEFT);
    retained_files_header->SizeToFit(scrollable_width);
    scroll_layout->AddView(retained_files_header);

    scroll_layout->StartRow(0, column_set_id);
    PermissionDetails details;
    for (size_t i = 0; i < prompt_->GetRetainedFileCount(); ++i) {
      details.push_back(prompt_->GetRetainedFile(i));
    }
    ExpandableContainerView* issue_advice_view =
        new ExpandableContainerView(this,
                                    base::string16(),
                                    details,
                                    scrollable_width,
                                    false);
    scroll_layout->AddView(issue_advice_view);
  }

  if (prompt_->GetRetainedDeviceCount()) {
    scroll_layout->AddPaddingRow(0, views::kRelatedControlVerticalSpacing);

    scroll_layout->StartRow(0, column_set_id);
    views::Label* retained_devices_header =
        new views::Label(prompt_->GetRetainedDevicesHeading());
    retained_devices_header->SetMultiLine(true);
    retained_devices_header->SetHorizontalAlignment(gfx::ALIGN_LEFT);
    retained_devices_header->SizeToFit(scrollable_width);
    scroll_layout->AddView(retained_devices_header);

    scroll_layout->StartRow(0, column_set_id);
    PermissionDetails details;
    for (size_t i = 0; i < prompt_->GetRetainedDeviceCount(); ++i) {
      details.push_back(prompt_->GetRetainedDeviceMessageString(i));
    }
    ExpandableContainerView* issue_advice_view =
        new ExpandableContainerView(this,
                                    base::string16(),
                                    details,
                                    scrollable_width,
                                    false);
     scroll_layout->AddView(issue_advice_view);
   }
 
  DCHECK_GE(prompt_->type(), 0);
   UMA_HISTOGRAM_ENUMERATION("Extensions.InstallPrompt.Type",
                             prompt_->type(),
                             ExtensionInstallPrompt::NUM_PROMPT_TYPES);

  scroll_view_->ClipHeightTo(
      0,
      std::min(kScrollViewMaxHeight, scrollable->GetPreferredSize().height()));

  dialog_size_ = gfx::Size(
      content_width + 2 * views::kButtonHEdgeMarginNew,
      container_->GetPreferredSize().height());

  std::string event_name = ExperienceSamplingEvent::kExtensionInstallDialog;
  event_name.append(
      ExtensionInstallPrompt::PromptTypeToString(prompt_->type()));
  sampling_event_ = ExperienceSamplingEvent::Create(event_name);
}
