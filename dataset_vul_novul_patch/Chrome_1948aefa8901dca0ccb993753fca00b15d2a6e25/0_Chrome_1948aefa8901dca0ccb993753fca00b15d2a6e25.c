void HTMLAnchorElement::handleClick(Event* event)
{
    event->setDefaultHandled();

    LocalFrame* frame = document().frame();
    if (!frame)
        return;

    StringBuilder url;
    url.append(stripLeadingAndTrailingHTMLSpaces(fastGetAttribute(hrefAttr)));
    appendServerMapMousePosition(url, event);
    KURL completedURL = document().completeURL(url.toString());

    sendPings(completedURL);

    ResourceRequest request(completedURL);
    request.setUIStartTime(event->platformTimeStamp());
    request.setInputPerfMetricReportPolicy(InputToLoadPerfMetricReportPolicy::ReportLink);

    ReferrerPolicy policy;
    if (hasAttribute(referrerpolicyAttr) && SecurityPolicy::referrerPolicyFromString(fastGetAttribute(referrerpolicyAttr), &policy) && !hasRel(RelationNoReferrer)) {
        request.setHTTPReferrer(SecurityPolicy::generateReferrer(policy, completedURL, document().outgoingReferrer()));
    }

    if (hasAttribute(downloadAttr)) {
        request.setRequestContext(WebURLRequest::RequestContextDownload);
        bool isSameOrigin = completedURL.protocolIsData() || document().getSecurityOrigin()->canRequest(completedURL);
        const AtomicString& suggestedName = (isSameOrigin ? fastGetAttribute(downloadAttr) : nullAtom);

        frame->loader().client()->loadURLExternally(request, NavigationPolicyDownload, suggestedName, false);
    } else {
        request.setRequestContext(WebURLRequest::RequestContextHyperlink);
        FrameLoadRequest frameRequest(&document(), request, getAttribute(targetAttr));
        frameRequest.setTriggeringEvent(event);
        if (hasRel(RelationNoReferrer)) {
            frameRequest.setShouldSendReferrer(NeverSendReferrer);
            frameRequest.setShouldSetOpener(NeverSetOpener);
         }
         if (hasRel(RelationNoOpener))
             frameRequest.setShouldSetOpener(NeverSetOpener);
        // TODO(japhet): Link clicks can be emulated via JS without a user gesture.
        // Why doesn't this go through NavigationScheduler?
         frame->loader().load(frameRequest);
     }
 }
