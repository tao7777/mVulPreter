  void AddServiceRequestHandlerOnIoThread(
      const std::string& name,
       const ServiceRequestHandler& handler) {
     DCHECK(io_thread_checker_.CalledOnValidThread());
     auto result = request_handlers_.insert(std::make_pair(name, handler));
    DCHECK(result.second) << "ServiceRequestHandler for " << name
                          << " already exists.";
   }
