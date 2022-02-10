  // put tasks in correct state
  task_table_entry_t* tasks_in_updating_state[TASK_TABLE_ID__COUNT] = {0};
  int tasks_in_updating_state_idx = 0;
  for (int i = 0; i < TASK_TABLE_ID__COUNT; i++)
  {
    for (int j = 0; j < MAX_CONFIG_IDS; j++)
    {
      if (sharedCoreMemory.task_table[i].config_ids[j] == request.cmd)
      {
        tasks_in_updating_state[tasks_in_updating_state_idx++] = &sharedCoreMemory.task_table[i];
        sharedCoreMemory.task_table[i].updating = 1;
      }
    }
  }

  // wait for all task to be in update state
  int waiting = 1;
  while(waiting) {
    waiting = 0;
    for(int i = 0; i < tasks_in_updating_state_idx; i++)
    {
      // keep waiting until all tasks are in update state
      waiting |= tasks_in_updating_state[i]->state != TASK_STATE_UPDATE;
    }
  }

    // signal to task udpate is complete
  for(int i = 0; i < tasks_in_updating_state_idx; i++)
  {
    tasks_in_updating_state[i]->updating = 0;
  }