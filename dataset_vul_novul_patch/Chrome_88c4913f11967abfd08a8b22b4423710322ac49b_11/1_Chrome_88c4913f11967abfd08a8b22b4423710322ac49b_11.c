     void ping()
     {
        CCMainThread::postTask(createMainThreadTask(this, &PingPongTestUsingTasks::pong));
         hit = true;
     }
