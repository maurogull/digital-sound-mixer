


// ------------------------------------------------------------

.equ TT_S1_TABLE,          0x00000000000000003    // NSTable=0, PXNTable=0, UXNTable=0, APTable=0

// TT block entries templates   (L1 and L2, NOT L3)
// Assuming table contents:
// 0 = b01000100 = Normal, Inner/Outer Non-Cacheable
// 1 = b11111111 = Normal, Inner/Outer WB/WA/RA
// 2 = b00000000 = Device-nGnRnE
.equ TT_S1_FAULT,           0x0
.equ TT_S1_NORMAL_NO_CACHE, 0x00000000000000401    // Index = 0, AF=1
.equ TT_S1_NORMAL_WBWA,     0x00000000000000405    // Index = 1, AF=1
.equ TT_S1_DEVICE_nGnRnE,   0x00600000000000409    // Index = 2, AF=1, PXN=1, UXN=1

.equ TT_S1_UXN,             (1 << 54)
.equ TT_S1_PXN,             (1 << 53)
.equ TT_S1_nG,              (1 << 11)
.equ TT_S1_NS,              (1 << 5)

.equ TT_S1_NON_SHARED,      (0 << 8)               // Non-shareable
.equ TT_S1_INNER_SHARED,    (3 << 8)               // Inner-shareable
.equ TT_S1_OUTER_SHARED,    (2 << 8)               // Outer-shareable

.equ TT_S1_PRIV_RW,         (0x0)
.equ TT_S1_PRIV_RO,         (0x2 << 6)
.equ TT_S1_USER_RW,         (0x1 << 6)
.equ TT_S1_USER_RO,         (0x3 << 6)

// ------------------------------------------------------------





