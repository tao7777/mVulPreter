  void CancelHandwriting(int n_strokes) {
     IBusInputContext* context = GetInputContext(input_context_path_, ibus_);
     if (!context) {
       return;
    }
    ibus_input_context_cancel_hand_writing(context, n_strokes);
     g_object_unref(context);
   }
