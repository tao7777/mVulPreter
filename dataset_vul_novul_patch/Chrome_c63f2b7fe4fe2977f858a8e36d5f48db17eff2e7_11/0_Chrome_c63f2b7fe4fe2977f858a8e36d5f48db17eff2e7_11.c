bool ExtensionTtsSpeakFunction::RunImpl() {
  int src_id = -1;
  EXTENSION_FUNCTION_VALIDATE(
      options->GetInteger(constants::kSrcIdKey, &src_id));

  // If we got this far, the arguments were all in the valid format, so
  // send the success response to the callback now - this ensures that
  // the callback response always arrives before events, which makes
  // the behavior more predictable and easier to write unit tests for too.
  SendResponse(true);

  UtteranceContinuousParameters continuous_params;
  continuous_params.rate = rate;
  continuous_params.pitch = pitch;
  continuous_params.volume = volume;

  Utterance* utterance = new Utterance(profile());
  utterance->set_text(text);
  utterance->set_voice_name(voice_name);
  utterance->set_src_extension_id(extension_id());
  utterance->set_src_id(src_id);
  utterance->set_src_url(source_url());
  utterance->set_lang(lang);
  utterance->set_gender(gender);
  utterance->set_continuous_parameters(continuous_params);
  utterance->set_can_enqueue(can_enqueue);
  utterance->set_required_event_types(required_event_types);
  utterance->set_desired_event_types(desired_event_types);
  utterance->set_extension_id(voice_extension_id);
  utterance->set_options(options.get());

  ExtensionTtsController* controller = ExtensionTtsController::GetInstance();
  controller->SpeakOrEnqueue(utterance);
   return true;
 }
