/*
main cheat on the kernel kernel modules and on kernel concepts
that can be exemplified in modules (much easier than recompiling and reinstalling the kernel)

#TODO

	#__user

		userspace values

	#__rcu

		a type of locking directive
*/

#include <linux/dcache.h>	/* dentry, super_block */
#include <linux/fs_struct.h>	/* fs_struct */
#include <linux/fs.h>		/* super_block */
#include <linux/interrupt.h> 	/* request_irq, IRQF_SHARED */
#include <linux/kernel.h>	/* KERN_INFO */
#include <linux/module.h>	/* module specific utilities: MODULE_* macros, module_param, module_init, module exit */
#include <linux/path.h>		/* path */
#include <linux/sched.h>	/* current */
#include <linux/sched/rt.h>	/* MAX_PRIO, MAX_USER_RT_PRIO, DEFAULT_PRIO */
#include <linux/spinlock.h>
#include <linux/string.h>	/* memcpy, memcmp */
#include <linux/version.h>
#include <linux/slab.h>

#define MODID __FILE__ ": "
#define INFO_ID KERN_INFO MODID

/*
#module description

##license

the kernel offers methods to indicate the license of modules or parts of modules
such as symbols

for entire modules, the `MODULE_LICENSE` macro can be used. Possible args:

- "GPL v2" (for GPL version two only)
- "GPL and additional rights"
- "Dual BSD/GPL"
- "Dual MPL/GPL"
- "Proprietary"

where `Dual` means that developers can either use BSD or GPL

If you don't set this, it is taken to be `Proprietary` by default,
'tainting' the module and the kernel,
so always define this if your module is not proprietary.
*/

MODULE_LICENSE("GPL");
MODULE_AUTHOR("John Smith <john.smith@mail.com>");
MODULE_DESCRIPTION("a cheat module");
MODULE_VERSION("0.1");
/*MODULE_DEVICE_TABLE(table_info);*/
/*MODULE_ALIAS("cheat2");*/

/*
#parameters

parameters an be passed to modules at insertion time:

- via command line arguments for `insmod` and `modprobe`
- via conf files in `/etc/modprobe.d/` for `modprobe` only

they are used from inside the program via the module_param macro.

if no vaule is given for them, they remain unchanged

the following types are supported:

- bool
- invbool (returns the negation of bool)
- charp: character pointer
- int, long, short, uint, ulong, ushort

the third is the file permissions for the module representation under `/sys/module/<name>/parameters`:

- `S_IRUGO`: readonly
- `S_IRUGO|S_IWUSR`: writeable by sudo

try:

	cat /sys/module/<thismodule>/parameters/param_i
*/

//declartion and default values of parameters:
static int param_i = 0;
static char *param_s = "aaa";
//static int param_is[] = {0,0,0};

//set values if given:
module_param(param_i, int, S_IRUGO);
module_param(param_s, charp, S_IRUGO);
/* TODO BUG this generates a NULL dereference and kernel oops:*/
/*module_param_array(param_is, int, 3, S_IRUGO);*/

/*
#export symbols

	when the a module gets inserted in to the kernel, it can see symbols defined in the kernel

	however other modules cannot see the symbols defined in a module unless you explicitly
	export them. This can be done with `EXPORT_SYMBOL` and its `GPL` version
*/

static int i_global;

int exported_symbol;
int exported_symbol_gpl;

EXPORT_SYMBOL(exported_symbol);
EXPORT_SYMBOL_GPL(exported_symbol_gpl);

//must be global:
DEFINE_PER_CPU(int, cpu_int);

/*
 * this function is defined as the entry point by the `module_init` call below.
 *
 * using `init_module` as name also worked. TODO why
 *
 * static is not mandatory, but good practice since this function should not be seen
 * from other files.
 *
 * typical things a real module would do here include:
 *
 * - initialize variables
 * - register an interrupt handler
 * - register a the bottom half of the interrupt handler
 *
 * #__init
 *
 * 	tells the compiler that this function is only used once at initialization,
 * 	so the kernel may free up its memory after using this function
 *
 * return value: a non 0 return means init_module failed; module can't be loaded.
 *
 * #__initdata TODO
 * #__initconst TODO
 * #__devinit TODO
 *
 * 	they are used as:
 *
 * 		static int init_variable __initdata = 0;
 * 		static const char linux_logo[] __initconst = { 0x32, 0x36, ... };
 *
 * but what do they do?
 *
 * #cleanup
 *
 * 	module insertion forget to nicely cleanup in case 
 *
 * */
