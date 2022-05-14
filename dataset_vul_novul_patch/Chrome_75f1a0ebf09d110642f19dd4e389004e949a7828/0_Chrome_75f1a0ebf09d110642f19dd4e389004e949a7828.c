 Player* DocumentTimeline::play(TimedItem* child)
 {
     Player* player = createPlayer(child);
    player->setStartTime(currentTime(), false);
     return player;
 }
