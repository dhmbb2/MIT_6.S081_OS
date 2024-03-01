# SYSCALL
In lab syscall, we add two syscalls, systrace and sysinfo, to xv6.

## Systrace

A trace call takes in an integral mask, whose bits specify which system call to trace. Every time a traced syscall is called. OS will print out the pid of process from which it is called, the name of the syscall and the return value of the call.

To begin with, notice that entrance to syscall is automatically generated in `user/usys.pl`, the following code should be added to `user/usys.pl`:
```c
// in user/usys.pl

sub entry {
    my $name = shift;
    print ".global $name\n";
    print "${name}:\n";
    print " li a7, SYS_${name}\n";
    print " ecall\n";
    print " ret\n";
}
entry("trace");
```

Add an integer `int traced_call` to `struct proc` documenting all traced syscall.

`sys_trace()` read the argument from `%a0` and store it in current process's `traced_call`.
```c
// in kernel/sysproc.c

uint64
sys_trace(void)
{
  int call_to_trace;
  argint(0, &call_to_trace);
  myproc()->traced_call = call_to_trace;
  return 0;
}
```

In function `syscall()`, add corresponding logic to print stuff:
```c
// in kernel/syscall.c

if (((p->traced_call >> num) & 1) == 1) {
    printf("%d: syscall %s -> %d\n",
            p->pid, command_name[num-1], p->trapframe->a0 );
}
```

Add some logic in `fork()` to make sure that child process inherit parents' traced calls
```c
// in kernal/proc.c fork()


if((np = allocproc()) == 0){
    return -1;
}

np->traced_call = p->traced_call;
```

## Sysinfo

`sysinfo` collects information about the running system. The system call takes one argument: a pointer to a struct `sysinfo` (see `kernel/sysinfo.h`). The kernel should fill out the fields of this struct: the `freemem` field should be set to the number of bytes of free memory, and the `nproc` field should be set to the number of processes whose state is not `UNUSED`.

First we need two helper functions. We iterate over free lists which stores free memory and calculate the num of free memory size in terms of bytes.
```c
uint free_list_size(void) 
{
  uint size = 0;
  acquire(&kmem.lock);
  struct run* p = kmem.freelist;
  for (; p; p = p->next) 
    size += 1;
  release(&kmem.lock);
  return size * PGSIZE;
}
```

Now we traverse over array which stores all proc `proc[NPROC]` and count proccesses which is not `UNUSED`
```c
uint 
num_used_proc(void) 
{
  struct proc* p;
  uint num = 0;

  for (p = proc; p < &proc[NPROC]; p ++) {
    if (p->state != UNUSED)
      ++num;
  }

  return num;
}
```

`sys_syscall()` first define a kernal space sysinfo struct and collect needed information. Then it uses `copyout()` to copy the kernal space struct into user space (Store data in the struct passed in as argument).
```c
uint64
sys_sysinfo(void)
{
  struct sysinfo info = {0, 0};
  info.freemem = free_list_size();
  info.nproc = num_used_proc();
  uint64 addr;
  if (argaddr(0, &addr) < 0)
    return -1;
  if (copyout(myproc()->pagetable, addr, (char*)&info, sizeof(info)) < 0)
    return -1;
  return 0;
}
```

