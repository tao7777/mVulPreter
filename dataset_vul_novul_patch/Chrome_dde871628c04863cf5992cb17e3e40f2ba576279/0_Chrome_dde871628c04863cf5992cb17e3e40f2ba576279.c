void ChromotingInstance::HandleMessage(const pp::Var& message) {
  if (!message.is_string()) {
    LOG(ERROR) << "Received a message that is not a string.";
    return;
  }

  scoped_ptr<base::Value> json(
      base::JSONReader::Read(message.AsString(),
                             base::JSON_ALLOW_TRAILING_COMMAS));
  base::DictionaryValue* message_dict = nullptr;
  std::string method;
  base::DictionaryValue* data = nullptr;
  if (!json.get() ||
      !json->GetAsDictionary(&message_dict) ||
      !message_dict->GetString("method", &method) ||
      !message_dict->GetDictionary("data", &data)) {
    LOG(ERROR) << "Received invalid message:" << message.AsString();
    return;
  }

  if (method == "connect") {
    HandleConnect(*data);
  } else if (method == "disconnect") {
    HandleDisconnect(*data);
  } else if (method == "incomingIq") {
    HandleOnIncomingIq(*data);
  } else if (method == "releaseAllKeys") {
    HandleReleaseAllKeys(*data);
  } else if (method == "injectKeyEvent") {
    HandleInjectKeyEvent(*data);
  } else if (method == "remapKey") {
    HandleRemapKey(*data);
  } else if (method == "trapKey") {
    HandleTrapKey(*data);
  } else if (method == "sendClipboardItem") {
    HandleSendClipboardItem(*data);
  } else if (method == "notifyClientResolution") {
    HandleNotifyClientResolution(*data);
  } else if (method == "pauseVideo") {
    HandlePauseVideo(*data);
  } else if (method == "videoControl") {
    HandleVideoControl(*data);
  } else if (method == "pauseAudio") {
    HandlePauseAudio(*data);
  } else if (method == "useAsyncPinDialog") {
    use_async_pin_dialog_ = true;
  } else if (method == "onPinFetched") {
    HandleOnPinFetched(*data);
  } else if (method == "onThirdPartyTokenFetched") {
    HandleOnThirdPartyTokenFetched(*data);
  } else if (method == "requestPairing") {
    HandleRequestPairing(*data);
  } else if (method == "extensionMessage") {
    HandleExtensionMessage(*data);
  } else if (method == "allowMouseLock") {
    HandleAllowMouseLockMessage();
  } else if (method == "sendMouseInputWhenUnfocused") {
     HandleSendMouseInputWhenUnfocused();
   } else if (method == "delegateLargeCursors") {
     HandleDelegateLargeCursors();
  } else if (method == "enableDebugRegion") {
    HandleEnableDebugRegion(*data);
   }
 }
