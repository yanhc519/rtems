/*  cpu.h
 *
 *  This include file contains information pertaining to the HP
 *  PA-RISC processor (Level 1.1).
 *
 *  COPYRIGHT (c) 1994 by Division Incorporated
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.OARcorp.com/rtems/license.html.
 *
 *  $Id$
 */

#ifndef __CPU_h
#define __CPU_h

#ifdef __cplusplus
extern "C" {
#endif

#include <rtems/score/unix.h>              /* pick up machine definitions */
#ifndef ASM
#include <rtems/score/unixtypes.h>
#endif

#include <rtems/score/unixsize.h>

#if defined(solaris2)
#undef  _POSIX_C_SOURCE
#define _POSIX_C_SOURCE 3
#undef  __STRICT_ANSI__
#define __STRICT_ANSI__
#endif

#if defined(linux)
#define MALLOC_0_RETURNS_NULL
#endif

/* conditional compilation parameters */

/*
 *  Should the calls to _Thread_Enable_dispatch be inlined?
 *
 *  If TRUE, then they are inlined.
 *  If FALSE, then a subroutine call is made.
 *
 *  Basically this is an example of the classic trade-off of size
 *  versus speed.  Inlining the call (TRUE) typically increases the
 *  size of RTEMS while speeding up the enabling of dispatching.
 *  [NOTE: In general, the _Thread_Dispatch_disable_level will
 *  only be 0 or 1 unless you are in an interrupt handler and that
 *  interrupt handler invokes the executive.]  When not inlined
 *  something calls _Thread_Enable_dispatch which in turns calls
 *  _Thread_Dispatch.  If the enable dispatch is inlined, then
 *  one subroutine call is avoided entirely.]
 */

#define CPU_INLINE_ENABLE_DISPATCH       FALSE

/*
 *  Should the body of the search loops in _Thread_queue_Enqueue_priority
 *  be unrolled one time?  In unrolled each iteration of the loop examines
 *  two "nodes" on the chain being searched.  Otherwise, only one node
 *  is examined per iteration.
 *
 *  If TRUE, then the loops are unrolled.
 *  If FALSE, then the loops are not unrolled.
 *
 *  The primary factor in making this decision is the cost of disabling
 *  and enabling interrupts (_ISR_Flash) versus the cost of rest of the
 *  body of the loop.  On some CPUs, the flash is more expensive than
 *  one iteration of the loop body.  In this case, it might be desirable
 *  to unroll the loop.  It is important to note that on some CPUs, this
 *  code is the longest interrupt disable period in RTEMS.  So it is
 *  necessary to strike a balance when setting this parameter.
 */

#define CPU_UNROLL_ENQUEUE_PRIORITY      TRUE

/*
 *  Does RTEMS manage a dedicated interrupt stack in software?
 *
 *  If TRUE, then a stack is allocated in _ISR_Handler_initialization.
 *  If FALSE, nothing is done.
 *
 *  If the CPU supports a dedicated interrupt stack in hardware,
 *  then it is generally the responsibility of the BSP to allocate it
 *  and set it up.
 *
 *  If the CPU does not support a dedicated interrupt stack, then
 *  the porter has two options: (1) execute interrupts on the
 *  stack of the interrupted task, and (2) have RTEMS manage a dedicated
 *  interrupt stack.
 *
 *  If this is TRUE, CPU_ALLOCATE_INTERRUPT_STACK should also be TRUE.
 *
 *  Only one of CPU_HAS_SOFTWARE_INTERRUPT_STACK and
 *  CPU_HAS_HARDWARE_INTERRUPT_STACK should be set to TRUE.  It is
 *  possible that both are FALSE for a particular CPU.  Although it
 *  is unclear what that would imply about the interrupt processing
 *  procedure on that CPU.
 */

#define CPU_HAS_SOFTWARE_INTERRUPT_STACK FALSE

/*
 *  Does this CPU have hardware support for a dedicated interrupt stack?
 *
 *  If TRUE, then it must be installed during initialization.
 *  If FALSE, then no installation is performed.
 *
 *  If this is TRUE, CPU_ALLOCATE_INTERRUPT_STACK should also be TRUE.
 *
 *  Only one of CPU_HAS_SOFTWARE_INTERRUPT_STACK and
 *  CPU_HAS_HARDWARE_INTERRUPT_STACK should be set to TRUE.  It is
 *  possible that both are FALSE for a particular CPU.  Although it
 *  is unclear what that would imply about the interrupt processing
 *  procedure on that CPU.
 */

#define CPU_HAS_HARDWARE_INTERRUPT_STACK TRUE

/*
 *  Does RTEMS allocate a dedicated interrupt stack in the Interrupt Manager?
 *
 *  If TRUE, then the memory is allocated during initialization.
 *  If FALSE, then the memory is allocated during initialization.
 *
 *  This should be TRUE if CPU_HAS_SOFTWARE_INTERRUPT_STACK is TRUE
 *  or CPU_INSTALL_HARDWARE_INTERRUPT_STACK is TRUE.
 */

#define CPU_ALLOCATE_INTERRUPT_STACK FALSE

/*
 *  Does the RTEMS invoke the user's ISR with the vector number and
 *  a pointer to the saved interrupt frame (1) or just the vector 
 *  number (0)?
 */

#define CPU_ISR_PASSES_FRAME_POINTER 0

/*
 *  Does the CPU have hardware floating point?
 *
 *  If TRUE, then the RTEMS_FLOATING_POINT task attribute is supported.
 *  If FALSE, then the RTEMS_FLOATING_POINT task attribute is ignored.
 *
 *  If there is a FP coprocessor such as the i387 or mc68881, then
 *  the answer is TRUE.
 *
 *  The macro name "NO_CPU_HAS_FPU" should be made CPU specific.
 *  It indicates whether or not this CPU model has FP support.  For
 *  example, it would be possible to have an i386_nofp CPU model
 *  which set this to false to indicate that you have an i386 without
 *  an i387 and wish to leave floating point support out of RTEMS.
 */

#define CPU_HARDWARE_FP     TRUE

/*
 *  Are all tasks RTEMS_FLOATING_POINT tasks implicitly?
 *
 *  If TRUE, then the RTEMS_FLOATING_POINT task attribute is assumed.
 *  If FALSE, then the RTEMS_FLOATING_POINT task attribute is followed.
 *
 *  So far, the only CPU in which this option has been used is the
 *  HP PA-RISC.  The HP C compiler and gcc both implicitly use the
 *  floating point registers to perform integer multiplies.  If
 *  a function which you would not think utilize the FP unit DOES,
 *  then one can not easily predict which tasks will use the FP hardware.
 *  In this case, this option should be TRUE.
 *
 *  If CPU_HARDWARE_FP is FALSE, then this should be FALSE as well.
 */

#define CPU_ALL_TASKS_ARE_FP     FALSE

/*
 *  Should the IDLE task have a floating point context?
 *
 *  If TRUE, then the IDLE task is created as a RTEMS_FLOATING_POINT task
 *  and it has a floating point context which is switched in and out.
 *  If FALSE, then the IDLE task does not have a floating point context.
 *
 *  Setting this to TRUE negatively impacts the time required to preempt
 *  the IDLE task from an interrupt because the floating point context
 *  must be saved as part of the preemption.
 */

#define CPU_IDLE_TASK_IS_FP      FALSE

/*
 *  Should the saving of the floating point registers be deferred
 *  until a context switch is made to another different floating point
 *  task?
 *
 *  If TRUE, then the floating point context will not be stored until
 *  necessary.  It will remain in the floating point registers and not
 *  disturned until another floating point task is switched to.
 *
 *  If FALSE, then the floating point context is saved when a floating
 *  point task is switched out and restored when the next floating point
 *  task is restored.  The state of the floating point registers between
 *  those two operations is not specified.
 *
 *  If the floating point context does NOT have to be saved as part of
 *  interrupt dispatching, then it should be safe to set this to TRUE.
 *
 *  Setting this flag to TRUE results in using a different algorithm
 *  for deciding when to save and restore the floating point context.
 *  The deferred FP switch algorithm minimizes the number of times
 *  the FP context is saved and restored.  The FP context is not saved
 *  until a context switch is made to another, different FP task.
 *  Thus in a system with only one FP task, the FP context will never
 *  be saved or restored.
 */

#define CPU_USE_DEFERRED_FP_SWITCH       TRUE

/*
 *  Does this port provide a CPU dependent IDLE task implementation?
 *
 *  If TRUE, then the routine _CPU_Thread_Idle_body
 *  must be provided and is the default IDLE thread body instead of
 *  _CPU_Thread_Idle_body.
 *
 *  If FALSE, then use the generic IDLE thread body if the BSP does
 *  not provide one.
 *
 *  This is intended to allow for supporting processors which have
 *  a low power or idle mode.  When the IDLE thread is executed, then
 *  the CPU can be powered down.
 *
 *  The order of precedence for selecting the IDLE thread body is:
 *
 *    1.  BSP provided
 *    2.  CPU dependent (if provided)
 *    3.  generic (if no BSP and no CPU dependent)
 */

#define CPU_PROVIDES_IDLE_THREAD_BODY    TRUE

/*
 *  Does the stack grow up (toward higher addresses) or down
 *  (toward lower addresses)?
 *
 *  If TRUE, then the grows upward.
 *  If FALSE, then the grows toward smaller addresses.
 */

#if defined(__hppa__)
#define CPU_STACK_GROWS_UP               TRUE
#elif defined(__sparc__) || defined(__i386__)
#define CPU_STACK_GROWS_UP               FALSE
#else
#error "unknown CPU!!"
#endif


/*
 *  The following is the variable attribute used to force alignment
 *  of critical RTEMS structures.  On some processors it may make
 *  sense to have these aligned on tighter boundaries than
 *  the minimum requirements of the compiler in order to have as
 *  much of the critical data area as possible in a cache line.
 *
 *  The placement of this macro in the declaration of the variables
 *  is based on the syntactically requirements of the GNU C
 *  "__attribute__" extension.  For example with GNU C, use
 *  the following to force a structures to a 32 byte boundary.
 *
 *      __attribute__ ((aligned (32)))
 *
 *  NOTE:  Currently only the Priority Bit Map table uses this feature.
 *         To benefit from using this, the data must be heavily
 *         used so it will stay in the cache and used frequently enough
 *         in the executive to justify turning this on.
 */

#ifdef __GNUC__
#define CPU_STRUCTURE_ALIGNMENT          __attribute__ ((aligned (32)))
#else
#define CPU_STRUCTURE_ALIGNMENT
#endif

/*
 *  Define what is required to specify how the network to host conversion
 *  routines are handled.
 */

#if defined(__hppa__) || defined(__sparc__)
#define CPU_HAS_OWN_HOST_TO_NETWORK_ROUTINES     FALSE
#define CPU_BIG_ENDIAN                           TRUE
#define CPU_LITTLE_ENDIAN                        FALSE
#elif defined(__i386__)
#define CPU_HAS_OWN_HOST_TO_NETWORK_ROUTINES     FALSE
#define CPU_BIG_ENDIAN                           FALSE
#define CPU_LITTLE_ENDIAN                        TRUE
#else
#error "Unknown CPU!!!"
#endif

/*
 *  The following defines the number of bits actually used in the
 *  interrupt field of the task mode.  How those bits map to the
 *  CPU interrupt levels is defined by the routine _CPU_ISR_Set_level().
 */

#define CPU_MODES_INTERRUPT_MASK   0x00000001

#define CPU_NAME "UNIX"

/*
 *  Processor defined structures
 *
 *  Examples structures include the descriptor tables from the i386
 *  and the processor control structure on the i960ca.
 */

/* may need to put some structures here.  */

#if defined(__hppa__)
/*
 * Word indices within a jmp_buf structure
 */

#ifdef RTEMS_NEWLIB_SETJMP
#define RP_OFF       6
#define SP_OFF       2
#define R3_OFF      10
#define R4_OFF      11
#define R5_OFF      12
#define R6_OFF      13
#define R7_OFF      14
#define R8_OFF      15
#define R9_OFF      16
#define R10_OFF     17
#define R11_OFF     18
#define R12_OFF     19
#define R13_OFF     20
#define R14_OFF     21
#define R15_OFF     22
#define R16_OFF     23
#define R17_OFF     24
#define R18_OFF     25
#define DP_OFF      26
#endif

#ifdef RTEMS_UNIXLIB_SETJMP
#define RP_OFF       0
#define SP_OFF       1
#define R3_OFF       4
#define R4_OFF       5
#define R5_OFF       6
#define R6_OFF       7
#define R7_OFF       8
#define R8_OFF       9
#define R9_OFF      10
#define R10_OFF     11
#define R11_OFF     12
#define R12_OFF     13
#define R13_OFF     14
#define R14_OFF     15
#define R15_OFF     16
#define R16_OFF     17
#define R17_OFF     18
#define R18_OFF     19
#define DP_OFF      20
#endif
#endif

#if defined(__i386__)
 
#ifdef RTEMS_NEWLIB
#error "Newlib not installed"
#endif
 
/*
 *  For i386 targets
 */
 
#ifdef RTEMS_UNIXLIB
#if defined(__FreeBSD__)
#define RET_OFF    0
#define EBX_OFF    1
#define EBP_OFF    2
#define ESP_OFF    3
#define ESI_OFF    4
#define EDI_OFF    5
#elif defined(__CYGWIN__)
#define EAX_OFF    0
#define EBX_OFF    1
#define ECX_OFF    2
#define EDX_OFF    3
#define ESI_OFF    4
#define EDI_OFF    5
#define EBP_OFF    6
#define ESP_OFF    7
#define RET_OFF    8
#else
/* Linux */
#define EBX_OFF    0
#define ESI_OFF    1
#define EDI_OFF    2
#define EBP_OFF    3
#define ESP_OFF    4
#define RET_OFF    5
#endif
#endif
 
#endif
 
#if defined(__sparc__)

/*
 *  Word indices within a jmp_buf structure
 */
 
#ifdef RTEMS_NEWLIB
#define ADDR_ADJ_OFFSET -8
#define SP_OFF    0
#define RP_OFF    1
#define FP_OFF    2
#endif

#ifdef RTEMS_UNIXLIB
#define ADDR_ADJ_OFFSET 0
#define G0_OFF    0
#define SP_OFF    1
#define RP_OFF    2   
#define FP_OFF    3
#define I7_OFF    4
#endif

#endif

/*
 * Contexts
 *
 *  Generally there are 2 types of context to save.
 *     1. Interrupt registers to save
 *     2. Task level registers to save
 *
 *  This means we have the following 3 context items:
 *     1. task level context stuff::  Context_Control
 *     2. floating point task stuff:: Context_Control_fp
 *     3. special interrupt level context :: Context_Control_interrupt
 *
 *  On some processors, it is cost-effective to save only the callee
 *  preserved registers during a task context switch.  This means
 *  that the ISR code needs to save those registers which do not
 *  persist across function calls.  It is not mandatory to make this
 *  distinctions between the caller/callee saves registers for the
 *  purpose of minimizing context saved during task switch and on interrupts.
 *  If the cost of saving extra registers is minimal, simplicity is the
 *  choice.  Save the same context on interrupt entry as for tasks in
 *  this case.
 *
 *  Additionally, if gdb is to be made aware of RTEMS tasks for this CPU, then
 *  care should be used in designing the context area.
 *
 *  On some CPUs with hardware floating point support, the Context_Control_fp
 *  structure will not be used or it simply consist of an array of a
 *  fixed number of bytes.   This is done when the floating point context
 *  is dumped by a "FP save context" type instruction and the format
 *  is not really defined by the CPU.  In this case, there is no need
 *  to figure out the exact format -- only the size.  Of course, although
 *  this is enough information for RTEMS, it is probably not enough for
 *  a debugger such as gdb.  But that is another problem.
 */

/*
 *  This is really just the area for the following fields.
 *
 *    jmp_buf    regs;
 *    unsigned32 isr_level;
 *
 *  Doing it this way avoids conflicts between the native stuff and the
 *  RTEMS stuff.
 *
 *  NOTE:
 *      hpux9 setjmp is optimized for the case where the setjmp buffer
 *      is 8 byte aligned.  In a RISC world, this seems likely to enable
 *      8 byte copies, especially for the float registers.
 *      So we always align them on 8 byte boundaries.
 */

#ifdef __GNUC__
#define CONTEXT_STRUCTURE_ALIGNMENT          __attribute__ ((aligned (8)))
#else
#define CONTEXT_STRUCTURE_ALIGNMENT
#endif

typedef struct {
  char      Area[ CPU_CONTEXT_SIZE_IN_BYTES ] CONTEXT_STRUCTURE_ALIGNMENT;
} Context_Control;

typedef struct {
} Context_Control_fp;

typedef struct {
} CPU_Interrupt_frame;


/*
 *  The following table contains the information required to configure
 *  the UNIX Simulator specific parameters.
 */

typedef struct {
  void       (*pretasking_hook)( void );
  void       (*predriver_hook)( void );
  void       (*postdriver_hook)( void );
  void       (*idle_task)( void );
  boolean      do_zero_of_workspace;
  unsigned32   idle_task_stack_size;
  unsigned32   interrupt_stack_size;
  unsigned32   extra_mpci_receive_server_stack;
  void *     (*stack_allocate_hook)( unsigned32 );
  void       (*stack_free_hook)( void* );
  /* end of required fields */
}   rtems_cpu_table;

/*
 *  Macros to access required entires in the CPU Table are in 
 *  the file rtems/system.h.
 */

/*
 *  Macros to access UNIX specific additions to the CPU Table
 */

/* There are no CPU specific additions to the CPU Table for this port. */

/*
 *  This variable is optional.  It is used on CPUs on which it is difficult
 *  to generate an "uninitialized" FP context.  It is filled in by
 *  _CPU_Initialize and copied into the task's FP context area during
 *  _CPU_Context_Initialize.
 */

SCORE_EXTERN Context_Control_fp  _CPU_Null_fp_context;

/*
 *  On some CPUs, RTEMS supports a software managed interrupt stack.
 *  This stack is allocated by the Interrupt Manager and the switch
 *  is performed in _ISR_Handler.  These variables contain pointers
 *  to the lowest and highest addresses in the chunk of memory allocated
 *  for the interrupt stack.  Since it is unknown whether the stack
 *  grows up or down (in general), this give the CPU dependent
 *  code the option of picking the version it wants to use.
 *
 *  NOTE: These two variables are required if the macro
 *        CPU_HAS_SOFTWARE_INTERRUPT_STACK is defined as TRUE.
 */

SCORE_EXTERN void               *_CPU_Interrupt_stack_low;
SCORE_EXTERN void               *_CPU_Interrupt_stack_high;

/*
 *  With some compilation systems, it is difficult if not impossible to
 *  call a high-level language routine from assembly language.  This
 *  is especially true of commercial Ada compilers and name mangling
 *  C++ ones.  This variable can be optionally defined by the CPU porter
 *  and contains the address of the routine _Thread_Dispatch.  This
 *  can make it easier to invoke that routine at the end of the interrupt
 *  sequence (if a dispatch is necessary).
 */

SCORE_EXTERN void           (*_CPU_Thread_dispatch_pointer)();

/*
 *  Nothing prevents the porter from declaring more CPU specific variables.
 */

/* XXX: if needed, put more variables here */

/*
 *  The size of the floating point context area.  On some CPUs this
 *  will not be a "sizeof" because the format of the floating point
 *  area is not defined -- only the size is.  This is usually on
 *  CPUs with a "floating point save context" instruction.
 */

#define CPU_CONTEXT_FP_SIZE sizeof( Context_Control_fp )

/*
 * The size of a frame on the stack
 */

#if defined(__hppa__)
#define CPU_FRAME_SIZE  (32 * 4)
#elif defined(__sparc__)
#define CPU_FRAME_SIZE  (112)   /* based on disassembled test code */
#elif defined(__i386__)
#define CPU_FRAME_SIZE  (24)  /* return address, sp, and bp pushed plus fudge */
#else
#error "Unknown CPU!!!"
#endif

/*
 *  Amount of extra stack (above minimum stack size) required by
 *  MPCI receive server thread.  Remember that in a multiprocessor
 *  system this thread must exist and be able to process all directives.
 */

#define CPU_MPCI_RECEIVE_SERVER_EXTRA_STACK 0

/*
 *  This defines the number of entries in the ISR_Vector_table managed
 *  by RTEMS.
 */

#define CPU_INTERRUPT_NUMBER_OF_VECTORS      64
#define CPU_INTERRUPT_MAXIMUM_VECTOR_NUMBER  (CPU_INTERRUPT_NUMBER_OF_VECTORS - 1)

/*
 *  Should be large enough to run all RTEMS tests.  This insures
 *  that a "reasonable" small application should not have any problems.
 */

#define CPU_STACK_MINIMUM_SIZE          (16 * 1024)

/*
 *  CPU's worst alignment requirement for data types on a byte boundary.  This
 *  alignment does not take into account the requirements for the stack.
 */

#define CPU_ALIGNMENT              8

/*
 *  This number corresponds to the byte alignment requirement for the
 *  heap handler.  This alignment requirement may be stricter than that
 *  for the data types alignment specified by CPU_ALIGNMENT.  It is
 *  common for the heap to follow the same alignment requirement as
 *  CPU_ALIGNMENT.  If the CPU_ALIGNMENT is strict enough for the heap,
 *  then this should be set to CPU_ALIGNMENT.
 *
 *  NOTE:  This does not have to be a power of 2.  It does have to
 *         be greater or equal to than CPU_ALIGNMENT.
 */

#define CPU_HEAP_ALIGNMENT         CPU_ALIGNMENT

/*
 *  This number corresponds to the byte alignment requirement for memory
 *  buffers allocated by the partition manager.  This alignment requirement
 *  may be stricter than that for the data types alignment specified by
 *  CPU_ALIGNMENT.  It is common for the partition to follow the same
 *  alignment requirement as CPU_ALIGNMENT.  If the CPU_ALIGNMENT is strict
 *  enough for the partition, then this should be set to CPU_ALIGNMENT.
 *
 *  NOTE:  This does not have to be a power of 2.  It does have to
 *         be greater or equal to than CPU_ALIGNMENT.
 */

#define CPU_PARTITION_ALIGNMENT    CPU_ALIGNMENT

/*
 *  This number corresponds to the byte alignment requirement for the
 *  stack.  This alignment requirement may be stricter than that for the
 *  data types alignment specified by CPU_ALIGNMENT.  If the CPU_ALIGNMENT
 *  is strict enough for the stack, then this should be set to 0.
 *
 *  NOTE:  This must be a power of 2 either 0 or greater than CPU_ALIGNMENT.
 */

#define CPU_STACK_ALIGNMENT        64

/* ISR handler macros */

/*
 *  Disable all interrupts for an RTEMS critical section.  The previous
 *  level is returned in _level.
 */

extern unsigned32 _CPU_ISR_Disable_support(void);

#define _CPU_ISR_Disable( _level ) \
    do { \
      (_level) = _CPU_ISR_Disable_support(); \
    } while ( 0 )

/*
 *  Enable interrupts to the previous level (returned by _CPU_ISR_Disable).
 *  This indicates the end of an RTEMS critical section.  The parameter
 *  _level is not modified.
 */

void _CPU_ISR_Enable(unsigned32 level);

/*
 *  This temporarily restores the interrupt to _level before immediately
 *  disabling them again.  This is used to divide long RTEMS critical
 *  sections into two or more parts.  The parameter _level is not
 * modified.
 */

#define _CPU_ISR_Flash( _level ) \
  do { \
      register unsigned32 _ignored = 0; \
      _CPU_ISR_Enable( (_level) ); \
      _CPU_ISR_Disable( _ignored ); \
  } while ( 0 )

/*
 *  Map interrupt level in task mode onto the hardware that the CPU
 *  actually provides.  Currently, interrupt levels which do not
 *  map onto the CPU in a generic fashion are undefined.  Someday,
 *  it would be nice if these were "mapped" by the application
 *  via a callout.  For example, m68k has 8 levels 0 - 7, levels
 *  8 - 255 would be available for bsp/application specific meaning.
 *  This could be used to manage a programmable interrupt controller
 *  via the rtems_task_mode directive.
 */

#define _CPU_ISR_Set_level( new_level ) \
  { \
    if ( new_level == 0 ) _CPU_ISR_Enable( 0 ); \
    else                  _CPU_ISR_Enable( 1 ); \
  }

unsigned32 _CPU_ISR_Get_level( void );

/* end of ISR handler macros */

/* Context handler macros */

/*
 *  This routine is responsible for somehow restarting the currently
 *  executing task.  If you are lucky, then all that is necessary
 *  is restoring the context.  Otherwise, there will need to be
 *  a special assembly routine which does something special in this
 *  case.  Context_Restore should work most of the time.  It will
 *  not work if restarting self conflicts with the stack frame
 *  assumptions of restoring a context.
 */

#define _CPU_Context_Restart_self( _the_context ) \
   _CPU_Context_restore( (_the_context) );

/*
 *  The purpose of this macro is to allow the initial pointer into
 *  a floating point context area (used to save the floating point
 *  context) to be at an arbitrary place in the floating point
 *  context area.
 *
 *  This is necessary because some FP units are designed to have
 *  their context saved as a stack which grows into lower addresses.
 *  Other FP units can be saved by simply moving registers into offsets
 *  from the base of the context area.  Finally some FP units provide
 *  a "dump context" instruction which could fill in from high to low
 *  or low to high based on the whim of the CPU designers.
 */

#define _CPU_Context_Fp_start( _base, _offset ) \
   ( (void *) _Addresses_Add_offset( (_base), (_offset) ) )

/*
 *  This routine initializes the FP context area passed to it to.
 *  There are a few standard ways in which to initialize the
 *  floating point context.  The code included for this macro assumes
 *  that this is a CPU in which a "initial" FP context was saved into
 *  _CPU_Null_fp_context and it simply copies it to the destination
 *  context passed to it.
 *
 *  Other models include (1) not doing anything, and (2) putting
 *  a "null FP status word" in the correct place in the FP context.
 */

#define _CPU_Context_Initialize_fp( _destination ) \
  { \
   *((Context_Control_fp *) *((void **) _destination)) = _CPU_Null_fp_context; \
  }

#define _CPU_Context_save_fp( _fp_context ) \
    _CPU_Save_float_context( *(Context_Control_fp **)(_fp_context))

#define _CPU_Context_restore_fp( _fp_context ) \
    _CPU_Restore_float_context( *(Context_Control_fp **)(_fp_context))

extern void _CPU_Context_Initialize(
  Context_Control  *_the_context,
  unsigned32       *_stack_base,
  unsigned32        _size,
  unsigned32        _new_level,
  void             *_entry_point,
  boolean           _is_fp
);

/* end of Context handler macros */

/* Fatal Error manager macros */

/*
 *  This routine copies _error into a known place -- typically a stack
 *  location or a register, optionally disables interrupts, and
 *  halts/stops the CPU.
 */

#define _CPU_Fatal_halt( _error ) \
    _CPU_Fatal_error( _error )

/* end of Fatal Error manager macros */

/* Bitfield handler macros */

/*
 *  This routine sets _output to the bit number of the first bit
 *  set in _value.  _value is of CPU dependent type Priority_Bit_map_control.
 *  This type may be either 16 or 32 bits wide although only the 16
 *  least significant bits will be used.
 *
 *  There are a number of variables in using a "find first bit" type
 *  instruction.
 *
 *    (1) What happens when run on a value of zero?
 *    (2) Bits may be numbered from MSB to LSB or vice-versa.
 *    (3) The numbering may be zero or one based.
 *    (4) The "find first bit" instruction may search from MSB or LSB.
 *
 *  RTEMS guarantees that (1) will never happen so it is not a concern.
 *  (2),(3), (4) are handled by the macros _CPU_Priority_mask() and
 *  _CPU_Priority_bits_index().  These three form a set of routines
 *  which must logically operate together.  Bits in the _value are
 *  set and cleared based on masks built by _CPU_Priority_mask().
 *  The basic major and minor values calculated by _Priority_Major()
 *  and _Priority_Minor() are "massaged" by _CPU_Priority_bits_index()
 *  to properly range between the values returned by the "find first bit"
 *  instruction.  This makes it possible for _Priority_Get_highest() to
 *  calculate the major and directly index into the minor table.
 *  This mapping is necessary to ensure that 0 (a high priority major/minor)
 *  is the first bit found.
 *
 *  This entire "find first bit" and mapping process depends heavily
 *  on the manner in which a priority is broken into a major and minor
 *  components with the major being the 4 MSB of a priority and minor
 *  the 4 LSB.  Thus (0 << 4) + 0 corresponds to priority 0 -- the highest
 *  priority.  And (15 << 4) + 14 corresponds to priority 254 -- the next
 *  to the lowest priority.
 *
 *  If your CPU does not have a "find first bit" instruction, then
 *  there are ways to make do without it.  Here are a handful of ways
 *  to implement this in software:
 *
 *    - a series of 16 bit test instructions
 *    - a "binary search using if's"
 *    - _number = 0
 *      if _value > 0x00ff
 *        _value >>=8
 *        _number = 8;
 *
 *      if _value > 0x0000f
 *        _value >=8
 *        _number += 4
 *
 *      _number += bit_set_table[ _value ]
 *
 *    where bit_set_table[ 16 ] has values which indicate the first
 *      bit set
 */

/*
 *  The UNIX port uses the generic C algorithm for bitfield scan to avoid
 *  dependencies on either a native bitscan instruction or an ffs() in the
 *  C library.
 */
 
#define CPU_USE_GENERIC_BITFIELD_CODE TRUE
#define CPU_USE_GENERIC_BITFIELD_DATA TRUE
 
/* end of Bitfield handler macros */
 
/* Priority handler handler macros */
 
/*
 *  The UNIX port uses the generic C algorithm for bitfield scan to avoid
 *  dependencies on either a native bitscan instruction or an ffs() in the
 *  C library.
 */
 
/* end of Priority handler macros */

/* functions */

/*
 *  _CPU_Initialize
 *
 *  This routine performs CPU dependent initialization.
 */

void _CPU_Initialize(
  rtems_cpu_table  *cpu_table,
  void      (*thread_dispatch)
);

/*
 *  _CPU_ISR_install_raw_handler
 *
 *  This routine installs a "raw" interrupt handler directly into the 
 *  processor's vector table.
 */
 
void _CPU_ISR_install_raw_handler(
  unsigned32  vector,
  proc_ptr    new_handler,
  proc_ptr   *old_handler
);

/*
 *  _CPU_ISR_install_vector
 *
 *  This routine installs an interrupt vector.
 */

void _CPU_ISR_install_vector(
  unsigned32  vector,
  proc_ptr    new_handler,
  proc_ptr   *old_handler
);

/*
 *  _CPU_Install_interrupt_stack
 *
 *  This routine installs the hardware interrupt stack pointer.
 *
 *  NOTE:  It need only be provided if CPU_HAS_HARDWARE_INTERRUPT_STACK
 *         is TRUE.
 */

void _CPU_Install_interrupt_stack( void );

/*
 *  _CPU_Thread_Idle_body
 *
 *  This routine is the CPU dependent IDLE thread body.
 *
 *  NOTE:  It need only be provided if CPU_PROVIDES_IDLE_THREAD_BODY
 *         is TRUE.
 */

void _CPU_Thread_Idle_body( void );

/*
 *  _CPU_Context_switch
 *
 *  This routine switches from the run context to the heir context.
 */

void _CPU_Context_switch(
  Context_Control  *run,
  Context_Control  *heir
);

/*
 *  _CPU_Context_restore
 *
 *  This routine is generally used only to restart self in an
 *  efficient manner.  It may simply be a label in _CPU_Context_switch.
 *
 *  NOTE: May be unnecessary to reload some registers.
 */

void _CPU_Context_restore(
  Context_Control *new_context
);

/*
 *  _CPU_Save_float_context
 *
 *  This routine saves the floating point context passed to it.
 */

void _CPU_Save_float_context(
  Context_Control_fp *fp_context_ptr
);

/*
 *  _CPU_Restore_float_context
 *
 *  This routine restores the floating point context passed to it.
 */

void _CPU_Restore_float_context(
  Context_Control_fp *fp_context_ptr
);


void _CPU_ISR_Set_signal_level(
  unsigned32 level
);

void _CPU_Fatal_error(
  unsigned32 _error
);

/*  The following routine swaps the endian format of an unsigned int.
 *  It must be static because it is referenced indirectly.
 *
 *  This version will work on any processor, but if there is a better
 *  way for your CPU PLEASE use it.  The most common way to do this is to:
 *
 *     swap least significant two bytes with 16-bit rotate
 *     swap upper and lower 16-bits
 *     swap most significant two bytes with 16-bit rotate
 *
 *  Some CPUs have special instructions which swap a 32-bit quantity in
 *  a single instruction (e.g. i486).  It is probably best to avoid
 *  an "endian swapping control bit" in the CPU.  One good reason is
 *  that interrupts would probably have to be disabled to insure that
 *  an interrupt does not try to access the same "chunk" with the wrong
 *  endian.  Another good reason is that on some CPUs, the endian bit
 *  endianness for ALL fetches -- both code and data -- so the code
 *  will be fetched incorrectly.
 */
 
static inline unsigned int CPU_swap_u32(
  unsigned int value
)
{
  unsigned32 byte1, byte2, byte3, byte4, swapped;
 
  byte4 = (value >> 24) & 0xff;
  byte3 = (value >> 16) & 0xff;
  byte2 = (value >> 8)  & 0xff;
  byte1 =  value        & 0xff;
 
  swapped = (byte1 << 24) | (byte2 << 16) | (byte3 << 8) | byte4;
  return( swapped );
}

#define CPU_swap_u16( value ) \
  (((value&0xff) << 8) | ((value >> 8)&0xff))

/*
 *  Special Purpose Routines to hide the use of UNIX system calls.
 */


/*
 *  Pointer to a sync io  Handler
 */

typedef void ( *rtems_sync_io_handler )(
  int fd,
  boolean read,
  boolean wrtie,
  boolean except
);

/* returns -1 if fd to large, 0 is successful */
int _CPU_Set_sync_io_handler(
  int fd,
  boolean read,
  boolean write,
  boolean except,
  rtems_sync_io_handler handler
);

/* returns -1 if fd to large, o if successful */
int _CPU_Clear_sync_io_handler(
  int fd
);

int _CPU_Get_clock_vector( void );

void _CPU_Start_clock( 
  int microseconds
);

void _CPU_Stop_clock( void );

#if defined(RTEMS_MULTIPROCESSING)

void _CPU_SHM_Init( 
  unsigned32   maximum_nodes,
  boolean      is_master_node,
  void       **shm_address,
  unsigned32  *shm_length
);

int _CPU_Get_pid( void );
 
int _CPU_SHM_Get_vector( void );
 
void _CPU_SHM_Send_interrupt(
  int pid,
  int vector
);
 
void _CPU_SHM_Lock( 
  int semaphore
);

void _CPU_SHM_Unlock(
  int semaphore
);
#endif

#ifdef __cplusplus
}
#endif

#endif