.macro GenerateMMUTables


  //
  // This example has a 4 entry L1 table, which points at four contigous L2 tables.
  // Allowing the L2 table to be treated as a single table.
  //

  // Generate Translation Table
  // ---------------------------

  //
  // Generate L1 table  (1 giga cada entrada)
  //
  
  LDR      x1, =tt_l1_base                   // Address of L1 table

  // [0]: 0x0000,0000 - 0x3FFF,FFFF
  LDR      x2, =tt_l2_base
  LDR      x0, =TT_S1_TABLE                 // pointer to next table entry
  ORR      x0, x0, x2
  STR      x0, [x1]

  // [1]: 0x4000,0000 - 0x7FFF,FFFF
  LDR      x0, =TT_S1_DEVICE_nGnRnE          // Entry template
  ORR      x0, x0, #0x40000000               
  STR      x0, [x1, #8]

  // [2]: 0x8000,0000 - 0xBFFF,FFFF 
  LDR      x0, =TT_S1_DEVICE_nGnRnE                   
  ORR      x0, x0, #0x80000000               
  STR      x0, [x1, #16]                  

  // [3]: 0xC000,0000 - 0xFFFF,FFFF 
  LDR      x0, =TT_S1_DEVICE_nGnRnE          
  ORR      x0, x0, #0xC0000000             
  STR      x0, [x1, #24]       


  //
  // Generate L2 table (2 MB cada entrada)
  //

  // This L2 table covers the address range:
  // 0x0000_0000 - 0x1FFF_FFFF
  //
  // primeros dos megas (un bloque)
  // 0x0000_0000 - 0x001F_FFFF  para cacheable

  LDR      x1, =tt_l2_base                   // Address of L1 table

  LDR      x0, =TT_S1_NORMAL_WBWA            // Entry template
  ORR      x0, x0, #TT_S1_INNER_SHARED       // 'OR' with attribute
  STR      x0, [x1]


  // resto de esta tabla todo device.. en 0x200000 debe estar la section noncacheable al linkear!
  LDR      x0, =TT_S1_DEVICE_nGnRnE
  ORR      x0, x0, #0x200000  
  STR      x0, [x1, #8]

  LDR      x0, =TT_S1_DEVICE_nGnRnE
  ORR      x0, x0, #0x400000  
  STR      x0, [x1, #16]

  LDR      x0, =TT_S1_DEVICE_nGnRnE
  ORR      x0, x0, #0x600000  
  STR      x0, [x1, #24]

  //no estamos usando mucho más, así que me da _pereza_ hacer en assembler el bucle que completa los 4096 bytes de la tabla JAJAJ



.endm



.macro InitMMU




 // Configure SCR_EL3
  // ------------------
  MOV      x0, #1                           // NS=1
  ORR      x0, x0, #(1 << 1)                // IRQ=1         IRQs routed to EL3
  ORR      x0, x0, #(1 << 2)                // FIQ=1         FIQs routed to EL3
  ORR      x0, x0, #(1 << 3)                // EA=1          SError routed to EL3
  ORR      x0, x0, #(1 << 8)                // HCE=1         HVC instructions are enabled
  ORR      x0, x0, #(1 << 10)               // RW=1          Next EL down uses AArch64
  ORR      x0, x0, #(1 << 11)               // ST=1          Secure EL1 can access CNTPS_TVAL_EL1, CNTPS_CTL_EL1 & CNTPS_CVAL_EL1
                                            // SIF=0         Secure state instruction fetches from Non-secure memory are permitted
                                            // SMD=0         SMC instructions are enabled
                                            // TWI=0         EL2, EL1 and EL0 execution of WFI instructions is not trapped to EL3
                                            // TWE=0         EL2, EL1 and EL0 execution of WFE instructions is not trapped to EL3
  MSR      SCR_EL3, x0


  /*
  // Install dummy vector table
  // ---------------------------
  LDR      x0, =vector_table
  MSR      VBAR_EL3, x0
  MSR      VBAR_EL2, x0
  MSR      VBAR_EL1, x0
  */

  DSB      SY
  ISB


  // Set the Base address
  // ---------------------
  LDR      x0, =tt_l1_base                  // Get address of level 1 for TTBR0_EL3
  MSR      TTBR0_EL3, x0                    // Set TTBR0_EL3 (NOTE: There is no TTBR1 at EL3)


  // Set up memory attributes
  // -------------------------
  // This equates to:
  // 0 = b01000100 = Normal, Inner/Outer Non-Cacheable
  // 1 = b11111111 = Normal, Inner/Outer WB/WA/RA
  // 2 = b00000000 = Device-nGnRnE
  MOV      x0, #0x000000000000FF44
  MSR      MAIR_EL3, x0


  // Set up TCR_EL3
  // ---------------
  MOV      x0, #32                          // T0SZ=0b011001 Limits VA space to 32 bits, translation starts @ l1
  ORR      x0, x0, #(0x1 << 8)              // IGRN0=0b01    Walks to TTBR0 are Inner WB/WA
  ORR      x0, x0, #(0x1 << 10)             // OGRN0=0b01    Walks to TTBR0 are Outer WB/WA
  ORR      x0, x0, #(0x3 << 12)             // SH0=0b11      Inner Shareable
                                            // TBI0=0b0      Top byte not ignored
                                            // TG0=0b00      4KB granule
                                            // IPS=0         32-bit PA space
  MSR      TCR_EL3, x0


  // Ensure changes to system register are visible before MMU enabled
  ISB
  

  // Invalidate TLBs
  // ----------------
  TLBI     ALLE3
  DSB      SY
  ISB






  DSB      SY

  // Enable MMU
  // -----------
  MOV      x0, #(1 << 0)                      // M=1 bit       Enable the stage 1 MMU
  ORR      x0, x0, #(1 << 2)                  // C=1 bit       Enable data and unified caches
  ORR      x0, x0, #(1 << 12)                 // I=1           Enable instruction fetches to allocate into unified caches
                                              // A=0           Strict alignment checking disabled
                                              // SA=0          Stack alignment checking disabled
                                              // WXN=0         Write permission does not imply XN
                                              // EE=0          EL3 data accesses are little endian
  MSR      SCTLR_EL3, x0
  ISB

  //
  // MMU is now enabled
  //

  NOP
  NOP
  NOP

.endm






// ***************************************
// SCTLR_EL1, System Control Register (EL1), Page 2654 of AArch64-Reference-Manual.
// ***************************************

#define SCTLR_RESERVED                  (3 << 28) | (3 << 22) | (1 << 20) | (1 << 11)
#define SCTLR_EE_LITTLE_ENDIAN          (0 << 25)
#define SCTLR_EOE_LITTLE_ENDIAN         (0 << 24)
#define SCTLR_I_CACHE_DISABLED          (0 << 12)
#define SCTLR_D_CACHE_DISABLED          (0 << 2)
#define SCTLR_I_CACHE_ENABLED           (1 << 12)
#define SCTLR_D_CACHE_ENABLED           (1 << 2)
#define SCTLR_MMU_DISABLED              (0 << 0)
#define SCTLR_MMU_ENABLED               (1 << 0)

#define SCTLR_VALUE_MMU_DISABLED	( SCTLR_EE_LITTLE_ENDIAN | SCTLR_I_CACHE_ENABLED | SCTLR_D_CACHE_ENABLED | SCTLR_MMU_DISABLED)

// ***************************************
// HCR_EL2, Hypervisor Configuration Register (EL2), Page 2487 of AArch64-Reference-Manual.
// ***************************************

#define HCR_RW	    			(1 << 31)


// mauro: https://developer.arm.com/documentation/102416/0100/EL1--Single-level-table
// FMO e IMO = 1  ??
#define HCR_VALUE			(HCR_RW  )



// ***************************************
// SCR_EL3, Secure Configuration Register (EL3), Page 2648 of AArch64-Reference-Manual.
// ***************************************

#define SCR_RESERVED	    		(3 << 4)
#define SCR_RW				(1 << 10)
#define SCR_NS				(1 << 0)



// mauro: https://developer.arm.com/documentation/102416/0100/EL1--Single-level-table
// << 11)  // ST=1  Secure EL1 can access timers
// << 8)   // HCE=1 HVC instructions are enabled
#define SCR_VALUE	    	    	(SCR_RESERVED | SCR_RW | SCR_NS |  (1 << 8) | (1 << 11))



// ***************************************
// SPSR_EL3, Saved Program Status Register (EL3) Page 389 of AArch64-Reference-Manual.
// ***************************************

#define SPSR_MASK_ALL 			(7 << 6)
#define SPSR_EL1h			(5 << 0)
#define SPSR_VALUE			(SPSR_MASK_ALL | SPSR_EL1h)





.macro WaitALittle
        mov     w0, 50880
        movk    w0, 0x5d, lsl 16
1:
        nop
        subs    w0, w0, #1
        bne     1b
.endm




.macro PasarAEL1
    

    // switch from EL3 down to EL1

    // Set up VMPIDR_EL2/VPIDR_EL1
    // ---------------------------
    MRS   x0, MIDR_EL1
    MSR   VPIDR_EL2, x0
    MRS   x0, MPIDR_EL1
    MSR   VMPIDR_EL2, x0
    
    
    // Set VMID
    // ---------
    // Although we are not using stage 2 translation, NS.EL1 still cares
    // about the VMID
    MSR   VTTBR_EL2, xzr


    // Set SCTLRs for EL1/2 to safe values
    // ------------------------------------
    MSR   SCTLR_EL2, xzr
    MSR   SCTLR_EL1, xzr



    ldr     x0, =SCTLR_VALUE_MMU_DISABLED
    msr	    sctlr_el1, x0		

    ldr     x0, =HCR_VALUE
    msr     hcr_el2, x0

    ldr     x0, =SCR_VALUE
    msr     scr_el3, x0

    ldr     x0, =SPSR_VALUE
    msr     spsr_el3, x0
    
    adr     x0, 1f		
    msr     elr_el3, x0

    eret			
1:
    nop

.endm




.globl _start
_start:
   


    // First enable the FPU
    mov     x0, #0x33ff
    msr     cptr_el3, x0 	 // Disable coprocessor traps to EL3
    mov     x0, #3 << 20
    msr     cpacr_el1, x0	 // Enable FP/SIMD at EL1



    // Disable trapping of CPTR_EL3 accesses or use of Adv.SIMD/FPU
    // -------------------------------------------------------------
    //MSR      CPTR_EL3, xzr
  


    mrs x0,mpidr_el1
    mov x1,#0xFF000000
    bic x0,x0,x1
    cbz x0,core_zero_dispatch
    sub x1,x0,#1
    cbz x1,core_one_dispatch
    sub x1,x0,#2
    cbz x1,core_two_dispatch
    sub x1,x0,#3
    cbz x1,core_three_dispatch

    b hang


core_zero_dispatch:          
    //b hang

    GenerateMMUTables
    InitMMU
    
    //PasarAEL1

    ldr x1, =0x1fc000 
    mov sp, x1 

    bl core0
    b hang 

core_one_dispatch:
    //b hang
    WaitALittle

    //PasarAEL1
    //GenerateMMUTables
    InitMMU
    ldr x1, =0x1fd000 
    mov sp, x1

    

    bl core1
    b hang


core_two_dispatch:
    //b hang
    WaitALittle
    WaitALittle

    //PasarAEL1

    //GenerateMMUTables
    InitMMU

    ldr x1, =0x1fe000 
    mov sp, x1 

    bl core2
    b hang

core_three_dispatch:
    //b hang
    WaitALittle
    WaitALittle
    WaitALittle

    //PasarAEL1  
    

    //GenerateMMUTables
    InitMMU

    LDR      x0, =vector_table
    MSR      VBAR_EL3, x0


    ldr x1, =0x1ff000 
    mov sp, x1

    bl core3
    b hang


hang: 
    nop
    b hang





/* "PROVIDE C FUNCTION: int getExceptionLevel (void);" */
.globl getExceptionLevel
.type getExceptionLevel, %function
getExceptionLevel:
    mrs x0, CurrentEL
    lsr x0, x0, #2
    ret



//"========================================================================="
//  semaphore_take -- AARCH64 Pi3 code
//  C Function: "void semaphore_take (uint32_t* sem);"
//  Entry: X0 will have semaphore address value
//  Return: nothing
//"========================================================================="
.section .text.semaphore_take, "ax", %progbits
.balign 4
.globl semaphore_take;
.type semaphore_take, %function
semaphore_take:
    nop
    mrs X4, daif                // Hold DAIF flags
    msr daifset, #2             // Disable Irq
    mov w2, #1
semaphore_take_loop:
    ldaxr     w1, [x0]
    cbnz     w1, semaphore_take_loop
    stxr       w3, w2, [x0]
    cbnz     w3, semaphore_take_loop
    dmb ish
    msr daif, X4                    // Restore DAIF flags to whatever they were
    ret


//"========================================================================="
//  semaphore_give -- Composite Pi1, Pi2 & Pi3 code
//  C Function: "void semaphore_give (uint32_t* sem);"
//  Entry: X0 will have semaphore address value
//  Return: nothing
//"========================================================================="
.section .text.semaphore_give, "ax", %progbits
.balign 4
.globl semaphore_give;
.type semaphore_give, %function
semaphore_give:
    stlrb   wzr, [x0]
    dmb ish
    ret
.balign 4
.ltorg  








.align 12

  .global vector_table
vector_table:

// ------------------------------------------------------------
// Current EL with SP0
// ------------------------------------------------------------
  .balign 128
sync_current_el_sp0:
  B        .                    //        Synchronous

  .balign 128
irq_current_el_sp0:
  B        .                    //        IRQ

  .balign 128
fiq_current_el_sp0:
  B        .                    //        FIQ

  .balign 128
serror_current_el_sp0:
  B        .                    //        SError

// ------------------------------------------------------------
// Current EL with SPx
// ------------------------------------------------------------

  .balign 128
sync_current_el_spx:
  B        .                    //        Synchronous

  .balign 128
irq_current_el_spx:
  B        .                    //        IRQ

  .balign 128
fiq_current_el_spx:
  B        .                    //        FIQ

  .balign 128
serror_current_el_spx:
  B        .                    //        SError

// ------------------------------------------------------------
// Lower EL using AArch64
// ------------------------------------------------------------

  .balign 128
sync_lower_el_aarch64:
   B        .                    

  .balign 128
irq_lower_el_aarch64:
  B        .                    //        IRQ

  .balign 128
fiq_lower_el_aarch64:
  B        .                    //        FIQ

  .balign 128
serror_lower_el_aarch64:
  B        .                    //        SError

// ------------------------------------------------------------
// Lower EL using AArch32
// ------------------------------------------------------------

  .balign 128
sync_lower_el_aarch32:
   B        .

  .balign 128
irq_lower_el_aarch32:
  B        .                    //        IRQ

  .balign 128
fiq_lower_el_aarch32:
  B        .                    //        FIQ

  .balign 128
serror_lower_el_aarch32:
  B        .                    //        SError







// ------------------------------------------------------------
// Translation tables
// ------------------------------------------------------------

  .section  .TT,"ax"
  .align 12

  .global tt_l1_base
tt_l1_base:
  .fill 4096 , 1 , 0

// ------------------------------------------------------------

  .global tt_l2_base
tt_l2_base:
  .fill 4096 , 1 , 0
