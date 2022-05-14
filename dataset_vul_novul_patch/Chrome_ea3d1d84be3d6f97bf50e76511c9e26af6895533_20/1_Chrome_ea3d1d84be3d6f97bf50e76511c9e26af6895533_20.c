   ResourceClientProxy(PluginChannelHost* channel, int instance_id)
     : channel_(channel), instance_id_(instance_id), resource_id_(0),
      notify_needed_(false), notify_data_(0),
       multibyte_response_expected_(false) {
   }
