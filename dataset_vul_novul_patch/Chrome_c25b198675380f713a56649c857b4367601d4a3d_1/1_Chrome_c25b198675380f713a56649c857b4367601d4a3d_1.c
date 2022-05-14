LockScreenMediaControlsView::LockScreenMediaControlsView(
    service_manager::Connector* connector,
    const Callbacks& callbacks)
    : connector_(connector),
      hide_controls_timer_(new base::OneShotTimer()),
      media_controls_enabled_(callbacks.media_controls_enabled),
      hide_media_controls_(callbacks.hide_media_controls),
      show_media_controls_(callbacks.show_media_controls) {
  DCHECK(callbacks.media_controls_enabled);
  DCHECK(callbacks.hide_media_controls);
  DCHECK(callbacks.show_media_controls);

  Shell::Get()->media_controller()->SetMediaControlsDismissed(false);

  middle_spacing_ = std::make_unique<NonAccessibleView>();
  middle_spacing_->set_owned_by_client();

  set_notify_enter_exit_on_child(true);
 
   contents_view_ = AddChildView(std::make_unique<views::View>());
   contents_view_->SetLayoutManager(std::make_unique<views::BoxLayout>(
      views::BoxLayout::Orientation::kVertical));
   contents_view_->SetBackground(views::CreateRoundedRectBackground(
       kMediaControlsBackground, kMediaControlsCornerRadius));
 
   contents_view_->SetPaintToLayer();  // Needed for opacity animation.
   contents_view_->layer()->SetFillsBoundsOpaquely(false);
 
  auto close_button_row = std::make_unique<NonAccessibleView>();
  views::GridLayout* close_button_layout =
      close_button_row->SetLayoutManager(std::make_unique<views::GridLayout>());
  views::ColumnSet* columns = close_button_layout->AddColumnSet(0);
  columns->AddPaddingColumn(0, kCloseButtonOffset);
  columns->AddColumn(views::GridLayout::CENTER, views::GridLayout::CENTER, 0,
                     views::GridLayout::USE_PREF, 0, 0);
  close_button_layout->StartRowWithPadding(
      0, 0, 0, 5 /* padding between close button and top of view */);
  auto close_button = CreateVectorImageButton(this);
  SetImageFromVectorIcon(close_button.get(), vector_icons::kCloseRoundedIcon,
                         kCloseButtonIconSize, gfx::kGoogleGrey700);
  close_button->SetPreferredSize(kCloseButtonSize);
  close_button->SetFocusBehavior(View::FocusBehavior::ALWAYS);
  base::string16 close_button_label(
      l10n_util::GetStringUTF16(IDS_ASH_LOCK_SCREEN_MEDIA_CONTROLS_CLOSE));
  close_button->SetAccessibleName(close_button_label);
  close_button_ = close_button_layout->AddView(std::move(close_button));
  close_button_->SetVisible(false);
  contents_view_->AddChildView(std::move(close_button_row));
  header_row_ =
      contents_view_->AddChildView(std::make_unique<MediaControlsHeaderView>());
 
   auto session_artwork = std::make_unique<views::ImageView>();
  session_artwork->SetPreferredSize(
      gfx::Size(kArtworkViewWidth, kArtworkViewHeight));
  session_artwork->SetBorder(views::CreateEmptyBorder(kArtworkInsets));
   session_artwork_ = contents_view_->AddChildView(std::move(session_artwork));
 
   progress_ = contents_view_->AddChildView(
      std::make_unique<media_message_center::MediaControlsProgressView>(
          base::BindRepeating(&LockScreenMediaControlsView::SeekTo,
                              base::Unretained(this))));

  auto button_row = std::make_unique<NonAccessibleView>();
  auto* button_row_layout =
      button_row->SetLayoutManager(std::make_unique<views::BoxLayout>(
          views::BoxLayout::Orientation::kHorizontal, kButtonRowInsets,
          kMediaButtonRowSeparator));
  button_row_layout->set_cross_axis_alignment(
      views::BoxLayout::CrossAxisAlignment::kCenter);
  button_row_layout->set_main_axis_alignment(
      views::BoxLayout::MainAxisAlignment::kCenter);
  button_row->SetPreferredSize(kMediaControlsButtonRowSize);
  button_row_ = contents_view_->AddChildView(std::move(button_row));

  CreateMediaButton(
      kChangeTrackIconSize, MediaSessionAction::kPreviousTrack,
      l10n_util::GetStringUTF16(
          IDS_ASH_LOCK_SCREEN_MEDIA_CONTROLS_ACTION_PREVIOUS_TRACK));

  CreateMediaButton(
      kSeekingIconsSize, MediaSessionAction::kSeekBackward,
      l10n_util::GetStringUTF16(
          IDS_ASH_LOCK_SCREEN_MEDIA_CONTROLS_ACTION_SEEK_BACKWARD));

  auto play_pause_button = views::CreateVectorToggleImageButton(this);
  play_pause_button->set_tag(static_cast<int>(MediaSessionAction::kPause));
  play_pause_button->SetPreferredSize(kMediaButtonSize);
  play_pause_button->SetFocusBehavior(views::View::FocusBehavior::ALWAYS);
  play_pause_button->SetTooltipText(l10n_util::GetStringUTF16(
      IDS_ASH_LOCK_SCREEN_MEDIA_CONTROLS_ACTION_PAUSE));
  play_pause_button->SetToggledTooltipText(l10n_util::GetStringUTF16(
      IDS_ASH_LOCK_SCREEN_MEDIA_CONTROLS_ACTION_PLAY));
  play_pause_button_ = button_row_->AddChildView(std::move(play_pause_button));

  views::SetImageFromVectorIcon(
      play_pause_button_,
      GetVectorIconForMediaAction(MediaSessionAction::kPause),
      kPlayPauseIconSize, kMediaButtonColor);
  views::SetToggledImageFromVectorIcon(
      play_pause_button_,
      GetVectorIconForMediaAction(MediaSessionAction::kPlay),
      kPlayPauseIconSize, kMediaButtonColor);

  CreateMediaButton(
      kSeekingIconsSize, MediaSessionAction::kSeekForward,
      l10n_util::GetStringUTF16(
          IDS_ASH_LOCK_SCREEN_MEDIA_CONTROLS_ACTION_SEEK_FORWARD));

  CreateMediaButton(kChangeTrackIconSize, MediaSessionAction::kNextTrack,
                    l10n_util::GetStringUTF16(
                        IDS_ASH_LOCK_SCREEN_MEDIA_CONTROLS_ACTION_NEXT_TRACK));

  MediaSessionMetadataChanged(base::nullopt);
  MediaSessionPositionChanged(base::nullopt);
  MediaControllerImageChanged(
      media_session::mojom::MediaSessionImageType::kSourceIcon, SkBitmap());
  SetArtwork(base::nullopt);

  if (!connector_)
    return;

  mojo::Remote<media_session::mojom::MediaControllerManager>
      controller_manager_remote;
  connector_->Connect(media_session::mojom::kServiceName,
                      controller_manager_remote.BindNewPipeAndPassReceiver());
  controller_manager_remote->CreateActiveMediaController(
      media_controller_remote_.BindNewPipeAndPassReceiver());

  media_controller_remote_->AddObserver(
      observer_receiver_.BindNewPipeAndPassRemote());

  media_controller_remote_->ObserveImages(
      media_session::mojom::MediaSessionImageType::kArtwork,
      kMinimumArtworkSize, kDesiredArtworkSize,
      artwork_observer_receiver_.BindNewPipeAndPassRemote());

  media_controller_remote_->ObserveImages(
      media_session::mojom::MediaSessionImageType::kSourceIcon,
      kMinimumIconSize, kDesiredIconSize,
      icon_observer_receiver_.BindNewPipeAndPassRemote());
}