static int __init init(void)
{
	i_global = 0;
	printk(INFO_ID "i_global = %d\n", i_global);

	/*
	#printk

		the kernel has no simple way to communicate with a terminal
		so you the simplest thing to do is dump program output to a file

		printk does this in a very reliable manner

		at the time of writting on Ubuntu 13.04 the file is: `/var/log/syslog`

		`KERN_INFO` is a message priority string macro

		it is understood by printk when put at the beginning of the input string

		8 levels are defined:

		printk takes printf format strings with containing things like `%d`
	*/
	{
		printk(INFO_ID "%s\n", __func__ );
	}

	/*
	#kernel version

		device drivers depend on kernel version.

		you can get some version flexibility with the preprocessor.

		#LINUX_VERSION_CODE

			example: on kernel `2.6.10` == 132618 (i.e., 0x02060a)
	*/
	{
		/* printk( "UTS_RELEASE = %s", UTS_RELEASE ); */	/* TODO get working */
		printk(INFO_ID "LINUX_VERSION_CODE = %d\n", LINUX_VERSION_CODE);
	}

	/*
	#assembly instructions that only kernel code can do

		some instructions require kernel priviledge to be used

		those that can be demonstrated here shall be

		the use of plain assembly should be avoided whenever possible,
		since more portable alternatives have usually already been coded,
		but understanding those instructions may give you insights
		on how the system achieves certain effects.

		#x86

			- interrupt flag IF instruction

				determines if interrupts are enabled or disabled

			- IO instructions

				- IN            Read from a port
				- OUT           Write to a port
				- INS/INSB      Input string from port/Input byte string from port
				- INS/INSW      Input string from port/Input word string from port
				- INS/INSD      Input string from port/Input doubleword string from port
				- OUTS/OUTSB    Output string to port/Output byte string to port
				- OUTS/OUTSW    Output string to port/Output word string to port
				- OUTS/OUTSD    Output string to port/Output doubleword string to port
	*/

#ifdef __i386__

	/*
	cheat on instructions that can only be done from kernel space

	in the kernel, those are be separated from non architecture specific files
	*/

	//TODO how to get cr0?

		//int out = 0;
		//asm (
		//	"mov %%cr0, %0"
		//	: "=m" (out)
		//);
		//printk( "%d", out );
#endif

	/*
	#fixed size integers

		Like c99 `int32_t` family

		u for unsigned, s for signed.

		Defined in `include/linux/types.h`.

	#fixed size endieness

		For cases were big or little endieness must be explicit

		Defined in `include/linux/types.h`.

	TODO what is the difference between using le and be?
	*/
	{
		//__u8 u8 = 127;
		//__s8 s8 = 255;

		__le16 le16 = 1;
		__be16 be16 = 1;
		if ( le16 != be16 ) return 0;
	}

	/*
	#smp

		stands for Symettrical MultiProcessing.

		means using multiple cpus at once (multicore systems)

	#per cpu variables

		<http://www.makelinux.net/ldd3/chp-8-sect-5>

		#DEFINE_PER_CPU

			define a copy of given variable for each cpu

		#get_cpu_var(name);

			get variable for current cpu

			this is a macro, so you can modify the variable with that too

		#put_cpu_var(name);

			must be called after the variable has been modified

		#smp_processor_id()

			get id of current processor

			run this many times and it may change

		#get_cpu(name, cpu)

			like `get_cpu_var`, but from any processor
	*/
	{
		get_cpu_var(cpu_int) = 0;
		put_cpu_var(cpu_int);
		printk(INFO_ID "cpu_int  = %d\n", get_cpu_var(cpu_int));

		printk(INFO_ID "smp_processor_id()  = %d\n", smp_processor_id());
	}

	/*
	#likely #unlikely

	 	the unlikely function marks a condition as rare, and makes it easier
	 	for compilers and processors to optimize the code

	 	likelly does the exac oposite

	 	those should only be used when the condition is extremelly rare (common)

	 	a typical use case is to test for errors conditions (which should, in theory, be rare...)
	*/
	{
		if (likely(0)) {
			printk(INFO_ID "ERROR\n");
		}

		if (likely(1)) {
			printk(INFO_ID "unlikely(1)\n");
		}

		if (unlikely(0)) {
			printk(INFO_ID "ERROR\n");
		}

		if (unlikely(1)) {
			printk(INFO_ID "unlikely(1)\n");
		}
	}

	/*
	#data structures

		the kernel has some basic and effective data structure implementations
		that should be reused whenever possible
	*/
	{
		/*
		#linked lists

			list_head with associated methods and macros is a doubly linked circular linked list

			based on `container_of`

			to understand things better:

				struct list_head {
					list_head *next;
					list_head *prev;
				}

			but you should use methods and macros instead of next and prev directly
		*/
		{
			struct char_list {
				char c; 		/* the actual data */
				struct list_head list;	/* used for the list */
			};

			/*
			#LIST_HEAD

				creates a differentiated `list_head` which shall represent the enteire list

				for example, you would pass this to functions that expect a list
			*/

			LIST_HEAD( alist );

			/*
			#LIST_HEAD_INIT

				links list.next and list.prev to itself:
				the first element of the list

				TODO when to use it? I have seen:

					struct char_list ca = {
						.c = 'a',
						.list = LIST_HEAD_INIT(&ca.list)
					};

				but this does not work properly for example with transversal:
				we need the differentiated element
			*/

			struct char_list ca = {
				.c = 'a',
			};

			/*
			#list_add

				adds the element next to the given one

			#list_add_tail

				adds the element previous to the given one

				if we use THE list head, this is the same as adding to the end of the list
			*/

			list_add_tail(&ca.list, &alist);

			struct char_list cb = {
				.c = 'b'
			};

			list_add_tail(&cb.list, &alist);

			struct char_list cc = {
				.c = 'c'
			};

			list_add_tail(&cc.list, &alist);

			/*
			#list_for_each_entry

				does the whole loop for us

					list_for_each_entry( list* list_pointer, list_head* list_head_ptr, field_name) {

				- list_pointer will contain each value of the list
				- list_head is the list head we want to start from
				- field name: name of the field of the struct that contains the list head

				TODO why is this skipping the a character?
			*/
			printk(INFO_ID "linked list:\n" );
			struct char_list *char_list_ptr;
			list_for_each_entry(char_list_ptr, &alist, list) {
				printk(INFO_ID "  %c\n", char_list_ptr->c );
			}

			/*
			to modify the list items while looping use:

				list_for_each_entry_safe
			*/

			/*
			#container_of

				this macros allows us to get the address of the struct given one of the
				addresses of the fields of the struct

				you don't need to use it directly with lists,
				but clearly it is needed internally to go from the `list_head` to the actual structure

				in general, the advantage of container_of is clear:
				it allows to emulate inheritance independant of the type of structure
				that is inheriting
			*/

			if ( container_of(&ca.list, struct char_list, list) != &ca )
				return 1;
		}
	}

	/*
	#algorithms

		Generally useful algorithms that you would take from libc.
	*/
	{
		int is[] = {0,1,2};
		int is2[3];
		memcpy( is2, is, 3 * sizeof( int ) );
		if ( memcmp( is, is2, 3 * sizeof(int)) != 0 ) return 1;
	}

	/*
	#page

		a page refers to the smallest virtual memory division that the kernel can get,
		usually around 4Kib today. This size is equal to the size of the page frame.

	#page frame

		a page frame refers to the smalles physical memory that the processor can ask
		from the RAM.

	#linking pages to page frames

		it would be too expensive to keep a link from every virtual memory:

			4 GiB / 4 KiB = 1 M structures per processes

		the solution is then to only keep links between used pages and frames

		this is done in a multilevel scheme
	*/

	/*
	#memory zones

		Each page belongs to a zone.

		Defined in `mmzone.h`.

		There are 3 zone types: ZONE_NORMAL, ZONE_DMA and ZONE_HIGHMEM.

		#ZONE_NORMAL

			Not any of the other pathological cases.

		#ZONE_DMA

			Used for hardware access communication.

			ISA.

			Mainly historical usage.

		#ZONE_HIGHMEM

			Memory that needs more than 32 bits to be addressed, that is,
			if you have more than 4 Gb memory.

			Harder to work with, but useful if you need lots of memory.

			It is possible to use even on IA-32 because of the PAE extension,
			which essentially adds 4 bits to the memory address bus, allowing
			64 Gb of memory. PAE was introduced in 1995.

			This zone is always empty on x64 since there is more than enough address space there.

			TODO why book says memory above 986 Mb is high memory? Why not 4 Gb?

	#page

		A page is a hardware imposed minimal unit of data transfer between the CPU and the RAM.

		Pages are modeled by `struct page` under `mm_types.h`.

		Hardware deals in terms of pages to:

		- make retrival faster, since the bus clock is much slower than the cpu clock
			and because of memory locality.

		- serve as a standard unit for page swap betweem RAM and HD

	#page flags

		Defined in `page-flags.h`.
	*/

	/*
	#kmalloc

		Like libc malloc, but for the kernel.

		Relies on the slab allocator, so it is a quite high level operation.

		Most convenient method to get small chunks of memory.

		#flags

			TODO
	*/
	{
		int *kmalloc_is = kmalloc(2 * sizeof(int), GFP_KERNEL);
		kmalloc_is[0] = 0;
		kmalloc_is[1] = 1;
		printk(INFO_ID "kmalloc_is[0] = %d\n", kmalloc_is[0]);
		printk(INFO_ID "kmalloc_is[1] = %d\n", kmalloc_is[1]);
		kfree(kmalloc_is);
	}

	/*
	#TASK_SIZE

		*virtual memory* is divided as follows:

		- memory from address from 0 to TASK_SIZE - 1 can be used by *each* processes
		- other memory adressses (from  TASK_SIZE to the maxinum adressable memory, 2^32 on 32 bits platforms
				of 2^64 on 64 ) belongs to the kernel

		TASK_SIZE is typically around 3/4 of the total memory

		note that this is *virtual* memory, so it is independant of the acutual size of the memory
		as the hardware and the kernel can give processes the illusion that they actually have
		ammounts of memory larger than the hardware for instance

	*/
	{
		printk(INFO_ID "TASK_SIZE (GiBs) = %lu\n", TASK_SIZE / (1 << 30));
	}

	/*
	#process

		the kernel manages user processes and kernel processes, scheduling them with some algorithm
		so that users see all process make some progress more or less at the same time.

		process representation is found under `sched.h` and is named `struct task_struct`

	#threads

		threads are processes that share the same address space so they act on common variables

	#kernel threads

		kernel threads are like user space threads, but are created by the kernel
		and execute on the kernel memory space with kernel priviledges.

		They can also be listed with ps:

	#current

		macro that gives the `task_struct` representing the current process

	#task_struct

		represents processes (called taks on the kern), found in `sched.h`

		#tgid

			thread group id

			same for all threads that TODO have the same data?

		#parent vs real_parent

			TODO

		there are two main scheduler used today: completely_fair and real_time

		real time attempts to be real time, but linux maker no guarantees that
		a process will actually run before a given time, only this is very likely

		#children

			processes keep a linked list of its children

		#sibling

			processes keep a linked list of its siblings

		#scheduling

			the following fields relate to process scheduling

			#state

				- TASK_RUNNING: running
				- TASK_INTERRUPTIBLE: task is waiting for some event to continue running

			#static_priority

				priority when the process was started

				can be changed with `nice` and `sched_setscheduler` system calls

			#normal_priority

				priority based on the static priority and on the scheduling policy

			#prio

				actual priority. Can be different from normal priority under certain conditions

				that the kernel decides to increase priorities

			#rt_priority

				real time priority. Range: 0 to 99, like nice, smallest is most urgent.

			#sched_class

				contains mainly function pointers that
				determine the operation of the scheduler.

			- policy

				one of:

				- SCHED_FIFO
				- SCHED_RR
				- SCHED_NORMAL
				- SCHED_BATCH
				- SCHED_IDLE

				representing the scheduling policy

			#run_list

				used by the real time scheduler only

				TODO

			#time_slice

				used by the real time only

				TODO
	*/
	{
		printk(INFO_ID "TASK_RUNNING = %d\n", TASK_RUNNING);
		printk(INFO_ID "TASK_INTERRUPTIBLE = %d\n", TASK_INTERRUPTIBLE);

		//self is obviously running when state gets printed, parent may be not:

			printk(INFO_ID "current->state  = %ld\n", current->state);
			printk(INFO_ID "current->parent->state  = %ld\n", current->parent->state);

		printk(INFO_ID "current->comm = %s\n", current->comm);
		printk(INFO_ID "current->pid  = %lld\n", (long long)current->pid);
		printk(INFO_ID "current->tgid = %lld\n", (long long)current->tgid);

		printk(INFO_ID "current->prio = %d\n", current->prio);
		printk(INFO_ID "current->static_prio = %d\n", current->static_prio);
		printk(INFO_ID "current->normal_prio = %d\n", current->normal_prio);
		printk(INFO_ID "current->rt_priority = %d\n", current->rt_priority);
		printk(INFO_ID "current->policy = %u\n", current->policy);
		printk(INFO_ID "SCHED_NORMAL = %u\n", SCHED_NORMAL);

		printk(INFO_ID "current->nr_cpus_allowed  = %d\n", current->nr_cpus_allowed);

		printk(INFO_ID "current->exit_state = %d\n", current->exit_state);
		printk(INFO_ID "current->exit_code = %d\n", current->exit_code);
		printk(INFO_ID "current->exit_signal = %d\n", current->exit_signal);

		/*  the signal sent when the parent dies  */

			printk(INFO_ID "current->pdeath_signal = %d\n", current->pdeath_signal);

		printk(INFO_ID "current->parent->pid  = %lld\n", (long long)current->parent->pid);
		printk(INFO_ID "current->parent->parent->pid  = %lld\n", (long long)current->parent->parent->pid);
		printk(INFO_ID "current->real_parent->pid  = %lld\n", (long long)current->real_parent->pid);

		//children transversal:
		{
			struct task_struct *task_struct_ptr;
			printk(INFO_ID "current->children pids:\n");
			list_for_each_entry(task_struct_ptr, &current->children, children) {
				printk(INFO_ID "  %lld\n", (long long)task_struct_ptr->pid);
			}
		}

		//siblings transversal:
		{
			struct task_struct *task_struct_ptr;

			printk(INFO_ID "current->sibling pids:\n");
			list_for_each_entry(task_struct_ptr, &current->sibling, children) {
				printk(INFO_ID "  %lld\n", (long long)task_struct_ptr->pid);
			}

			printk(INFO_ID "current->parent->sibling pids:\n");
			list_for_each_entry(task_struct_ptr, &current->parent->sibling, children) {
				printk(INFO_ID "  %lld\n", (long long)task_struct_ptr->pid);
			}
		}

		//struct list_head sibling;	/* linkage in my parent's children list */

		/* threadgroup leader */

			printk(INFO_ID "current->group_leader->pid  = %lld\n", (long long)current->group_leader->pid);

		/*
		#fs

			Process keeps a `include/linux/fs_struct.h` `fs_struct` structure,
			which contains information relating the process to the filesystem such as:

			- root path:

				Each process has a root.

				It cannot see files located outside its root.

				File operations such as `open` that start with slash `/` start at that root.

				Root in inherited (TODO check), and by default the kernel stats the initial processes at `/`.

				Root can be changed on `sh` via `chroot`.

			- pwd path:

				Good and old current directory.
		*/
		{
			printk(INFO_ID "basename pwd = %s\n", 		current->fs->pwd.dentry->d_name.name);
			printk(INFO_ID "basename dirname pwd = %s\n", 	current->fs->pwd.dentry->d_parent->d_name.name);
			printk(INFO_ID "basename root = %s\n", 		current->fs->root.dentry->d_name.name);
			printk(INFO_ID "basename dirname root = %s\n", 	current->fs->root.dentry->d_parent->d_name.name);
		}
	}

	/*
	#scheduling

		modern systems are preemptive: they can stop tasks to start another ones, and continue with the old task later

		a major reason for this is to give users the illusion that
		their text editor, compiler and music player can
		run at the same time even if they have a single cpu

		scheduling is chooshing which processes will run next

		the processes which stopped running is said to have been *preempted*

		the main difficulty is that switching between processes (called *context switch*)
		has a cost because if requires copying old memory out and putting new memory in.

		balancing this is a question throughput vs latency balace.

		- throughput is the total average performance. Constant context switches reduce it because they have a cost
		- latency is the time it takes to attend to new matters such as refreshing the screen for the user.
			Reducing latency means more context switches which means smaller throughput

		#state

			processes can be in one of the following states:

			- running: running
			- waiting: wants to run, but scheduler let another one run for now
			- sleeping: is waiting for an event before it can run
			- stopped: killed but is waiting for its parent to call wait and get exit status
			- zombie: has been killed, but parent also without calling wait

		#policy

			policy is the way of choosing which process will run next

			POSIX specifies some policies, Linux implements them and defines new ones

			policy in inherited by children processes

			#normal vs real time policies

				policies can be divided into two classes: normal and real time

				real time processes always have higher priority:
				whenever a real time process is runnable it runs instead of normal processes
				therefore they must be used with much care not to bog the system down

				the name real time policy is not very true: Linux does not absolutelly ensure
				that process will finish before some deadline.

				however, realtime processes are very priviledged,
				and in absense of other real time processes without even higher priorities,
				the processes will run as fast as the hardware can possibly run it.

		#priorities

			priorities are a measure of how important processes are,
			which defines how much cpu time
			they shall get relative to other processes

			there are 2 types of priority:

			- real time priority

				ranges from 0 to 99

				only applies to process with a real time scheduling policy

			- normal priorities

				ranges from -20 to 19

				only applies to proces with a normal scheduling policy

				also known as *nice value*. The name relates to the fact that higher nice values
				mean less priority, so the process is being nice to others and letting them run first.

				nice has an exponential meaning: each increase in nice value means that
				the relative importance of a process increases in 1.25.

			for both of them, lower numbers mean higher priority

			internally, both real time and normal priorities are represented on a single
			integer which ranges from 0 to 140:

			- real time processes are in the 0 - 99 range
			- normal processes are in the 100 - 140 range

			once again, the lower values correspond to the greater priorities

			priority is inherited by children processes

			#nice

				is the traditional name for normal priority,
				ranging from -20 (greater priority) to 19.

				an increase in 1 nice level means 10% more cpu power

		#normal policies

			#completelly fair scheduler

				all normal processes are currently dealt with internally by the *completelly fair scheduler* (CFS)

				the idea behind this scheduler is imagining a system where there are as many cpu's
				as there are processeimagining a system where there are as many cpu's as there are processes

				being fair means giving one processor for each processes

				what the CFS tries to do is to get as close to that behaviour as possible,
				even though the actual number of processors is much smaller.

			#normal scheduling policy

				represented by the `SCHED_NORMAL` define

			#batch scheduling prolicy

				represented by the `SCHED_BATCH` define

				gets lower priority than normal processes TODO exactly how much lower

			#idle scheduling prolicy

				the lowest priority possible

				processes with this policy only run when the system has absolutely

				represented by the `SCHED_IDLE` define

		#real time policies

			#fifo

				represented by the `SCHED_FIFO` define

				highes priority possible

				handled by the real time scheduler.

				the process with highest real time priority runs however much it wants

				it can only be interrupted by:

				- another real time processes with even higher priority becomes RUNNABLE
				- a SIGSTOP
				- a sched_yield() call
				- a blocking operation such as a pipe read which puts it to sleep

				therefore, lots of care must be taken because an infinite loop here
				can easily take down the system.

			#round robin

				represented by the `SCHED_RR` define

				processes run fixed ammounts of time proportional to their real time priority

				like turning around in a pie where each process has a slice proportional to
				it real time priority

				can only be preempted like fifo processes, except that it may also be preempted
				by other round robin processes

			TODO if there is a round robin and a fifo processes, who runs?

		#swapper process

			when there are no other processes to do,
			the scheduler chooses a (TODO dummy?) processes called *swapper process*

		#runqueues

			a runqueue is a list of processes that will be given cpu time
			in order which process will be activated.

			it is managed by schedulers, and is a central part of how the scheduler
			chooses which process to run next

			there is one runqueu per processor.
	*/
	{
		//max priority of an rt process:

			printk(INFO_ID "MAX_USER_RT_PRIO  = %d\n", MAX_USER_RT_PRIO);

		//max priority of any process:

			printk(INFO_ID "MAX_PRIO  = %d\n", MAX_PRIO);

		//default priority for new processes:

			printk(INFO_ID "DEFAULT_PRIO  = %d\n", DEFAULT_PRIO);
	}

	/*
	#filesystem

		mnemonic: fs

		Specifies exactly how data should be stored on the disk.

		You can have a different filesystem per partition.

	# filesystem

		linux abstracts over several hardwares and filesystem types to create a simple interface for programs

		that abstraction is called the virtual filesystem (VFS)

	#virtual filesystem

		aka VFS

		An abstraction over all filesystem types.

		Allows programs to use a single API for all types of block devices (HD, flash, DVD)

		To be supported, a filesystem has to implement this abstraction.

		The concepts of the VS are largely bijective with the ext filesystem family,
		since those are often used with Linux.

		Support for each individual filesystem type can be loaded on kernel memory as a module.

		The virtual filesystem has some similarities with RAM management because both deal with
		the retreival and modification of data, but there are a few important differences:

		- HDs data is persistent. If for example a poweroff happens in the middle of an operation,
			the disk could remain corrupted, while RAM data is thrown away at each poweroff.

		- HDs operations are *much* slower than RAM operations, because you have to wait for a magnetic
			disk to turn around and read heads to be positioned at exact locations in order to get
			your data.

		4 major structures model the virtual filesystem:

		- superblock
		- inode
		- dentry
		- file

		- inode struct:

			represents a file in the usual sense: a chunk of data on disk with medatada such as

			- filesize
			- timestamps.
			- superbloc owner

			located in `fs.h`:

		- file struct:

			represents a file open for reading.

			serveral file structs can refer to a single inode

			it contains information such as:

			- current position in the file
			- mode (read only, read-write)

			located in `fs.h`:

		- dentry struct:

			located under `dcache.h`

			represents a path component

			ex: the path `/usr/bin/env` will have the following path components:

			- /
			- usr
			- bin
			- env

			and each one has an associated `dentry` object

			located in `dcache.h`

			it facilitates directory operations, and contains fields such as:

			- d_parent:

				Pointer to the parent dentry.

				The root points to itself.
	*/
	{
		/*
		#superblock struct

			located under `fs.h`

			Represents an entire filesystem (partition).

			Superblocks are kept in a linked list.

			Fields:

			- struct list_head s_inodes

				list of all inodes

			- const struct super_operations s_op: pointers to many functions that implement super block operations
		*/
		{
			struct dentry *root;
			struct super_block *root_sb;

			root = current->fs->root.dentry;
			root_sb = root->d_sb;

			printk(INFO_ID "super_block:\n");

			//virtual blocksize:
			printk(INFO_ID "  s_blocksize = %ld\n", root_sb->s_blocksize);

			printk(INFO_ID "  s_maxbytes (GiB) = %llu\n", (long long unsigned)root_sb->s_maxbytes / (1 << 30));

			//name of corresponding block device
			printk(INFO_ID "  s_id = %s\n", root_sb->s_id);

			u8 s_uuid_string[17];
			memcpy( s_uuid_string, root_sb->s_uuid, 16 * sizeof( u8 ) );
			s_uuid_string[16] = '\0';
			printk(INFO_ID "s_uuid = %s", s_uuid_string);
		}
	}

	/*
	#ext filesystem family

		Free sources:

		- <http://www.virtualblueness.net/Ext2fs-overview/Ext2fs-overview-0.1.html>

		Main versions used: ext2, ext3 and ext4.

	#block ext

		A *block* or *sector* is the minimal unit of data transfer.

		There are two types of blocks:

		- physical: the actual minimum data transfer unit supported by the hardware.

		- logical: a filesystem parameter. Can be configured at filesystem creation.

			Determines the actual minimal blocksize that the OS will allow.

			Must be a multipe of 2^10.

			In the Linux ext c implementation of ext2 this is represented on the `s_log_block_size`
			of the `superblock` struct `struct ext2_super_block`

		You can get both physical and logical block sizethose values on `sh` with `sudo parted -l`.

		Common values for logical block sizes are: 2^10, 2^11 and 2^12.

		If the system is expected to have a few large files,
		using larger block files will be more efficient.

		Traditionally, a block could only contain a single file.
		Therefore, a one byte file would occupy an entire block.

		There are preparations for allowing block fragmentation TODO what is their status?

		In any case, fragmenting blocks would necessarily mean that access to inner files
		would be slower.

	#disk layout

		Disk layout for ext2:

			| boot block | block group 0 | ... | block group ng |

		where:

		- nb is total number of block groups

		#boot block

			aka boot sector

			Fits into one block.

			The boot block is not managed by the filesystem,
			but its space must be reserved.

			There are two types of boot blocks:

			TODO difference between both, confirm all the following info:

			- master boot record (MBR):

				One at the very start of each hard disk.

				Contains two pieces of information:

				- code to boot the system
				- a partition table, which indicates where each partition starts.

			- volume boot record (VBR):

		#block group

			Each block groups is of type:

				| super block | group descriptor 0 | ... | group descriptor ng | data block bitmap |
				| inode bitmap | inode table 0 | ... | inode table ni | data block 0 | ... | data block ni |

			where:

			- nb is total number of block groups
			- ni is total number of inodes

			Each block must fit into one physical block.

			The data of each super block and of the bg group descriptors of each group block are the same
			on all group blocks.

			This redundance is done to:

			- keep metadata close to data to reduce the access time

			- reduce probability of disk corruption (TODO confirm this)

			##super block

				Store global information about the entire filesystem.

				Represented in the Linux kernel by `struct ext2_superblock` under `linux/kernel/fs/ext2.h`

				This includes interesting fields such as:

				- s_inodes_count: number of inodes in entire filesystem.

				- s_blocks_count: size of the filesystem in blocks.

				- s_log_block_size: log 2 of the multiplier of 1024 of the logical block size.

					Ex: 0 means block size 1024, 1 means 2 x 1024.

				- s_blocks_per_group: number of blocks for each group

				- s_mtime: last mount time

				- s_wtime: time of latest write opertion

				- s_mount_count: number of times this has been mounted.

				- s_max_mnt_count: maximun number of times it can be mounted.

				- s_magic: major version number. Differentiates ext2 from ext3 and ext4.

				- s_minor_rev_level: minor version number

				- __u8[16] s_uuid: numerical identifier

				- char[16] s_volume_name: numerical unique identifier for filesystem

				- char[64] s_last_mounted: path where it was last mounted.

			##group descriptor

				ext2_group_desc
	*/
	{
	}

	/*
	#device driver

	there are 3 main types of device drivers:

	- character devices: simplest one. Applications can only access data as a stream, not randomly.

		Useful for devices like mice, keyboard

	- block device: devices like hard disks or dvd readers. Random access is required.

	- network device: TODO
	*/

	/*
	#character device

		they are represented by the struct cdev found in TODO

		dev_t dev;
		int alloc_chrdev_region(&dev, 0, 1, "char_cheat");
	*/


	/*
	#interrupt handler

		is a function that does what must be done in case of an interrupt,
		typically a message sent by hardware such as a mouse saying "hey I moved"

		it cannot be interrupted by other kernel process, so it should be real quick in its job
		and do only what is absolutelly essential

		typically the jobs it will do are:

		      - save the data from some buffer into RAM. this prevents small buffers from getting filled up.
		      - send an aknowledgment to the hardware that the interrupt was handled so the hardware can
		          //continue to send data for example

		what it typically should *not* do it to actually procees the data that was aquired
		this should be left for the bottom half.

	#request_irq

		this function tells the kernel to use a given interrupt function to deal with certain interrupts

		1. interrupt line. If non existent, will be created.

			this is a number the identifies each type of interrupt (ex: one from the mouse, one from the keyboard, etc.)

			it may be fixed for very traditional devices like the keyboard, but dynamic for most modern devices.

			TODO how to determine a line dynamically?

		2. interrupt handler function

		3. flags.

			- `IRQF_SHARED` means that multiple handlers will handle this line.

				when each one is registered it must use `IRQF_SHARED`

			- `IRQF_SAMPLE_RANDOM` tells the kernel to use timings of this interrupt to add entropy to the random number generator

				should only be done for things which are unpredictable in timing such as mouve moves of disk accesses,
				but not for regular things like a hardware clock

		4. name. Should be unique, allows for communication with the user via `/proc/irq` and `/proc/interrupts` TODO understand

		5. number that distinguishes between multiple irqs in the same line.

			therefore be unique across different irqs on the same line.

			Can be `NULL` if irq is not shared. TODO understand

	*/

		//TODO get working
		/*if (request_irq(rtc_irq, rtc_interrupt, IRQF_SHARED, "rtc", (void *)&rtc_port)) {*/
			/*printk(KERN_ERR "rtc: cannot register IRQ %d\n", rtc_irq);*/
			/*return -EIO;*/
		/*}*/

	/*
	#modules
	*/
	{
		/*
		#THIS_MODULE

			pointer to the module struct of current module

			the module struct and THIS_MODULE are both defined inside `module.h`

			this struct determines all the information about a module
		*/
		{
			//version is was set with the MODULE_VERSION macro:

			printk(INFO_ID "THIS_MODULE->version = %s\n", THIS_MODULE->version );
		}

		/*
		#parameters
		*/
		{
			printk(INFO_ID "param_i = %d\n", param_i);
			printk(INFO_ID "param_s = %s\n", param_s);
			/*printk("param_is = %d, %d, %d\n", param_is[0], param_is[1], param_is[2]);*/
		}
	}

	return 0;
}

