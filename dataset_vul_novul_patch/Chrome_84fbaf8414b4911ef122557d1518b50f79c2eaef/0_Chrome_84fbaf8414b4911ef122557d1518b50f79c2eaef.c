void OomInterventionTabHelper::StartDetectionInRenderer() {
  auto* config = OomInterventionConfig::GetInstance();
  bool renderer_pause_enabled = config->is_renderer_pause_enabled();
  bool navigate_ads_enabled = config->is_navigate_ads_enabled();

  if ((renderer_pause_enabled || navigate_ads_enabled) && decider_) {
    DCHECK(!web_contents()->GetBrowserContext()->IsOffTheRecord());
    const std::string& host = web_contents()->GetVisibleURL().host();
    if (!decider_->CanTriggerIntervention(host)) {
      renderer_pause_enabled = false;
      navigate_ads_enabled = false;
     }
   }
 
  if (!renderer_pause_enabled && !navigate_ads_enabled)
    return;
   content::RenderFrameHost* main_frame = web_contents()->GetMainFrame();
   DCHECK(main_frame);
   content::RenderProcessHost* render_process_host = main_frame->GetProcess();
  DCHECK(render_process_host);
  content::BindInterface(render_process_host,
                         mojo::MakeRequest(&intervention_));
  DCHECK(!binding_.is_bound());
  blink::mojom::OomInterventionHostPtr host;
  binding_.Bind(mojo::MakeRequest(&host));
  blink::mojom::DetectionArgsPtr detection_args =
      config->GetRendererOomDetectionArgs();
  intervention_->StartDetection(std::move(host), std::move(detection_args),
                                renderer_pause_enabled, navigate_ads_enabled);
}
