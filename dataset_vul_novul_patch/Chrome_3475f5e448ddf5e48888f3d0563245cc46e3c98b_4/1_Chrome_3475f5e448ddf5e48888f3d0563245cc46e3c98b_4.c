LauncherView::LauncherView(LauncherModel* model,
                           LauncherDelegate* delegate,
                           ShelfLayoutManager* shelf_layout_manager)
     : model_(model),
       delegate_(delegate),
       view_model_(new views::ViewModel),
       last_visible_index_(-1),
       overflow_button_(NULL),
       dragging_(false),
       drag_view_(NULL),
       drag_offset_(0),
       start_drag_index_(-1),
       context_menu_id_(0),
      alignment_(SHELF_ALIGNMENT_BOTTOM) {
   DCHECK(model_);
   bounds_animator_.reset(new views::BoundsAnimator(this));
   bounds_animator_->AddObserver(this);
  set_context_menu_controller(this);
  focus_search_.reset(new LauncherFocusSearch(view_model_.get()));
  tooltip_.reset(new LauncherTooltipManager(alignment_, shelf_layout_manager));
}
