/* This is not a particular application, it is only meant to see if the
 * core compiles. Most options are switched on. */

#include "femtoos_code.h"

#if (callAppBoot == cfgTrue)
  void appBoot(void) { }
#endif


#if (callAppTick00 == cfgTrue)
  void appTick00(void) { }
#endif


#if (callAppTick08 == cfgTrue)
  void appTick08(void) { }
#endif


#if (callAppTick16 == cfgTrue)
  void appTick6(void) { }
#endif


#if (preTaskDefined(task0))

  #if (callAppInit == cfgTrue)
    void Init_task0(void) { }
  #endif

  void appLoop_task0(void)
  { genFireEvent(7);
    while (true); }

  #if (cfgUseTaskWatchdog == cfgTrue) && (callAppBark == cfgTrue)
    void appBark_task0(void) { }
  #endif

#endif

