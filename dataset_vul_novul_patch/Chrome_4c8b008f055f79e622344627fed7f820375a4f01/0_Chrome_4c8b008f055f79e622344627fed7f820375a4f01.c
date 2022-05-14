 void Document::detach(const AttachContext& context)
 {
     TRACE_EVENT0("blink", "Document::detach");
    RELEASE_ASSERT(!m_frame || m_frame->tree().childCount() == 0);
     if (!isActive())
         return;
 
    FrameNavigationDisabler navigationDisabler(*m_frame);
    HTMLFrameOwnerElement::UpdateSuspendScope suspendWidgetHierarchyUpdates;
    ScriptForbiddenScope forbidScript;
    view()->dispose();
    m_markers->prepareForDestruction();
    if (LocalDOMWindow* window = this->domWindow())
        window->willDetachDocumentFromFrame();

    m_lifecycle.advanceTo(DocumentLifecycle::Stopping);

    if (page())
        page()->documentDetached(this);
    InspectorInstrumentation::documentDetached(this);

    if (m_frame->loader().client()->sharedWorkerRepositoryClient())
        m_frame->loader().client()->sharedWorkerRepositoryClient()->documentDetached(this);

    stopActiveDOMObjects();

    if (m_scriptedAnimationController)
        m_scriptedAnimationController->clearDocumentPointer();
    m_scriptedAnimationController.clear();

    m_scriptedIdleTaskController.clear();

    if (svgExtensions())
        accessSVGExtensions().pauseAnimations();

    if (m_domWindow)
        m_domWindow->clearEventQueue();

    if (m_layoutView)
        m_layoutView->setIsInWindow(false);

    if (registrationContext())
        registrationContext()->documentWasDetached();

    m_hoverNode = nullptr;
    m_activeHoverElement = nullptr;
    m_autofocusElement = nullptr;

    if (m_focusedElement.get()) {
        RefPtrWillBeRawPtr<Element> oldFocusedElement = m_focusedElement;
        m_focusedElement = nullptr;
        if (frameHost())
            frameHost()->chromeClient().focusedNodeChanged(oldFocusedElement.get(), nullptr);
    }

    if (this == &axObjectCacheOwner())
        clearAXObjectCache();

    m_layoutView = nullptr;
    ContainerNode::detach(context);

    if (this != &axObjectCacheOwner()) {
        if (AXObjectCache* cache = existingAXObjectCache()) {
            for (Node& node : NodeTraversal::descendantsOf(*this)) {
                cache->remove(&node);
            }
        }
    }

    styleEngine().didDetach();

    frameHost()->eventHandlerRegistry().documentDetached(*this);

    m_frame->inputMethodController().documentDetached();

    if (!loader())
        m_fetcher->clearContext();
    if (m_importsController)
        HTMLImportsController::removeFrom(*this);

    m_timers.setTimerTaskRunner(
        Platform::current()->currentThread()->scheduler()->timerTaskRunner()->adoptClone());

    m_frame = nullptr;

    if (m_mediaQueryMatcher)
        m_mediaQueryMatcher->documentDetached();

    DocumentLifecycleNotifier::notifyDocumentWasDetached();
    m_lifecycle.advanceTo(DocumentLifecycle::Stopped);

    DocumentLifecycleNotifier::notifyContextDestroyed();
    ExecutionContext::notifyContextDestroyed();
}
