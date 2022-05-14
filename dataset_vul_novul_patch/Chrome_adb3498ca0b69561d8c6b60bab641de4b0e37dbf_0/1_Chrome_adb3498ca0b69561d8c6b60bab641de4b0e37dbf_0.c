 void GraphicsContext::clip(const Path& path) 
{
#ifdef __WXMAC__
     if (paintingDisabled())
         return;
    wxGraphicsContext* gc = m_data->context->GetGraphicsContext();
    CGContextRef context = (CGContextRef)gc->GetNativeContext();
 
    if (!context)
        return;
    CGPathRef nativePath = (CGPathRef)path.platformPath()->GetNativePath();
     if (path.isEmpty())
        CGContextClipToRect(context, CGRectZero);
    else if (nativePath) {
        CGContextBeginPath(context);
        CGContextAddPath(context, nativePath);
        CGContextClip(context);
    }
#else
    notImplemented();
#endif
 }
