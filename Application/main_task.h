#ifndef __MAIN_TASK_H
#define __MAIN_TASK_H

#if defined(__cplusplus)
extern "C" {
#endif

extern void StartImuAcquire(void * argument);
extern void StartFaultMonitor(void * argument);
extern void StartStorage(void * argument);

#if defined(__cplusplus)
}
#endif

#endif
