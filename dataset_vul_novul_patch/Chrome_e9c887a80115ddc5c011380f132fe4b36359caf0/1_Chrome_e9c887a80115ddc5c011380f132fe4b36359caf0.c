ScriptPromise ImageBitmapFactories::createImageBitmap(EventTarget& eventTarget, HTMLCanvasElement* canvas, int sx, int sy, int sw, int sh, ExceptionState& exceptionState)
{
    ASSERT(eventTarget.toDOMWindow());

    if (!canvas) {
        exceptionState.throwTypeError("The canvas element provided is invalid.");
        return ScriptPromise();
    }
    if (!canvas->originClean()) {
        exceptionState.throwSecurityError("The canvas element provided is tainted with cross-origin data.");
        return ScriptPromise();
    }
    if (!sw || !sh) {
         exceptionState.throwDOMException(IndexSizeError, String::format("The source %s provided is 0.", sw ? "height" : "width"));
         return ScriptPromise();
     }
    return fulfillImageBitmap(eventTarget.executionContext(), ImageBitmap::create(canvas, IntRect(sx, sy, sw, sh)));
 }
