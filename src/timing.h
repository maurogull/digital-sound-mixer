


void  timer_init ( void );

unsigned int timer_tick ( void );


void wait_usec(unsigned int);

unsigned long get_system_timer();


/**
 * Wait N CPU cycles
 */
inline void wait_cycles(unsigned int n)
{
    if(n) while(n--) { asm volatile("nop"); }
}