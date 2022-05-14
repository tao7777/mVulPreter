   void LoadURL() {
     const GURL data_url(kDataURL);
     NavigateToURL(shell(), data_url);
 
     RenderWidgetHostImpl* host = GetWidgetHost();
    host->GetView()->SetSize(gfx::Size(400, 400));
 
     HitTestRegionObserver observer(host->GetFrameSinkId());
 
    observer.WaitForHitTestData();
  }
