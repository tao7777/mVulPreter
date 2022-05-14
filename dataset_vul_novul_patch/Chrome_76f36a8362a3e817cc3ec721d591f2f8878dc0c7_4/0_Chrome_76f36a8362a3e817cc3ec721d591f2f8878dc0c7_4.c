TaskQueueManager::TaskQueueManager(
    size_t task_queue_count,
    scoped_refptr<NestableSingleThreadTaskRunner> main_task_runner,
    TaskQueueSelector* selector,
    const char* disabled_by_default_tracing_category)
    : main_task_runner_(main_task_runner),
      selector_(selector),
       task_was_run_bitmap_(0),
       pending_dowork_count_(0),
       work_batch_size_(1),
      time_source_(new base::DefaultTickClock),
       disabled_by_default_tracing_category_(
           disabled_by_default_tracing_category),
       deletion_sentinel_(new DeletionSentinel()),
      weak_factory_(this) {
  DCHECK(main_task_runner->RunsTasksOnCurrentThread());
  DCHECK_LE(task_queue_count, sizeof(task_was_run_bitmap_) * CHAR_BIT)
      << "You need a bigger int for task_was_run_bitmap_";
  TRACE_EVENT_OBJECT_CREATED_WITH_ID(disabled_by_default_tracing_category,
                                     "TaskQueueManager", this);

  for (size_t i = 0; i < task_queue_count; i++) {
    scoped_refptr<internal::TaskQueue> queue(make_scoped_refptr(
        new internal::TaskQueue(this, disabled_by_default_tracing_category)));
    queues_.push_back(queue);
  }

  std::vector<const base::TaskQueue*> work_queues;
  for (const auto& queue : queues_)
    work_queues.push_back(&queue->work_queue());
  selector_->RegisterWorkQueues(work_queues);
  selector_->SetTaskQueueSelectorObserver(this);

  do_work_from_main_thread_closure_ =
      base::Bind(&TaskQueueManager::DoWork, weak_factory_.GetWeakPtr(), true);
  do_work_from_other_thread_closure_ =
      base::Bind(&TaskQueueManager::DoWork, weak_factory_.GetWeakPtr(), false);
}
