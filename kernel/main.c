/*
main cheat on the kernel kernel modules and on kernel concepts
that can be exemplified in modules (much easier than recompiling and reinstalling the kernel)

#TODO

	#__user

		userspace values

	#__rcu

		a type of locking directive

#__KERNEL__

	Defined on the Makefile when compiling the kernel or kernel modules.

	Used with `ifdef` blocks on files which may be included from userspace,
	to avoid that parts of those files be used on userpace. Example:

		//for kernel or userspace
		#ifdef __KERNEL__
			//only for the kernel
		#endif
*/

#include <linux/version.h> 	/* include/generated/uapi/linux. LINUX_VERSION_CODE, KERNEL_VERSION */

#include <asm/page.h>		/* PAGE_SIZE */
#include <asm/param.h>		/* HZ */
#include <asm/atomic.h>		/* atomic_t */

#include <linux/dcache.h>	/* dentry, super_block */
#include <linux/errno.h>	/* ENOMEM,  */
#include <linux/fs_struct.h>	/* fs_struct */
#include <linux/fs.h>		/* super_block */
#include <linux/gfp.h>		/* Mnemonic: Get Free Pages. alloc_pages */
#include <linux/interrupt.h> 	/* request_irq, IRQF_SHARED */
#include <linux/jiffies.h> 	/* jiffies */
#include <linux/kernel.h>	/* KERN_INFO */
#include <linux/kthread.h>	/* kthread_create */
#include <linux/mm.h>		/* Memory Management. page_address. Includes mm_types.h. */
#include <linux/mm_types.h>	/* page, mm_struct */
#include <linux/module.h>	/* module specific utilities: MODULE_* macros, module_param, module_init, module exit */
#include <linux/path.h>		/* path */
#include <linux/sched.h>	/* current */
#if LINUX_VERSION_CODE >= KERNEL_VERSION(3, 9, 0)
	#include <linux/sched/rt.h>	/* MAX_PRIO, MAX_USER_RT_PRIO, DEFAULT_PRIO */
#endif
#include <linux/slab.h> 	/* kmalloc, kmem_cach_create, kmem_cach_alloc */
#include <linux/spinlock.h>
#include <linux/string.h>	/* memcpy, memcmp */

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
static atomic_t i_global_atomic;

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
 * return value:i
 * - 0 on success
 * - non zero on failure.
 *
 *   	You should always return the negation of constants defined in `linux/errno.h`,
 *   	for example as `return -ENOMEM`
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
	/* separate from older entries in log*/
	printk(KERN_INFO __FILE__ ": \n============================================================\n");

	/*
	#printk

		The kernel has no simple way to communicate with a terminal
		so you the simplest thing to do is dump program output to a file.

		`printk` does this in a very reliable manner

		At the time of writting on Ubuntu 13.04 the file is: `/var/log/syslog`

		This can be viewed with `dmesg`.

		`KERN_INFO` is a message priority string macro

		It is understood by printk when put at the beginning of the input string.

		8 levels are defined in order of decreasing priority:

		- LOG_EMERG: 	system is unusable
		- LOG_ALERT: 	action must be taken immediately
		- LOG_CRIT: 	critical conditions
		- LOG_ERR: 	error conditions
		- LOG_WARNING: 	warning conditions
		- LOG_NOTICE: 	normal, but significant, condition
		- LOG_INFO: 	informational message
		- LOG_DEBUG: 	debug-level message

		printk takes printf format strings with containing things like `%d`
	*/
	{
		printk(KERN_INFO "%s\n", __func__ );
	}

	/* Don't be afraid, it's just a c program. Globals are still globals. */
	i_global = 0;
	printk(KERN_INFO "i_global = %d\n", i_global);

	/*
	#version

		Device drivers depend on kernel version.

		You can get some version flexibility with the preprocessor.

		#LINUX_VERSION_CODE

			Example: on kernel `2.6.10` == 0x02060a

		#KERNEL_VERSION

			Transform human version numbers into HEXA notation:

				0x02060a == KERNEL_VERSION(2, 6, 10)

			Always use it in case some day the version organization changes.
	*/
	{
		/* printk( "UTS_RELEASE = %s", UTS_RELEASE ); */	/* TODO get working */
		printk(KERN_INFO "LINUX_VERSION_CODE = %d\n", LINUX_VERSION_CODE);

		/* are we at least at 2.6.10? */
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 10)
		printk(KERN_INFO "LINUX_VERSION_CODE >= 2.6.10\n");
