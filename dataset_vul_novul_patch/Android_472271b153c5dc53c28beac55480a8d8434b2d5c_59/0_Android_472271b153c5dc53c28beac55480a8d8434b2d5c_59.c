static reactor_status_t run_reactor(reactor_t *reactor, int iterations) {
  assert(reactor != NULL);

  reactor->run_thread = pthread_self();
  reactor->is_running = true;

 struct epoll_event events[MAX_EVENTS];
 for (int i = 0; iterations == 0 || i < iterations; ++i) {
    pthread_mutex_lock(&reactor->list_lock);
    list_clear(reactor->invalidation_list);
    pthread_mutex_unlock(&reactor->list_lock);

 
     int ret;
     do {
      ret = TEMP_FAILURE_RETRY(epoll_wait(reactor->epoll_fd, events, MAX_EVENTS, -1));
     } while (ret == -1 && errno == EINTR);
 
     if (ret == -1) {
      LOG_ERROR("%s error in epoll_wait: %s", __func__, strerror(errno));
      reactor->is_running = false;
 return REACTOR_STATUS_ERROR;
 }

 for (int j = 0; j < ret; ++j) {
 if (events[j].data.ptr == NULL) {
 eventfd_t value;
        eventfd_read(reactor->event_fd, &value);
        reactor->is_running = false;
 return REACTOR_STATUS_STOP;
 }

 reactor_object_t *object = (reactor_object_t *)events[j].data.ptr;

      pthread_mutex_lock(&reactor->list_lock);
 if (list_contains(reactor->invalidation_list, object)) {
        pthread_mutex_unlock(&reactor->list_lock);
 continue;
 }

      pthread_mutex_lock(&object->lock);
      pthread_mutex_unlock(&reactor->list_lock);

      reactor->object_removed = false;
 if (events[j].events & (EPOLLIN | EPOLLHUP | EPOLLRDHUP | EPOLLERR) && object->read_ready)
        object->read_ready(object->context);
 if (!reactor->object_removed && events[j].events & EPOLLOUT && object->write_ready)
        object->write_ready(object->context);
      pthread_mutex_unlock(&object->lock);

 if (reactor->object_removed) {
        pthread_mutex_destroy(&object->lock);
        osi_free(object);
 }
 }
 }

  reactor->is_running = false;
 return REACTOR_STATUS_DONE;
}
