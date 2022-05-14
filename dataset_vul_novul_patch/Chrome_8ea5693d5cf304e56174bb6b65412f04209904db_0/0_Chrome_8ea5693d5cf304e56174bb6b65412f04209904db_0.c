static bool ExecuteTranspose(LocalFrame& frame,
                              Event*,
                              EditorCommandSource,
                              const String&) {
  Transpose(frame);
   return true;
 }