module_init(init);

/*static irqreturn_t rtc_interrupt(int irq, void *dev)*/
/*{*/
	/*
	* Can be an alarm interrupt, update complete interrupt,
	* or a periodic interrupt. We store the status in the
	* low byte and the number of interrupts received since
	* the last read in the remainder of rtc_irq_data.
	*/
	/*spin_lock(&rtc_lock);*/
	/*rtc_irq_data += 0x100;*/
	/*rtc_irq_data &= ~0xff;*/
	/*rtc_irq_data |= (CMOS_READ(RTC_INTR_FLAGS) & 0xF0);*/
	/*if (rtc_status & RTC_TIMER_ON)*/
		/*mod_timer(&rtc_irq_timer, jiffies + HZ/rtc_freq + 2*HZ/100);*/
	/*spin_unlock(&rtc_lock);*/
	/*
	* Now do the rest of the actions
	*/
	/*spin_lock(&rtc_task_lock);*/
	/*if (rtc_callback)*/
		/*rtc_callback->func(rtc_callback->private_data);*/
	/*spin_unlock(&rtc_task_lock);*/
	/*wake_up_interruptible(&rtc_wait);*/
	/*kill_fasync(&rtc_async_queue, SIGIO, POLL_IN);*/
	/*return IRQ_HANDLED;*/
/*}*/

/*
 * same as init, with `module_exit` to fix this as exit point.
 *
 * naming this `module_cleanup` worked.
 *
 * #__exit
 *
 * 	if this is module is ever built into the kernel,
 * 	`__exit` functions are simply discarded to make up free space.
 *
 * */
static void __exit cleanup(void)
{
	printk(INFO_ID "%s\n", __func__ );
}

module_exit(cleanup);
