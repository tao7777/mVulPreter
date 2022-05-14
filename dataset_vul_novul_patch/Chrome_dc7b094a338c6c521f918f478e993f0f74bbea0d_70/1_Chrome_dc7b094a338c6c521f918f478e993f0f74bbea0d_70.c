  void SendHandwritingStroke(const HandwritingStroke& stroke) {
     if (stroke.size() < 2) {
       LOG(WARNING) << "Empty stroke data or a single dot is passed.";
       return;
    }

    IBusInputContext* context = GetInputContext(input_context_path_, ibus_);
    if (!context) {
      return;
    }

    const size_t raw_stroke_size = stroke.size() * 2;
    scoped_array<double> raw_stroke(new double[raw_stroke_size]);
    for (size_t n = 0; n < stroke.size(); ++n) {
      raw_stroke[n * 2] = stroke[n].first;  // x
      raw_stroke[n * 2 + 1] = stroke[n].second;  // y
    }
    ibus_input_context_process_hand_writing_event(
        context, raw_stroke.get(), raw_stroke_size);
     g_object_unref(context);
   }
