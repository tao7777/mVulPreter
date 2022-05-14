void GraphicsContext::addInnerRoundedRectClip(const IntRect& rect, int thickness)
void GraphicsContext::addInnerRoundedRectClip(const IntRect& r, int thickness)
 {
    if (paintingDisabled())
        return;

    FloatRect rect(r);
    clip(rect);
    Path path;
    path.addEllipse(rect);
    rect.inflate(-thickness);
    path.addEllipse(rect);
    clipPath(path, RULE_EVENODD);
 }
