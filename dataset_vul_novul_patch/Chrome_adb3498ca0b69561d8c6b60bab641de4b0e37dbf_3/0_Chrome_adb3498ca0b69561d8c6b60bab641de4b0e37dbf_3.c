void GraphicsContext::clipPath(const Path&, WindRule)
void GraphicsContext::clipPath(const Path& path, WindRule clipRule)
 {
    if (paintingDisabled())
        return;
        
    if (path.isEmpty())
        return; 
    
    wxGraphicsContext* gc = m_data->context->GetGraphicsContext();

#if __WXMAC__
    CGContextRef context = (CGContextRef)gc->GetNativeContext();   
    CGPathRef nativePath = (CGPathRef)path.platformPath()->GetNativePath(); 
    
    CGContextBeginPath(context);
    CGContextAddPath(context, nativePath);
    if (clipRule == RULE_EVENODD)
        CGContextEOClip(context);
    else
        CGContextClip(context);
#elif __WXMSW__
    Gdiplus::Graphics* g = (Gdiplus::Graphics*)gc->GetNativeContext();
    Gdiplus::GraphicsPath* nativePath = (Gdiplus::GraphicsPath*)path.platformPath()->GetNativePath();
    if (clipRule == RULE_EVENODD)
        nativePath->SetFillMode(Gdiplus::FillModeAlternate);
    else
        nativePath->SetFillMode(Gdiplus::FillModeWinding);
    g->SetClip(nativePath);
#endif
 }
