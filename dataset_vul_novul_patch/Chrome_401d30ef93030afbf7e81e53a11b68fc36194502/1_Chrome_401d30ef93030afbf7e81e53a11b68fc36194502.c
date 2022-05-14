Document::Document(const DocumentInit& initializer, DocumentClassFlags documentClasses)
    : ContainerNode(0, CreateDocument)
    , TreeScope(this)
    , m_styleResolverThrowawayTimer(this, &Document::styleResolverThrowawayTimerFired)
    , m_styleResolverAccessCount(0)
    , m_lastStyleResolverAccessCount(0)
    , m_didCalculateStyleResolver(false)
    , m_ignorePendingStylesheets(false)
    , m_evaluateMediaQueriesOnStyleRecalc(false)
    , m_needsNotifyRemoveAllPendingStylesheet(false)
    , m_hasNodesWithPlaceholderStyle(false)
    , m_pendingSheetLayout(NoLayoutWithPendingSheets)
    , m_frame(initializer.frame())
    , m_domWindow(0)
    , m_import(initializer.import())
    , m_activeParserCount(0)
    , m_contextFeatures(ContextFeatures::defaultSwitch())
    , m_wellFormed(false)
    , m_printing(false)
    , m_paginatedForScreen(false)
    , m_compatibilityMode(NoQuirksMode)
    , m_compatibilityModeLocked(false)
    , m_didPostCheckFocusedElementTask(false)
    , m_domTreeVersion(++s_globalTreeVersion)
    , m_listenerTypes(0)
    , m_mutationObserverTypes(0)
    , m_styleEngine(StyleEngine::create(*this))
    , m_visitedLinkState(VisitedLinkState::create(*this))
    , m_visuallyOrdered(false)
    , m_readyState(Complete)
    , m_bParsing(false)
    , m_styleRecalcTimer(this, &Document::styleRecalcTimerFired)
    , m_inStyleRecalc(false)
    , m_gotoAnchorNeededAfterStylesheetsLoad(false)
    , m_containsValidityStyleRules(false)
    , m_updateFocusAppearanceRestoresSelection(false)
    , m_ignoreDestructiveWriteCount(0)
    , m_titleSetExplicitly(false)
    , m_markers(adoptPtr(new DocumentMarkerController))
    , m_updateFocusAppearanceTimer(this, &Document::updateFocusAppearanceTimerFired)
    , m_cssTarget(0)
    , m_loadEventProgress(LoadEventNotRun)
    , m_startTime(currentTime())
    , m_overMinimumLayoutThreshold(false)
    , m_scriptRunner(ScriptRunner::create(this))
    , m_xmlVersion("1.0")
    , m_xmlStandalone(StandaloneUnspecified)
    , m_hasXMLDeclaration(0)
    , m_designMode(inherit)
    , m_hasAnnotatedRegions(false)
    , m_annotatedRegionsDirty(false)
    , m_useSecureKeyboardEntryWhenActive(false)
    , m_documentClasses(documentClasses)
    , m_isViewSource(false)
     , m_sawElementsInKnownNamespaces(false)
     , m_isSrcdocDocument(false)
     , m_isMobileDocument(false)
     , m_renderer(0)
     , m_eventQueue(DocumentEventQueue::create(this))
     , m_weakFactory(this)
    , m_contextDocument(initializer.contextDocument())
    , m_idAttributeName(idAttr)
    , m_hasFullscreenElementStack(false)
    , m_loadEventDelayCount(0)
    , m_loadEventDelayTimer(this, &Document::loadEventDelayTimerFired)
    , m_referrerPolicy(ReferrerPolicyDefault)
    , m_directionSetOnDocumentElement(false)
    , m_writingModeSetOnDocumentElement(false)
    , m_didAllowNavigationViaBeforeUnloadConfirmationPanel(false)
    , m_writeRecursionIsTooDeep(false)
    , m_writeRecursionDepth(0)
    , m_lastHandledUserGestureTimestamp(0)
    , m_textAutosizer(TextAutosizer::create(this))
    , m_registrationContext(initializer.registrationContext(this))
    , m_pendingTasksTimer(this, &Document::pendingTasksTimerFired)
    , m_scheduledTasksAreSuspended(false)
    , m_sharedObjectPoolClearTimer(this, &Document::sharedObjectPoolClearTimerFired)
#ifndef NDEBUG
    , m_didDispatchViewportPropertiesChanged(false)
#endif
    , m_timeline(DocumentTimeline::create(this))
    , m_templateDocumentHost(0)
    , m_fonts(0)
    , m_didAssociateFormControlsTimer(this, &Document::didAssociateFormControlsTimerFired)
{
    setClient(this);
    ScriptWrappable::init(this);

    if (m_frame) {
        provideContextFeaturesToDocumentFrom(this, m_frame->page());

        m_fetcher = m_frame->loader()->activeDocumentLoader()->fetcher();
    }

    if (!m_fetcher)
        m_fetcher = ResourceFetcher::create(0);
    m_fetcher->setDocument(this);

    if (initializer.shouldSetURL())
        setURL(initializer.url());

    initSecurityContext(initializer);
    initDNSPrefetch();

    for (unsigned i = 0; i < WTF_ARRAY_LENGTH(m_nodeListCounts); i++)
        m_nodeListCounts[i] = 0;

    InspectorCounters::incrementCounter(InspectorCounters::DocumentCounter);

    m_lifecyle.advanceTo(DocumentLifecycle::Inactive);
}
