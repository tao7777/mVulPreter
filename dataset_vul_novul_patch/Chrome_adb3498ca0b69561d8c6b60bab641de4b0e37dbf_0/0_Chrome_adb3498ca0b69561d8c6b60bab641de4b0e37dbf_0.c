 void GraphicsContext::clip(const Path& path) 
{ 
     if (paintingDisabled())
         return;
 
    // if the path is empty, we clip against a zero rect to reduce the clipping region to
    // nothing - which is the intended behavior of clip() if the path is empty.
     if (path.isEmpty())
        m_data->context->SetClippingRegion(0, 0, 0, 0);
    else
        clipPath(path, RULE_NONZERO);
 }
