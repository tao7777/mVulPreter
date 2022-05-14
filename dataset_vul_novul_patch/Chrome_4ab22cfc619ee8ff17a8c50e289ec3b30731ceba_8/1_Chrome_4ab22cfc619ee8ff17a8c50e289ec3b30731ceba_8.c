void InitCallbacks(struct mg_context* ctx, Dispatcher* dispatcher,
                    base::WaitableEvent* shutdown_event,
                    bool forbid_other_requests) {
   dispatcher->AddShutdown("/shutdown", shutdown_event);
  dispatcher->AddStatus("/healthz");
 
   dispatcher->Add<CreateSession>("/session");
 
  dispatcher->Add<FindOneElementCommand>(  "/session/*/element");
  dispatcher->Add<FindManyElementsCommand>("/session/*/elements");
  dispatcher->Add<ActiveElementCommand>(   "/session/*/element/active");
  dispatcher->Add<FindOneElementCommand>(  "/session/*/element/*/element");
  dispatcher->Add<FindManyElementsCommand>("/session/*/elements/*/elements");
  dispatcher->Add<ElementAttributeCommand>("/session/*/element/*/attribute/*");
  dispatcher->Add<ElementCssCommand>(      "/session/*/element/*/css/*");
  dispatcher->Add<ElementClearCommand>(    "/session/*/element/*/clear");
  dispatcher->Add<ElementDisplayedCommand>("/session/*/element/*/displayed");
  dispatcher->Add<ElementEnabledCommand>(  "/session/*/element/*/enabled");
  dispatcher->Add<ElementEqualsCommand>(   "/session/*/element/*/equals/*");
  dispatcher->Add<ElementLocationCommand>( "/session/*/element/*/location");
  dispatcher->Add<ElementLocationInViewCommand>(
      "/session/*/element/*/location_in_view");
  dispatcher->Add<ElementNameCommand>(    "/session/*/element/*/name");
  dispatcher->Add<ElementSelectedCommand>("/session/*/element/*/selected");
  dispatcher->Add<ElementSizeCommand>(    "/session/*/element/*/size");
  dispatcher->Add<ElementSubmitCommand>(  "/session/*/element/*/submit");
  dispatcher->Add<ElementTextCommand>(    "/session/*/element/*/text");
  dispatcher->Add<ElementToggleCommand>(  "/session/*/element/*/toggle");
  dispatcher->Add<ElementValueCommand>(   "/session/*/element/*/value");

  dispatcher->Add<ScreenshotCommand>("/session/*/screenshot");

  dispatcher->Add<MoveAndClickCommand>("/session/*/element/*/click");
  dispatcher->Add<DragCommand>(        "/session/*/element/*/drag");
  dispatcher->Add<HoverCommand>(       "/session/*/element/*/hover");

  dispatcher->Add<MoveToCommand>(     "/session/*/moveto");
  dispatcher->Add<ClickCommand>(      "/session/*/click");
  dispatcher->Add<ButtonDownCommand>( "/session/*/buttondown");
  dispatcher->Add<ButtonUpCommand>(   "/session/*/buttonup");
  dispatcher->Add<DoubleClickCommand>("/session/*/doubleclick");

  dispatcher->Add<AcceptAlertCommand>(  "/session/*/accept_alert");
  dispatcher->Add<AlertTextCommand>(    "/session/*/alert_text");
  dispatcher->Add<BackCommand>(         "/session/*/back");
  dispatcher->Add<DismissAlertCommand>( "/session/*/dismiss_alert");
  dispatcher->Add<ExecuteCommand>(      "/session/*/execute");
  dispatcher->Add<ExecuteAsyncScriptCommand>(
                                        "/session/*/execute_async");
  dispatcher->Add<ForwardCommand>(      "/session/*/forward");
  dispatcher->Add<SwitchFrameCommand>(  "/session/*/frame");
  dispatcher->Add<RefreshCommand>(      "/session/*/refresh");
  dispatcher->Add<SourceCommand>(       "/session/*/source");
  dispatcher->Add<TitleCommand>(        "/session/*/title");
  dispatcher->Add<URLCommand>(          "/session/*/url");
  dispatcher->Add<WindowCommand>(       "/session/*/window");
  dispatcher->Add<WindowHandleCommand>( "/session/*/window_handle");
  dispatcher->Add<WindowHandlesCommand>("/session/*/window_handles");
  dispatcher->Add<SetAsyncScriptTimeoutCommand>(
                                        "/session/*/timeouts/async_script");
  dispatcher->Add<ImplicitWaitCommand>( "/session/*/timeouts/implicit_wait");

  dispatcher->Add<CookieCommand>(     "/session/*/cookie");
  dispatcher->Add<NamedCookieCommand>("/session/*/cookie/*");

  dispatcher->Add<SessionWithID>("/session/*");

  if (forbid_other_requests)
    dispatcher->ForbidAllOtherRequests();
}
