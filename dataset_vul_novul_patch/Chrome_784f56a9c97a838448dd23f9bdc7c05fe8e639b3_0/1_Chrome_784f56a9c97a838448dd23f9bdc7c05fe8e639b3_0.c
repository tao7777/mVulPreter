 void RenderFrameHostImpl::ResetFeaturePolicy() {
   RenderFrameHostImpl* parent_frame_host = GetParent();
   const FeaturePolicy* parent_policy =
      parent_frame_host ? parent_frame_host->get_feature_policy() : nullptr;
   ParsedFeaturePolicyHeader container_policy =
       frame_tree_node()->effective_container_policy();
   feature_policy_ = FeaturePolicy::CreateFromParentPolicy(
      parent_policy, container_policy, last_committed_origin_);
}
