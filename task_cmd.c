/*
 *  COPYRIGHT (c) 1989-2007.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 * 
 */

#include <stdio.h>
#include <stdlib.h>

#include <bsp.h>

rtems_id   Task_id[ 5 ];         /* array of task ids */
rtems_name Task_name[ 5 ];       /* array of task names */

static inline uint32_t get_ticks_per_second( void )
{
  rtems_interval ticks_per_second;
  (void) rtems_clock_get( RTEMS_CLOCK_GET_TICKS_PER_SECOND, &ticks_per_second );  return ticks_per_second;
}

rtems_task Test_task(
  rtems_task_argument unused
)
{
  int               i;
  rtems_id          tid;
  rtems_status_code status;

  status = rtems_task_ident( RTEMS_SELF, RTEMS_SEARCH_ALL_NODES, &tid );
  for (i = 0; i < 10 ; i++ ) 
  {
    status = rtems_task_wake_after( 1 * get_ticks_per_second() );
  }
  printf( "*** TASK 0x%08X FINISHED ***\n",tid);
  status = rtems_task_delete( RTEMS_SELF );    /* should not return */
}

int task_command( int argc, char *argv[])
{
  rtems_status_code status;

  printf( "\n\n*** TASKING DEMO ***\n" );
  printf( "\n\n  Create 5 tasks that will run for 10 seconds\n" );

  Task_name[ 0 ] = rtems_build_name( 'T', 'A', '1', ' ' );
  Task_name[ 1 ] = rtems_build_name( 'T', 'A', '2', ' ' );
  Task_name[ 2 ] = rtems_build_name( 'T', 'A', '3', ' ' );
  Task_name[ 3 ] = rtems_build_name( 'T', 'A', '4', ' ' );
  Task_name[ 4 ] = rtems_build_name( 'T', 'A', '5', ' ' );

  status = rtems_task_create(
    Task_name[ 0 ], 1, RTEMS_MINIMUM_STACK_SIZE * 2, RTEMS_DEFAULT_MODES,
    RTEMS_DEFAULT_ATTRIBUTES, &Task_id[ 0 ]
  );
  status = rtems_task_create(
    Task_name[ 1 ], 2, RTEMS_MINIMUM_STACK_SIZE * 2, RTEMS_DEFAULT_MODES,
    RTEMS_DEFAULT_ATTRIBUTES, &Task_id[ 1 ]
  );
  status = rtems_task_create(
    Task_name[ 2 ], 3, RTEMS_MINIMUM_STACK_SIZE * 2, RTEMS_DEFAULT_MODES,
    RTEMS_DEFAULT_ATTRIBUTES, &Task_id[ 2 ]
  );
  status = rtems_task_create(
    Task_name[ 3 ], 4, RTEMS_MINIMUM_STACK_SIZE * 2, RTEMS_DEFAULT_MODES,
    RTEMS_DEFAULT_ATTRIBUTES, &Task_id[ 3 ]
  );
  status = rtems_task_create(
    Task_name[ 4 ], 5, RTEMS_MINIMUM_STACK_SIZE * 2, RTEMS_DEFAULT_MODES,
    RTEMS_DEFAULT_ATTRIBUTES, &Task_id[ 4 ]
  );

  status = rtems_task_start( Task_id[ 0 ], Test_task, 0 );
  status = rtems_task_start( Task_id[ 1 ], Test_task, 1 );
  status = rtems_task_start( Task_id[ 2 ], Test_task, 2 );
  status = rtems_task_start( Task_id[ 3 ], Test_task, 3 );
  status = rtems_task_start( Task_id[ 4 ], Test_task, 4 );

  return(0);
}


