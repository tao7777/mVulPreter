   void LoadURL() {
    WebContents* contents = shell()->web_contents();
    WebPreferences prefs =
        contents->GetRenderViewHost()->GetWebkitPreferences();
    prefs.mock_scrollbars_enabled = true;
    contents->GetRenderViewHost()->UpdateWebkitPreferences(prefs);
     const GURL data_url(kDataURL);
     NavigateToURL(shell(), data_url);
 
     RenderWidgetHostImpl* host = GetWidgetHost();
 
     HitTestRegionObserver observer(host->GetFrameSinkId());
 
    observer.WaitForHitTestData();
  }
