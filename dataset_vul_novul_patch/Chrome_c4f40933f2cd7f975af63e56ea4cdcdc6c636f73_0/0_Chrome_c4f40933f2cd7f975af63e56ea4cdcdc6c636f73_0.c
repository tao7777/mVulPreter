void TaskManagerView::Init() {
  table_model_.reset(new TaskManagerTableModel(model_));

  columns_.push_back(ui::TableColumn(IDS_TASK_MANAGER_TASK_COLUMN,
                                     ui::TableColumn::LEFT, -1, 1));
  columns_.back().sortable = true;
  columns_.push_back(ui::TableColumn(IDS_TASK_MANAGER_PROFILE_NAME_COLUMN,
                                     ui::TableColumn::LEFT, -1, 0));
  columns_.back().sortable = true;
  columns_.push_back(ui::TableColumn(IDS_TASK_MANAGER_PHYSICAL_MEM_COLUMN,
                                     ui::TableColumn::RIGHT, -1, 0));
  columns_.back().sortable = true;
  columns_.push_back(ui::TableColumn(IDS_TASK_MANAGER_SHARED_MEM_COLUMN,
                                     ui::TableColumn::RIGHT, -1, 0));
  columns_.back().sortable = true;
  columns_.push_back(ui::TableColumn(IDS_TASK_MANAGER_PRIVATE_MEM_COLUMN,
                                     ui::TableColumn::RIGHT, -1, 0));
  columns_.back().sortable = true;
  columns_.push_back(ui::TableColumn(IDS_TASK_MANAGER_CPU_COLUMN,
                                     ui::TableColumn::RIGHT, -1, 0));
  columns_.back().sortable = true;
  columns_.push_back(ui::TableColumn(IDS_TASK_MANAGER_NET_COLUMN,
                                     ui::TableColumn::RIGHT, -1, 0));
  columns_.back().sortable = true;
  columns_.push_back(ui::TableColumn(IDS_TASK_MANAGER_PROCESS_ID_COLUMN,
                                     ui::TableColumn::RIGHT, -1, 0));
  columns_.back().sortable = true;
  columns_.push_back(ui::TableColumn(
      IDS_TASK_MANAGER_WEBCORE_IMAGE_CACHE_COLUMN,
      ui::TableColumn::RIGHT, -1, 0));
  columns_.back().sortable = true;
  columns_.push_back(ui::TableColumn(
      IDS_TASK_MANAGER_WEBCORE_SCRIPTS_CACHE_COLUMN,
      ui::TableColumn::RIGHT, -1, 0));
  columns_.back().sortable = true;
  columns_.push_back(ui::TableColumn(IDS_TASK_MANAGER_WEBCORE_CSS_CACHE_COLUMN,
                                     ui::TableColumn::RIGHT, -1, 0));
  columns_.back().sortable = true;
  columns_.push_back(ui::TableColumn(IDS_TASK_MANAGER_FPS_COLUMN,
                                     ui::TableColumn::RIGHT, -1, 0));
  columns_.back().sortable = true;
  columns_.push_back(ui::TableColumn(IDS_TASK_MANAGER_SQLITE_MEMORY_USED_COLUMN,
                                     ui::TableColumn::RIGHT, -1, 0));
  columns_.back().sortable = true;
  columns_.push_back(
      ui::TableColumn(IDS_TASK_MANAGER_JAVASCRIPT_MEMORY_ALLOCATED_COLUMN,
                      ui::TableColumn::RIGHT, -1, 0));
  columns_.back().sortable = true;

  tab_table_ = new BackgroundColorGroupTableView(
      table_model_.get(), columns_, highlight_background_resources_);

  tab_table_->SetColumnVisibility(IDS_TASK_MANAGER_PROFILE_NAME_COLUMN, false);
  tab_table_->SetColumnVisibility(IDS_TASK_MANAGER_PROCESS_ID_COLUMN, false);
  tab_table_->SetColumnVisibility(IDS_TASK_MANAGER_SHARED_MEM_COLUMN, false);
  tab_table_->SetColumnVisibility(IDS_TASK_MANAGER_PRIVATE_MEM_COLUMN, false);
  tab_table_->SetColumnVisibility(IDS_TASK_MANAGER_WEBCORE_IMAGE_CACHE_COLUMN,
                                  false);
  tab_table_->SetColumnVisibility(IDS_TASK_MANAGER_WEBCORE_SCRIPTS_CACHE_COLUMN,
                                  false);
  tab_table_->SetColumnVisibility(IDS_TASK_MANAGER_WEBCORE_CSS_CACHE_COLUMN,
                                  false);
  tab_table_->SetColumnVisibility(IDS_TASK_MANAGER_SQLITE_MEMORY_USED_COLUMN,
                                  false);
  tab_table_->SetColumnVisibility(
      IDS_TASK_MANAGER_JAVASCRIPT_MEMORY_ALLOCATED_COLUMN, false);
  tab_table_->SetColumnVisibility(IDS_TASK_MANAGER_GOATS_TELEPORTED_COLUMN,
                                  false);

  UpdateStatsCounters();
  tab_table_->SetObserver(this);
  tab_table_->set_context_menu_controller(this);
  set_context_menu_controller(this);
  if (CommandLine::ForCurrentProcess()->HasSwitch(
      switches::kPurgeMemoryButton)) {
    purge_memory_button_ = new views::NativeTextButton(this,
        l10n_util::GetStringUTF16(IDS_TASK_MANAGER_PURGE_MEMORY));
   }
   kill_button_ = new views::NativeTextButton(
       this, l10n_util::GetStringUTF16(IDS_TASK_MANAGER_KILL));
  kill_button_->AddAccelerator(ui::Accelerator(ui::VKEY_E, ui::EF_NONE));
   kill_button_->SetAccessibleKeyboardShortcut(L"E");
   kill_button_->set_prefix_type(views::TextButtonBase::PREFIX_SHOW);
   about_memory_link_ = new views::Link(
      l10n_util::GetStringUTF16(IDS_TASK_MANAGER_ABOUT_MEMORY_LINK));
  about_memory_link_->set_listener(this);

  OnSelectionChanged();
}
