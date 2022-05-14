 error::Error GLES2DecoderPassthroughImpl::ProcessQueries(bool did_finish) {
  bool program_completion_query_deferred = false;
   while (!pending_queries_.empty()) {
    PendingQuery& query = pending_queries_.front();
     GLuint result_available = GL_FALSE;
     GLuint64 result = 0;
     switch (query.target) {
      case GL_COMMANDS_COMPLETED_CHROMIUM:
        DCHECK(query.commands_completed_fence != nullptr);
        result_available =
            did_finish || query.commands_completed_fence->HasCompleted();
        result = result_available;
        break;

      case GL_COMMANDS_ISSUED_CHROMIUM:
        result_available = GL_TRUE;
        result = GL_TRUE;
        break;

      case GL_LATENCY_QUERY_CHROMIUM:
        result_available = GL_TRUE;
        result = (base::TimeTicks::Now() - base::TimeTicks()).InMilliseconds();
        break;

      case GL_ASYNC_PIXEL_PACK_COMPLETED_CHROMIUM:
        result_available = GL_TRUE;
        result = GL_TRUE;
        for (const PendingReadPixels& pending_read_pixels :
             pending_read_pixels_) {
          if (pending_read_pixels.waiting_async_pack_queries.count(
                  query.service_id) > 0) {
            DCHECK(!did_finish);
            result_available = GL_FALSE;
            result = GL_FALSE;
            break;
          }
        }
        break;

      case GL_READBACK_SHADOW_COPIES_UPDATED_CHROMIUM:
        DCHECK(query.buffer_shadow_update_fence);
        if (did_finish || query.buffer_shadow_update_fence->HasCompleted()) {
          ReadBackBuffersIntoShadowCopies(query.buffer_shadow_updates);
          result_available = GL_TRUE;
          result = 0;
        }
        break;

      case GL_GET_ERROR_QUERY_CHROMIUM:
        result_available = GL_TRUE;
        FlushErrors();
         result = PopError();
         break;
 
      case GL_PROGRAM_COMPLETION_QUERY_CHROMIUM:
        GLint status;
        if (!api()->glIsProgramFn(query.program_service_id)) {
          status = GL_TRUE;
        } else {
          api()->glGetProgramivFn(query.program_service_id,
                                  GL_COMPLETION_STATUS_KHR, &status);
        }
        result_available = (status == GL_TRUE);
        if (!result_available) {
          // Move the query to the end of queue, so that other queries may have
          // chance to be processed.
          auto temp = std::move(query);
          pending_queries_.pop_front();
          pending_queries_.emplace_back(std::move(temp));
          if (did_finish && !OnlyHasPendingProgramCompletionQueries()) {
            continue;
          } else {
            program_completion_query_deferred = true;
          }
        }
        result = 0;
        break;

       default:
         DCHECK(!IsEmulatedQueryTarget(query.target));
         if (did_finish) {
          result_available = GL_TRUE;
        } else {
          api()->glGetQueryObjectuivFn(
              query.service_id, GL_QUERY_RESULT_AVAILABLE, &result_available);
        }
        if (result_available == GL_TRUE) {
          if (feature_info_->feature_flags().ext_disjoint_timer_query) {
            api()->glGetQueryObjectui64vFn(query.service_id, GL_QUERY_RESULT,
                                           &result);
          } else {
            GLuint temp_result = 0;
            api()->glGetQueryObjectuivFn(query.service_id, GL_QUERY_RESULT,
                                         &temp_result);
            result = temp_result;
          }
        }
        break;
    }

    if (!result_available) {
      break;
    }

    query.sync->result = result;
    base::subtle::Release_Store(&query.sync->process_count, query.submit_count);
    pending_queries_.pop_front();
  }
 
  DCHECK(!did_finish || pending_queries_.empty() ||
         program_completion_query_deferred);
   return error::kNoError;
 }
