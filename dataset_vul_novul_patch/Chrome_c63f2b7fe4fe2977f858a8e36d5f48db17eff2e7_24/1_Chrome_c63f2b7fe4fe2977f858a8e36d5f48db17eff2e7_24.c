Utterance::~Utterance() {
  DCHECK_EQ(completion_task_, static_cast<Task *>(NULL));
}
