bool Frame::canHaveSecureChild() const
{
    for (const Frame* parent = this; parent; parent = parent->tree().parent()) {
        if (!parent->securityContext()->getSecurityOrigin()->isPotentiallyTrustworthy())
            return false;
    }
    return true;
}
