std::unique_ptr<content::BluetoothChooser> Browser::RunBluetoothChooser(
    content::RenderFrameHost* frame,
    const content::BluetoothChooser::EventHandler& event_handler) {
  std::unique_ptr<BluetoothChooserController> bluetooth_chooser_controller(
      new BluetoothChooserController(frame, event_handler));

  std::unique_ptr<BluetoothChooserDesktop> bluetooth_chooser_desktop(
      new BluetoothChooserDesktop(bluetooth_chooser_controller.get()));

  std::unique_ptr<ChooserBubbleDelegate> chooser_bubble_delegate(
      new ChooserBubbleDelegate(frame,
                                std::move(bluetooth_chooser_controller)));

  Browser* browser = chrome::FindBrowserWithWebContents(
       WebContents::FromRenderFrameHost(frame));
   BubbleReference bubble_reference = browser->GetBubbleManager()->ShowBubble(
       std::move(chooser_bubble_delegate));
  bluetooth_chooser_desktop->set_bubble(std::move(bubble_reference));
 
   return std::move(bluetooth_chooser_desktop);
 }
