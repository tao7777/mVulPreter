  virtual v8::Handle<v8::FunctionTemplate> GetNativeFunction(
      v8::Handle<v8::String> name) {
    if (name->Equals(v8::String::New("GetExtensionAPIDefinition"))) {
      return v8::FunctionTemplate::New(GetExtensionAPIDefinition);
    } else if (name->Equals(v8::String::New("GetExtensionViews"))) {
      return v8::FunctionTemplate::New(GetExtensionViews,
                                       v8::External::New(this));
    } else if (name->Equals(v8::String::New("GetNextRequestId"))) {
      return v8::FunctionTemplate::New(GetNextRequestId);
    } else if (name->Equals(v8::String::New("OpenChannelToTab"))) {
       return v8::FunctionTemplate::New(OpenChannelToTab);
     } else if (name->Equals(v8::String::New("GetNextContextMenuId"))) {
       return v8::FunctionTemplate::New(GetNextContextMenuId);
    } else if (name->Equals(v8::String::New("GetNextTtsEventId"))) {
      return v8::FunctionTemplate::New(GetNextTtsEventId);
     } else if (name->Equals(v8::String::New("GetCurrentPageActions"))) {
       return v8::FunctionTemplate::New(GetCurrentPageActions,
                                        v8::External::New(this));
    } else if (name->Equals(v8::String::New("StartRequest"))) {
      return v8::FunctionTemplate::New(StartRequest,
                                       v8::External::New(this));
    } else if (name->Equals(v8::String::New("GetRenderViewId"))) {
      return v8::FunctionTemplate::New(GetRenderViewId);
    } else if (name->Equals(v8::String::New("SetIconCommon"))) {
      return v8::FunctionTemplate::New(SetIconCommon,
                                       v8::External::New(this));
    } else if (name->Equals(v8::String::New("IsExtensionProcess"))) {
      return v8::FunctionTemplate::New(IsExtensionProcess,
                                       v8::External::New(this));
    } else if (name->Equals(v8::String::New("IsIncognitoProcess"))) {
      return v8::FunctionTemplate::New(IsIncognitoProcess);
    } else if (name->Equals(v8::String::New("GetUniqueSubEventName"))) {
      return v8::FunctionTemplate::New(GetUniqueSubEventName);
    } else if (name->Equals(v8::String::New("GetLocalFileSystem"))) {
      return v8::FunctionTemplate::New(GetLocalFileSystem);
    } else if (name->Equals(v8::String::New("DecodeJPEG"))) {
      return v8::FunctionTemplate::New(DecodeJPEG, v8::External::New(this));
    }

    return ExtensionBase::GetNativeFunction(name);
  }