#endif
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
	{
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
	}
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
		printk(KERN_INFO "cpu_int  = %d\n", get_cpu_var(cpu_int));

		printk(KERN_INFO "smp_processor_id()  = %d\n", smp_processor_id());
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
			printk(KERN_INFO "ERROR\n");
		}

		if (likely(1)) {
			printk(KERN_INFO "unlikely(1)\n");
		}

		if (unlikely(0)) {
			printk(KERN_INFO "ERROR\n");
		}

		if (unlikely(1)) {
			printk(KERN_INFO "unlikely(1)\n");
		}
	}

	/*
	#atomic operations

		ANSI C does not guarantee that any operation is atomic, not even things like: `a++`.

		Therefore, you must use atomic opertation if you want to ensure atomicity.

		In some cases, use of those instructions is enough to guarantee synchronization,
		for example when incrementing usage counters. This is why usage counters are often `atomic_t`.

		This is a very efficient, but restricted, synchornization mechanism if applicable.

		Available operations do things like:

		- add
		- subtract
		- subtract and test greater than
		- binary operations

	#atomic_t

		Type used on all atomic operations.

	#ATOMIC_INIT(int)

		Initialize an atomic to given integer.

		Only works at compile time and for initialization.

	#atomic_set(int *, int)

		Set value of atomic.

		Unlike ATOMIC_INIT can be used anywhere.
	*/
	{
		atomic_t i = ATOMIC_INIT(0);
		//ERROR: not initialization
		//i = ATOMIC_INIT(0);
		atomic_inc(&i);
		if ( atomic_read(&i) != 1 ) return -1;
	}

	/*
	#time

		There are 2 types of time:

		- absolute. Ex: 1/1/2010. Hardware: RTC. Precision: Hz.
		- relative: Ex: 1 sec after now.

			Hardware: system timer. Precision: kHz. Interface: jiffies.

			Greater precision interfaces (up to nanoseconds are also available)

		Time is important on the kernel,
		for example when giving hardware time to complete certain tasks.

	#system timer

		Programmable hardware that emmits interrputs at a given frequency,
		on the 10 - 1k Hz range as of 2013.

	*/
	{
		/*
		#HZ

			Frequency of the system clock.

		#jiffies

			How many system clock periods have passed since boot.

			Starts at 0.

			Mnemonic: in a jiffy is an informal / old expression for in a while.
			A jiffle then is a small amount of time.

		#wraparound

			jiffies is an unsigned long, so if we reach its limit it wraps around to 0.

			Example:

				//half a second in the future
				unsigned long timeout = jiffies + HZ/2;

				//work

				//see whether we took too long
				if (timeout > jiffies) {
					//we did not time out, good
				} else {
					//we timed out, error
				}

			What if `jiffies` goes around and comes back to 0?

			This is why you should use:

				#define time_after(unknown, known) ((long)(known) - (long)(unknown) < 0)
				#define time_before(unknown, known) ((long)(unknown) - (long)(known) < 0)
				#define time_after_eq(unknown, known) ((long)(unknown) - (long)(known) >= 0)
				#define time_before_eq(unknown, known) ((long)(known) - (long)(unknown) >= 0)

			to compare times as:

				if (time_before(jiffies, timeout)) {

			TODO why does this work?
		*/
		{
			printk( KERN_INFO "HZ = %d\n", HZ );
			printk( KERN_INFO "jiffies = %lu\n", jiffies );
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
			printk(KERN_INFO "linked list:\n" );
			struct char_list *char_list_ptr;
			list_for_each_entry(char_list_ptr, &alist, list) {
				printk(KERN_INFO "  %c\n", char_list_ptr->c );
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
				return -1;
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
		if ( memcmp( is, is2, 3 * sizeof(int)) != 0 ) return -1;
	}

	/*
	#page

		First learn about hardware paging in a common architecture such as x86 family.
		This will be not explained here.

		Pages are modeled by `struct page` under `mm_types.h`.

		Hardware deals in terms of pages to:

		- make retrival faster, since the bus clock is much slower than the cpu clock
			and because of memory locality.

		- serve as a standard unit for page swap betweem RAM and HD

	#page flags

		Defined in `page-flags.h`.

	#page frame

		A page frame refers to the smalles physical memory that the processor can ask
		from the RAM.

		Paging usually has hardware support today.

	#linking pages to page frames

		it would be too expensive to keep a link from every virtual memory:

			4 GiB / 4 KiB = 1 M structures per processes

		the solution is then to only keep links between used pages and frames

		this is done in a multilevel scheme
	*/
	{
		printk(KERN_INFO "PAGE_SIZE (Kib) = %lu\n", PAGE_SIZE / ( 1 << 10 ));
	}

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
	*/

	/*
	#memory allocation

		The following methods are common for memory allocation by the kernel for is own use:

		- alloc_pages
		- kmalloc
		- slab alocator methods such as: kmem_cach_create + kmem_cache_alloc
		- vmalloc

		#gfp flags

			Certain flags are used on all of those functions.

			They are:

			TODO
	*/

	/*
	#alloc_pages

		Gets a given number of contiguous (linear address) pages.

		The number of pages is the log_2 of the multiplier.

		Use this when you want the memory for a small number of large objects.

		Based on the Buddy System.

	#page_address

		Returns start of linear address of given page, NULL if that page is on high memory
		or is not mapped.

		TODO page_address vs page->virtual?

	#free_pages

		Like aloc_pages, but takes the starting linear address.

	#page struct

		Fields:

		- long virtual: address of current page

		- atomic_t _count: usage count by whom TODO

	*/
	{
		struct page *page;
		page = alloc_pages(GFP_KERNEL, 1);
		printk(KERN_INFO "alloc_pages\n" );
		if ( page == NULL ){
			printk(KERN_INFO "  NULL\n" );
		} else {
			char *cs = page_address(page);

			if ( (long)cs % PAGE_SIZE != 0 ) return -1;

			for ( int i = 0; i < 2 * PAGE_SIZE; i++ ){
				cs[i] = i;
			}

			printk( KERN_INFO "  _count = %d\n", atomic_read(&page->_count) );

			free_pages( (long)cs, 1);
		}
	}

	/*
	#slab allocator

		Best way to allocate several objects of the same type (size and required initial data).

		This is more efficicient than other methods because

		- it tries to keep hardware caches correctly aligned

		Structure:

		- each cache contains many slabs.

			All objects contained in those slabs will be of the same type.

		- each slab contains objects.

			Each slab occupies an integer number of contiguous pages.

			Therefore, this method is only good if you are going to allocate enough
			small objects to at least fill a page.

			Objects can be either free or occupied.

		#kmem_cache_create

			Create a cache.

			Signature:

				kmem_cache *kmem_cache_create(
					const char *name,
					size_t size,
					size_t offset,
					unsigned long flags,
					void (*ctor)(void *)
				);

			In a module, this operation would be typically done at module startup time.

			The constructor is called on the data at creation of every object.
			NULL means no contructor.

		#kmem_cache_alloc

			Allocate data on a created chache.

			You do not need to know in which slab it will be created.

		#kmem_cache_free

			Free data on a cache.

		#kmem_cache_destroy

			Delete a cache.

	*/
	{
		struct kmem_cache *cache;
		int *is[2];

		/* simple constructor function that initializes each array to { 1, 2 } */
		void ctor(void *vobj){
			int *obj = (int *)vobj;
			obj[0] = 1;
			obj[1] = 2;
		}

		//create the cache
		cache = kmem_cache_create(
			"test_cache_0",
			2 * sizeof( int ),
			0,
			0,
			ctor
		);
		if (!cache) return -1;

		//allocate memory for the cache
		//we make two pairs of integers
		is[0] = kmem_cache_alloc(cache, GFP_KERNEL);
		is[1] = kmem_cache_alloc(cache, GFP_KERNEL);

		is[1][1]++;

		if ( is[0][0] != 1 ) return -1;
		if ( is[0][1] != 2 ) return -1;
		if ( is[1][0] != 1 ) return -1;
		if ( is[1][1] != 3 ) return -1;

		kmem_cache_free(cache, is[0]);
		kmem_cache_free(cache, is[1]);

		kmem_cache_destroy(cache);
	}

	/*
	#kmalloc

		Like libc malloc, but for the kernel.

		Use this when you want to create a single,
		or a small number of objects of a type that is not too large.

		Based on the slab allocator.
	*/
	{
		int *is = kmalloc(2 * sizeof(int), GFP_KERNEL);
		if ( !is ) return -ENOMEM;
		is[0] = 0;
		is[1] = 1;
		is[0]++;
		is[1]++;
		if ( is[0] != 1 ) return -1;
		if ( is[1] != 2 ) return -1;
		kfree(is);
	}

	/*
	#process

		the kernel manages user processes and kernel processes, scheduling them with some algorithm
		so that users see all process make some progress more or less at the same time.

		process representation is found under `sched.h` and is named `struct task_struct`

	#threads

		threads are processes that share the same address space so they act on common variables

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

				#TASK_RUNNING

					The scheduler may run it at any time running.

				#TASK_INTERRUPTIBLE

					Will not be scheduled until:

					- someone calls `wake_up_interruptible` on it
					- it receives a signal

				#TASK_UNINTERRUPTIBLE

					Same as TASK_INTERRUPTIBLE, except that it cannot receive signals.

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
		printk(KERN_INFO "TASK_RUNNING = %d\n", TASK_RUNNING);
		printk(KERN_INFO "TASK_INTERRUPTIBLE = %d\n", TASK_INTERRUPTIBLE);

		//self is obviously running when state gets printed, parent may be not:

			printk(KERN_INFO "current->state  = %ld\n", current->state);
			printk(KERN_INFO "current->parent->state  = %ld\n", current->parent->state);

		printk(KERN_INFO "current->comm = %s\n", current->comm);
		printk(KERN_INFO "current->pid  = %lld\n", (long long)current->pid);
		printk(KERN_INFO "current->tgid = %lld\n", (long long)current->tgid);

		printk(KERN_INFO "current->prio = %d\n", current->prio);
		printk(KERN_INFO "current->static_prio = %d\n", current->static_prio);
		printk(KERN_INFO "current->normal_prio = %d\n", current->normal_prio);
		printk(KERN_INFO "current->rt_priority = %d\n", current->rt_priority);
		printk(KERN_INFO "current->policy = %u\n", current->policy);
		printk(KERN_INFO "SCHED_NORMAL = %u\n", SCHED_NORMAL);

		printk(KERN_INFO "current->nr_cpus_allowed  = %d\n", current->nr_cpus_allowed);

		printk(KERN_INFO "current->exit_state = %d\n", current->exit_state);
		printk(KERN_INFO "current->exit_code = %d\n", current->exit_code);
		printk(KERN_INFO "current->exit_signal = %d\n", current->exit_signal);

		/*  the signal sent when the parent dies  */

			printk(KERN_INFO "current->pdeath_signal = %d\n", current->pdeath_signal);

		printk(KERN_INFO "current->parent->pid  = %lld\n", (long long)current->parent->pid);
		printk(KERN_INFO "current->parent->parent->pid  = %lld\n", (long long)current->parent->parent->pid);
		printk(KERN_INFO "current->real_parent->pid  = %lld\n", (long long)current->real_parent->pid);

		//children transversal:
		{
			struct task_struct *task_struct_ptr;
			printk(KERN_INFO "current->children pids:\n");
			list_for_each_entry(task_struct_ptr, &current->children, children) {
				printk(KERN_INFO "  %lld\n", (long long)task_struct_ptr->pid);
			}
		}

		//siblings transversal:
		{
			struct task_struct *task_struct_ptr;

			printk(KERN_INFO "current->sibling pids:\n");
			list_for_each_entry(task_struct_ptr, &current->sibling, children) {
				printk(KERN_INFO "  %lld\n", (long long)task_struct_ptr->pid);
			}

			printk(KERN_INFO "current->parent->sibling pids:\n");
			list_for_each_entry(task_struct_ptr, &current->parent->sibling, children) {
				printk(KERN_INFO "  %lld\n", (long long)task_struct_ptr->pid);
			}
		}

		//struct list_head sibling;	/* linkage in my parent's children list */

		/* threadgroup leader */

			printk(KERN_INFO "current->group_leader->pid  = %lld\n", (long long)current->group_leader->pid);

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
			printk(KERN_INFO "basename pwd = %s\n", 		current->fs->pwd.dentry->d_name.name);
			printk(KERN_INFO "basename dirname pwd = %s\n", 	current->fs->pwd.dentry->d_parent->d_name.name);
			printk(KERN_INFO "basename root = %s\n", 		current->fs->root.dentry->d_name.name);
			printk(KERN_INFO "basename dirname root = %s\n", 	current->fs->root.dentry->d_parent->d_name.name);
		}

		/*
		#mm_struct

			Describes the process adress space.

			TODO mm vs active_mm

			- struct rb_root mm_rb: root of the rb tree that orders memory

			- unsigned long
				start_code, end_code,
				start_data, end_data,
				start_brk, brk (end),
				start_stack, (TODO no end)
				arg_start, arg_end,
				env_start, env_end:

				start and end of all given memory zones.

				The only cryptic one is brk which is the heap (malloc).

			- unsigned long total_vm: number of pages in process address space

			- unsigned long locked_vm: pages that cannot be swapped out
		*/
		{
			printk(KERN_INFO "mm_struct");
			printk(KERN_INFO "  start_code = %lx\n", current->mm->start_code);
			printk(KERN_INFO "  end_code   = %lx\n", current->mm->end_code);
			printk(KERN_INFO "  start_data = %lx\n", current->mm->start_data);
			printk(KERN_INFO "  end_data   = %lx\n", current->mm->end_data);
			printk(KERN_INFO "  start_brk  = %lx\n", current->mm->start_brk);
			printk(KERN_INFO "  brk        = %lx\n", current->mm->brk);
			printk(KERN_INFO "  arg_start  = %lx\n", current->mm->arg_start);
			printk(KERN_INFO "  arg_end    = %lx\n", current->mm->arg_end);
			printk(KERN_INFO "  env_start  = %lx\n", current->mm->env_start);
			printk(KERN_INFO "  env_end    = %lx\n", current->mm->env_end);

			printk(KERN_INFO "  total_vm   = %lu\n", current->mm->total_vm);
			printk(KERN_INFO "  locked_vm  = %lu\n", current->mm->locked_vm);
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

		#sources

			#sleep

				- <http://www.linuxjournal.com/node/8144/print>

					how to sleep in the kernel

				Low level way:

					set_current_state(TASK_INTERRUPTIBLE);
					schedule();

				Higher level way: wait queues.

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

	#schedule()

		Tell the scheduler that he can schedule another process for now.

		Like posix yield.

	#wake_up_process(struct task_struct)

		Sets process to `TASK_RUNNING`.

	#set_current_state

		Set state for current process. Ex:

			set_current_state(TASK_INTERRUPTIBLE)
	*/
	{
		//max priority of an rt process:

			printk(KERN_INFO "MAX_USER_RT_PRIO  = %d\n", MAX_USER_RT_PRIO);

		//max priority of any process:

			printk(KERN_INFO "MAX_PRIO  = %d\n", MAX_PRIO);

		//default priority for new processes:

			printk(KERN_INFO "DEFAULT_PRIO  = %d\n", DEFAULT_PRIO);

		schedule();
	}

	/*
	#kernel threads #kthread

		The kernel can spawn its own threads.

		They are run on kernel space.

		This is often used to do cyclical jobs such as swapping memory pages,
		processing driver data, etc.

		You can explicitly create a new thread via the `kthread_create` function.

		Kernel threads are quite low level, so on real applications first check
		if what you want to achieve cannot be achieved via sotfirq or other bottom halves
		which is most often the case.

		This is a low level mechanism which is not very often used diretcly.
		Prefer higher level mechanisms is possible such as:

		- wait queue
		- bottom half

		#kthread_create

			Create kernel threads.

			Signature:

				struct task_struct *kthread_create(
					int (*function)(void *data),
					void *data,
					const char name[],
					...
				)

			- `function` is what will be run on the thread
			- `data` is what will be passed to function
			- `name` is an identifier for the thread

			After creating a thread you must wake it up with a `wake_up_process(struc task_struct *)` call`

		#kthread_run

			Same as kthread_create, but also starts the thread.

		#kthread_stop

			<https://www.kernel.org/doc/htmldocs/device-drivers/API-kthread-stop.html>

			TODO how to use it. Waits or kills? Seems to kill. How to wait?

			Signature:

				kthread_stop(struc task_struct *)

		#kthread_should_stop

			TODO how to use it
	*/

	//single thread
	{
		struct task_struct *thread;

		struct data {
			int i;
			struct task_struct* caller;
		};

		//int status;

		int function(void* vdata)
		{
			struct data *data = (struct data *)vdata;
			printk(KERN_INFO "kthread. i = %d, pid = %lld", data->i, (long long)current->pid);
			i_global++;
			//wake up our caller so he can continue
			wake_up_process(data->caller);
			return 0;
		}

		struct data data = {
			.i = 0,
			.caller = current
		};

		i_global = 0;

		thread = kthread_run(
			function,
			&data,
			"test_kthread_0"
		);

		//if we had used kthread_create:

			//if ( thread != NULL ) {
			//	wake_up_process(thread);
			//}

		set_current_state(TASK_INTERRUPTIBLE);
		schedule();

		//assert that the thread finished
		//if ( i_global_atomic != 1 ) return -1;
		if ( i_global != 1 ) printk("fail");
	}

	/*
	*/
	if ( 0 ) //TODO get working on mulitple threads. Sync problem?
	{
		const int n_threads = 4;
		struct task_struct *threads[n_threads];

		struct data {
			int i;
			struct task_struct* caller;
		};

		//int status;

		int function(void* vdata)
		{
			struct data *data = (struct data *)vdata;
			printk(KERN_INFO "kthread. i = %d, pid = %lld", data->i, (long long)current->pid);
			atomic_inc(&i_global_atomic);
			//wake up our caller so he can continue
			wake_up_process(data->caller); //TODO: is the sync problem here?
			return 0;
		}

		atomic_set(&i_global_atomic, 0);

		for ( int i = 0; i < n_threads; i++ ) {

			struct data data = {
				.i = i,
				.caller = current
			};

			threads[i] = kthread_run(
				function,
				&data,
				"test_kthread_0"
			);
		}

		while( atomic_read(&i_global_atomic) < n_threads ) {
			//sleep until the kthread wakes us up
			set_current_state(TASK_INTERRUPTIBLE);
			schedule();
		}

		//assert that all threads finished
		//if ( i_global_atomic != 1 ) return -1;
		if ( atomic_read(&i_global_atomic) != n_threads ) printk("fail");
	}

	/*
	#wait queues

		[ldd3e]

		High level method to create kthreads and wait for them to end.

		Create one statically:

			DECLARE_WAIT_QUEUE_HEAD(name);

		dynamicly:

			wait_queue_head_t my_queue;
			init_waitqueue_head(&my_queue);

		Sleep macros:

		- void wait_event(queue, condition)

			Sleep, not interruptible by signals.

		- int wait_event_interruptible(queue, condition)

			Sleep, interruptible by signals.

			Return value: `0` means we were not interrupted,
			`1` means we were.

		- timeout versions

			- int wait_event_timeout(queue, condition, timeout)
			- int wait_event_interruptible_timeout(queue, condition, timeout)

			timeout given in jiffles

			After timeout, functions return 0.

		Conditions: the process sleeps in the first place only if the contidion is not met.

		Wake up methods:

		- void wake_up(wait_queue_head_t *queue);

			Wakes single method from queue.

		- void wake_up_all(wait_queue_head_t *queue);

			Wakes all methods from queue.

		- void wake_up_interruptible(wait_queue_head_t *queue);

			Wakes single interruptible method from queue.
	*/
	if ( 0 ) //TODO get working.
	{
		struct task_struct *thread;

		DECLARE_WAIT_QUEUE_HEAD(wq); //BUG i think I can't do this

		struct data {
			int i;
			int j;
		};

		//int status;

		int function(void* vdata)
		{
			struct data *data = (struct data *)vdata;
			printk(KERN_INFO "kthread. i = %d, j = %d, pid = %lld", data->i, data->j, (long long)current->pid);
			i_global++;
			//wake up our caller so he can continue
			wake_up_all(&wq);
			return 0;
		}

		struct data data = {
			.i = 1,
			.j = 2
		};

		i_global = 0;

		//for ( int i = 0; i < 2; i++ )
		thread = kthread_run(
			function,
			&data,
			"test_kthread_0"
		);

		wait_event_interruptible(wq, i_global == 1);

		//assert that the function finished
		//if ( i_global != 1 ) return -1;
		if ( i_global != 1 ) printk("fail");
	}

	/*
	#TASK_SIZE

		*virtual memory* is divided as follows:

		- memory from address from 0 to TASK_SIZE - 1 can be used by *each* processes
		- other memory adressses (from  TASK_SIZE to the maxinum adressable memory, 2^32 on 32 bits platforms
				of 2^64 on 64 ) belongs to the kernel

		TASK_SIZE is typically around 3/4 of the total memory

		Note that this is *virtual* memory, so it is independant of the acutual size of the memory
		as the hardware and the kernel can give processes the illusion that they actually have
		ammounts of memory larger than the hardware for instance.

	*/
	{
		printk(KERN_INFO "TASK_SIZE (GiB) = %lu\n", TASK_SIZE / (1 << 30));

		//Kernel virtual memory must be above `TASK_SIZE`:

			int i;
			if ( (int)&i < TASK_SIZE ) return -1;
			printk( KERN_INFO "(void*)&i = %p\n", (void*)&i );

		//User virtual memory will always be below TASK_SIZE.
		//Print addresses of user space program variables to check this (3Gb = `0xc0000000`)
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

			printk(KERN_INFO "super_block:\n");

			//virtual blocksize:
			printk(KERN_INFO "  s_blocksize = %ld\n", root_sb->s_blocksize);

			printk(KERN_INFO "  s_maxbytes (GiB) = %llu\n", (long long unsigned)root_sb->s_maxbytes / (1 << 30));

			//name of corresponding block device
			printk(KERN_INFO "  s_id = %s\n", root_sb->s_id);

			u8 s_uuid_string[17];
			memcpy( s_uuid_string, root_sb->s_uuid, 16 * sizeof( u8 ) );
			s_uuid_string[16] = '\0';
			printk(KERN_INFO "s_uuid = %s\n", s_uuid_string);
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

			printk(KERN_INFO "THIS_MODULE->version = %s\n", THIS_MODULE->version );
		}

		/*
		#parameters
		*/
		{
			printk(KERN_INFO "param_i = %d\n", param_i);
			printk(KERN_INFO "param_s = %s\n", param_s);
			/*printk("param_is = %d, %d, %d\n", param_is[0], param_is[1], param_is[2]);*/
		}
	}

	printk(KERN_INFO "============================================================\n");

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
		/*mod_timer(&rtc_irq_timer, jiffles + HZ/rtc_freq + 2*HZ/100);*/
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
	printk(KERN_INFO "%s\n", __func__ );
}

module_exit(cleanup);
