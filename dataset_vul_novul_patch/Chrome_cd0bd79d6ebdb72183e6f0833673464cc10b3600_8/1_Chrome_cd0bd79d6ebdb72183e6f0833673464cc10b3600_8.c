 void GpuMessageFilter::EstablishChannelCallback(
     IPC::Message* reply,
     const IPC::ChannelHandle& channel,
    base::ProcessHandle gpu_process_for_browser,
     const content::GPUInfo& gpu_info) {
   DCHECK(BrowserThread::CurrentlyOn(BrowserThread::IO));
 
  base::ProcessHandle renderer_process_for_gpu;
  if (gpu_process_for_browser != 0) {
#if defined(OS_WIN)
    DuplicateHandle(base::GetCurrentProcessHandle(),
                    peer_handle(),
                    gpu_process_for_browser,
                    &renderer_process_for_gpu,
                    PROCESS_DUP_HANDLE,
                    FALSE,
                    0);
#else
    renderer_process_for_gpu = peer_handle();
#endif
  } else {
    renderer_process_for_gpu = 0;
  }
   GpuHostMsg_EstablishGpuChannel::WriteReplyParams(
      reply, render_process_id_, channel, renderer_process_for_gpu, gpu_info);
   Send(reply);
 }
