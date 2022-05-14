void InputMethodIBus::ProcessUnfilteredKeyPressEvent(
    const base::NativeEvent& native_event,
    guint32 ibus_keyval) {
  DCHECK(native_event);

  TextInputClient* client = GetTextInputClient();
  DispatchKeyEventPostIME(native_event);

  if (client != GetTextInputClient())
    return;

  const uint32 state =
      EventFlagsFromXFlags(GetKeyEvent(native_event)->state);

  if (character_composer_.FilterKeyPress(ibus_keyval, state)) {
     string16 composed = character_composer_.composed_character();
     if (!composed.empty()) {
       client = GetTextInputClient();
      if (client)
        client->InsertText(composed);
     }
     return;
   }

  client = GetTextInputClient();

  uint16 ch = 0;
  if (!(state & ui::EF_CONTROL_DOWN))
    ch = ui::GetCharacterFromXEvent(native_event);
  if (!ch) {
    ch = ui::GetCharacterFromKeyCode(
        ui::KeyboardCodeFromNative(native_event), state);
  }

  if (client && ch)
    client->InsertChar(ch, state);
}
