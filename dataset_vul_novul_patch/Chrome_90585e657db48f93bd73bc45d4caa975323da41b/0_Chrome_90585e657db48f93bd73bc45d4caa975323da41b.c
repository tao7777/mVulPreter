void WebUIExtension::Send(gin::Arguments* args) {
  blink::WebLocalFrame* frame;
  RenderFrame* render_frame;
  if (!ShouldRespondToRequest(&frame, &render_frame))
    return;

  std::string message;
  if (!args->GetNext(&message)) {
    args->ThrowError();
    return;
  }

  if (base::EndsWith(message, "RequiringGesture",
                     base::CompareCase::SENSITIVE) &&
      !blink::WebUserGestureIndicator::IsProcessingUserGesture(frame)) {
    NOTREACHED();
    return;
  }

  std::unique_ptr<base::ListValue> content;
  if (args->PeekNext().IsEmpty() || args->PeekNext()->IsUndefined()) {
    content.reset(new base::ListValue());
  } else {
    v8::Local<v8::Object> obj;
    if (!args->GetNext(&obj)) {
      args->ThrowError();
      return;
    }

     content = base::ListValue::From(V8ValueConverter::Create()->FromV8Value(
         obj, frame->MainWorldScriptContext()));
     DCHECK(content);
    // The conversion of |obj| could have triggered arbitrary JavaScript code,
    // so check that the frame is still valid to avoid dereferencing a stale
    // pointer.
    if (frame != blink::WebLocalFrame::FrameForCurrentContext()) {
      NOTREACHED();
      return;
    }
   }
 
  render_frame->Send(new FrameHostMsg_WebUISend(render_frame->GetRoutingID(),
                                                frame->GetDocument().Url(),
                                                message, *content));
}
