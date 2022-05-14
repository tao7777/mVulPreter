BookmarkManagerView::BookmarkManagerView(Profile* profile)
    : profile_(profile->GetOriginalProfile()),
      table_view_(NULL),
      tree_view_(NULL),
      ALLOW_THIS_IN_INITIALIZER_LIST(search_factory_(this)) {
  search_tf_ = new views::TextField();
  search_tf_->set_default_width_in_chars(30);

  table_view_ = new BookmarkTableView(profile_, NULL);
  table_view_->SetObserver(this);
  table_view_->SetContextMenuController(this);

  tree_view_ = new BookmarkFolderTreeView(profile_, NULL);
  tree_view_->SetController(this);
  tree_view_->SetContextMenuController(this);

  views::MenuButton* organize_menu_button = new views::MenuButton(
      NULL, l10n_util::GetString(IDS_BOOKMARK_MANAGER_ORGANIZE_MENU),
      this, true);
  organize_menu_button->SetID(kOrganizeMenuButtonID);

  views::MenuButton* tools_menu_button = new views::MenuButton(
      NULL, l10n_util::GetString(IDS_BOOKMARK_MANAGER_TOOLS_MENU),
      this, true);
  tools_menu_button->SetID(kToolsMenuButtonID);

  split_view_ = new views::SingleSplitView(tree_view_, table_view_);
  split_view_->set_background(
      views::Background::CreateSolidBackground(kBackgroundColorBottom));

  views::GridLayout* layout = new views::GridLayout(this);
  SetLayoutManager(layout);
  const int top_id = 1;
  const int split_cs_id = 2;
  layout->SetInsets(2, 0, 0, 0); // 2px padding above content.
  views::ColumnSet* column_set = layout->AddColumnSet(top_id);
  column_set->AddColumn(views::GridLayout::LEADING, views::GridLayout::CENTER,
                        0, views::GridLayout::USE_PREF, 0, 0);
  column_set->AddColumn(views::GridLayout::LEADING, views::GridLayout::CENTER,
                        0, views::GridLayout::USE_PREF, 0, 0);
  column_set->AddPaddingColumn(1, kUnrelatedControlHorizontalSpacing);
  column_set->AddColumn(views::GridLayout::LEADING, views::GridLayout::CENTER,
                        0, views::GridLayout::USE_PREF, 0, 0);
  column_set->AddPaddingColumn(0, kRelatedControlHorizontalSpacing);
  column_set->AddColumn(views::GridLayout::TRAILING, views::GridLayout::CENTER,
                        0, views::GridLayout::USE_PREF, 0, 0);
  column_set->AddPaddingColumn(0, 3); // 3px padding at end of row.

  column_set = layout->AddColumnSet(split_cs_id);
  column_set->AddColumn(views::GridLayout::FILL, views::GridLayout::FILL, 1,
                        views::GridLayout::USE_PREF, 0, 0);

  layout->StartRow(0, top_id);
  layout->AddView(organize_menu_button);
  layout->AddView(tools_menu_button);
  layout->AddView(new views::Label(
      l10n_util::GetString(IDS_BOOKMARK_MANAGER_SEARCH_TITLE)));
  layout->AddView(search_tf_);

  layout->AddPaddingRow(0, 3); // 3px padding between rows.

   layout->StartRow(1, split_cs_id);
   layout->AddView(split_view_);
 
  // Press Ctrl-W to close bookmark manager window.
  AddAccelerator(views::Accelerator('W', false, true, false));

   BookmarkModel* bookmark_model = profile_->GetBookmarkModel();
   if (!bookmark_model->IsLoaded())
     bookmark_model->AddObserver(this);
}
