void NetworkScreen::OnConnectionTimeout() {
  StopWaitingForConnection(network_id_);
  NetworkLibrary* network = CrosLibrary::Get()->GetNetworkLibrary();
  bool is_connected = network && network->Connected();

  if (!is_connected &&
      !view()->is_dialog_open() &&
      !(help_app_.get() && help_app_->is_open())) {
    ClearErrors();
    views::View* network_control = view()->GetNetworkControlView();
    bubble_ = MessageBubble::Show(
        network_control->GetWidget(),
        network_control->GetScreenBounds(),
        BubbleBorder::LEFT_TOP,
        ResourceBundle::GetSharedInstance().GetBitmapNamed(IDR_WARNING),
        UTF16ToWide(l10n_util::GetStringFUTF16(
             IDS_NETWORK_SELECTION_ERROR,
             l10n_util::GetStringUTF16(IDS_PRODUCT_OS_NAME),
             network_id_)),
        UTF16ToWide(l10n_util::GetStringUTF16(IDS_LEARN_MORE)),
         this);
     network_control->RequestFocus();
   }
}
