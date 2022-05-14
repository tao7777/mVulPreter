  static void UpdatePropertyHandler(
      void* object, const ImePropertyList& prop_list) {
     if (!BrowserThread::CurrentlyOn(BrowserThread::UI)) {
       LOG(ERROR) << "Not on UI thread";
       return;
     }
 
    InputMethodLibraryImpl* input_method_library =
        static_cast<InputMethodLibraryImpl*>(object);
    input_method_library->UpdateProperty(prop_list);
   }
