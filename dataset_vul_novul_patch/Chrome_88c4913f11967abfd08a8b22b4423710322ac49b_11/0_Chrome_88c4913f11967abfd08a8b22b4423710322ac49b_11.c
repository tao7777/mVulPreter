     void ping()
     {
        m_mainThreadProxy->postTask(createMainThreadTask(this, &PingPongTestUsingTasks::pong));
         hit = true;
     }
