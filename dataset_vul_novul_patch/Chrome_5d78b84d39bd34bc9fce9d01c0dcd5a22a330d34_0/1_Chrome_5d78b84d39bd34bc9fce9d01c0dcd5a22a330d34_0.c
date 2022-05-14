LayerTreeHost::LayerTreeHost(InitParams* params, CompositorMode mode)
    : micro_benchmark_controller_(this),
      image_worker_task_runner_(params->image_worker_task_runner),
      compositor_mode_(mode),
      ui_resource_manager_(base::MakeUnique<UIResourceManager>()),
      client_(params->client),
      rendering_stats_instrumentation_(RenderingStatsInstrumentation::Create()),
      settings_(*params->settings),
       debug_state_(settings_.initial_debug_state),
       id_(s_layer_tree_host_sequence_number.GetNext() + 1),
       task_graph_runner_(params->task_graph_runner),
       event_listener_properties_(),
       mutator_host_(params->mutator_host) {
   DCHECK(task_graph_runner_);
  DCHECK(!settings_.enable_checker_imaging || image_worker_task_runner_);
  DCHECK(!settings_.enable_checker_imaging ||
         settings_.image_decode_tasks_enabled);

  mutator_host_->SetMutatorHostClient(this);

  rendering_stats_instrumentation_->set_record_rendering_stats(
      debug_state_.RecordRenderingStats());
}
