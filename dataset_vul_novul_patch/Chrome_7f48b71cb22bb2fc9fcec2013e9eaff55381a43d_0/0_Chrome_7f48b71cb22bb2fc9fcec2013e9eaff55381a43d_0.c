void RenderViewHostImpl::CreateNewWindow(
    int route_id,
    const ViewHostMsg_CreateWindow_Params& params,
    SessionStorageNamespace* session_storage_namespace) {
   ViewHostMsg_CreateWindow_Params validated_params(params);
   ChildProcessSecurityPolicyImpl* policy =
       ChildProcessSecurityPolicyImpl::GetInstance();
  FilterURL(policy, GetProcess(), false, &validated_params.opener_url);
  FilterURL(policy, GetProcess(), true,
            &validated_params.opener_security_origin);
 
   delegate_->CreateNewWindow(route_id, validated_params,
                              session_storage_namespace);
}
