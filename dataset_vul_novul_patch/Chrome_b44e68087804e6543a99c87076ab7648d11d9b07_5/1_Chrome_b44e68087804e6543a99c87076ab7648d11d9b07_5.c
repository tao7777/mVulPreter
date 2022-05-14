void ResourceCoordinatorService::OnStart() {
  ref_factory_.reset(new service_manager::ServiceContextRefFactory(
      base::Bind(&service_manager::ServiceContext::RequestQuit,
                 base::Unretained(context()))));

  ukm_recorder_ = ukm::MojoUkmRecorder::Create(context()->connector());

  registry_.AddInterface(
      base::Bind(&CoordinationUnitIntrospectorImpl::BindToInterface,
                 base::Unretained(&introspector_)));

  auto page_signal_generator_impl = std::make_unique<PageSignalGeneratorImpl>();
  registry_.AddInterface(
      base::Bind(&PageSignalGeneratorImpl::BindToInterface,
                 base::Unretained(page_signal_generator_impl.get())));
  coordination_unit_manager_.RegisterObserver(
      std::move(page_signal_generator_impl));

  coordination_unit_manager_.RegisterObserver(
      std::make_unique<MetricsCollector>());

  coordination_unit_manager_.RegisterObserver(
      std::make_unique<IPCVolumeReporter>(
          std::make_unique<base::OneShotTimer>()));

  coordination_unit_manager_.OnStart(&registry_, ref_factory_.get());
  coordination_unit_manager_.set_ukm_recorder(ukm_recorder_.get());

  memory_instrumentation_coordinator_ =
      std::make_unique<memory_instrumentation::CoordinatorImpl>(
          context()->connector());
   registry_.AddInterface(base::BindRepeating(
       &memory_instrumentation::CoordinatorImpl::BindCoordinatorRequest,
       base::Unretained(memory_instrumentation_coordinator_.get())));
 
   tracing_agent_registry_ = std::make_unique<tracing::AgentRegistry>();
   registry_.AddInterface(
      base::BindRepeating(&tracing::AgentRegistry::BindAgentRegistryRequest,
                          base::Unretained(tracing_agent_registry_.get())));

  tracing_coordinator_ = std::make_unique<tracing::Coordinator>();
  registry_.AddInterface(
      base::BindRepeating(&tracing::Coordinator::BindCoordinatorRequest,
                          base::Unretained(tracing_coordinator_.get())));
}
