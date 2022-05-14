BrowserActionsContainer::BrowserActionsContainer(
    Profile* profile, ToolbarView* toolbar)
    : profile_(profile),
       toolbar_(toolbar),
       popup_(NULL),
       popup_button_(NULL),
       resize_gripper_(NULL),
       chevron_(NULL),
       suppress_chevron_(false),
       resize_amount_(0),
       animation_target_size_(0),
       ALLOW_THIS_IN_INITIALIZER_LIST(task_factory_(this)) {
   ExtensionsService* extension_service = profile->GetExtensionsService();
   if (!extension_service)  // The |extension_service| can be NULL in Incognito.
     return;
 
  registrar_.Add(this, NotificationType::EXTENSION_LOADED,
                 Source<Profile>(profile_));
  registrar_.Add(this, NotificationType::EXTENSION_UNLOADED,
                 Source<Profile>(profile_));
  registrar_.Add(this, NotificationType::EXTENSION_UNLOADED_DISABLED,
                 Source<Profile>(profile_));
   registrar_.Add(this, NotificationType::EXTENSION_HOST_VIEW_SHOULD_CLOSE,
                  Source<Profile>(profile_));
 
   resize_animation_.reset(new SlideAnimation(this));

   resize_gripper_ = new views::ResizeGripper(this);
   resize_gripper_->SetVisible(false);
   AddChildView(resize_gripper_);

  ResourceBundle& rb = ResourceBundle::GetSharedInstance();
  SkBitmap* chevron_image = rb.GetBitmapNamed(IDR_BOOKMARK_BAR_CHEVRONS);
  chevron_ = new views::MenuButton(NULL, std::wstring(), this, false);
  chevron_->SetVisible(false);
  chevron_->SetIcon(*chevron_image);
  chevron_->EnableCanvasFlippingForRTLUI(true);
  AddChildView(chevron_);

   int predefined_width =
       profile_->GetPrefs()->GetInteger(prefs::kBrowserActionContainerWidth);
   container_size_ = gfx::Size(predefined_width, kButtonSize);

  SetID(VIEW_ID_BROWSER_ACTION_TOOLBAR);
 }
