bool WebFrame::canHaveSecureChild() const
{
    Frame* frame = toImplBase()->frame();
    if (!frame)
        return false;
    return frame->canHaveSecureChild();
}
