 void GpuMessageFilter::EstablishChannelCallback(
     IPC::Message* reply,
     const IPC::ChannelHandle& channel,
     const content::GPUInfo& gpu_info) {
   DCHECK(BrowserThread::CurrentlyOn(BrowserThread::IO));
 
   GpuHostMsg_EstablishGpuChannel::WriteReplyParams(
      reply, render_process_id_, channel, gpu_info);
   Send(reply);
 }
