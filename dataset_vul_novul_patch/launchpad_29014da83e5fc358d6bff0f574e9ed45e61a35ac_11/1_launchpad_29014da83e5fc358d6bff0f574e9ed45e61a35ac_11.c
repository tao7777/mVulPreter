 void BrowserMainParts::PostMainMessageLoopRun() {
   CompositorUtils::GetInstance()->Shutdown();
 }
