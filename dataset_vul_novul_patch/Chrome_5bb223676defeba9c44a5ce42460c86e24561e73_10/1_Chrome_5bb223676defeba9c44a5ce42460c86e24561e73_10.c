    NavigateToAboutBlank() {
  GURL about_blank(url::kAboutBlankURL);
  content::NavigationController::LoadURLParams params(about_blank);
  params.frame_tree_node_id = frame_tree_node_id_;
  params.source_site_instance = parent_site_instance_;
  params.is_renderer_initiated = true;
  web_contents()->GetController().LoadURLWithParams(params);
}
