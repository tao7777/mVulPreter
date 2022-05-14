void RenderViewHostImpl::CreateNewWindow(
    int route_id,
    const ViewHostMsg_CreateWindow_Params& params,
    SessionStorageNamespace* session_storage_namespace) {
   ViewHostMsg_CreateWindow_Params validated_params(params);
   ChildProcessSecurityPolicyImpl* policy =
       ChildProcessSecurityPolicyImpl::GetInstance();
 
   delegate_->CreateNewWindow(route_id, validated_params,
                              session_storage_namespace);
}
