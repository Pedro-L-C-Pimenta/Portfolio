.section .bss
.align 4
isr_stack: # Final da pilha das ISRs
.skip 1024 # Aloca 1024 bytes para a pilha
isr_stack_end: # Base da pilha das ISRs

.section .data
_system_time: .skip 4

.section .text
.set base, 0xffff0300
.set wait, 200
.globl _start
_start:
    #system stack
    la t0, isr_stack_end # t0 <= base da pilha
    csrw mscratch, t0 # mscratch <= t0

    #user stack
    li sp, 0x07FFFFFC
    
    #registrando a ISR
    la t0, main_isr # Carrega o endereço da main_isr
    csrw mtvec, t0 # em mtvec

    # Habilita Interrupções Externas
    csrr t1, mie # Seta o bit 11 (MEIE)
    li t2, 0x800 # do registrador mie
    or t1, t1, t2
    csrw mie, t1

    # Habilita Interrupções Globais
    csrr t1, mstatus # Seta o bit 3 (MIE)
    ori t1, t1, 0x8 # do registrador mstatus
    csrw mstatus, t1

    #system user mode
    csrr t1, mstatus # Update the mstatus.MPP
    li t2, ~0x1800 # field (bits 11 and 12)
    and t1, t1, t2 # with value 00 (U-mode)
    csrw mstatus, t1

    la t0, main # Loads the user software
    csrw mepc, t0 # entry point into mepc
    mret # PC <= MEPC; mode <= MPP;


.globl main_isr
main_isr:
     # Salvar o contexto
    csrrw sp, mscratch, sp # Troca sp com mscratch
    addi sp, sp, -4
    sw ra, 0(sp)

    # Trata a interrupção
    #indentifica qual dos casos que estamos lidando
    li t0, 10
    beq a7, t0, Syscall_set_engine_and_steering
    li t0, 11
    beq a7, t0, Syscall_set_handbrake
    li t0, 12
    beq a7, t0, Syscall_read_sensors
    li t0, 13
    beq a7, t0, Syscall_read_sensor_distance
    li t0, 15
    beq a7, t0, Syscall_get_position
    li t0, 16
    beq a7, t0, Syscall_get_rotation
    li t0, 17
    beq a7, t0, Syscall_read_serial
    li t0, 18
    beq a7, t0, Syscall_write_seral
    li t0, 20
    beq a7, t0, Syscall_get_systime
    j termina       #se chegou aqui é pq chamou o ecall errado kkkkkkkkkk

    # a0 = engine, a1 = wheel, a7 = 10, a0 (ret) = value #
    Syscall_set_engine_and_steering:
    li t0, -1
    li t1, 2
    li t2, -127
    li t3, 128
    blt a0, t0, 1f
    bge a0, t1, 1f
    blt a1, t2, 1f
    bge a1, t3, 1f
        li t0, base         #Base
        addi t0, t0, 0x20     #wheel 
        sb a1, 0(t0)        #sets an angle in the Wheel
        sb a0, 1(t0)        #stores a0 in the engine (wheel + 1)
        li a0, 0
        j termina
    1:
        li a0, -1
        j termina

    # a0 = num, a7 = 11
    Syscall_set_handbrake:
    li t0, base         #Base
    addi t0, t0, 0x22   #hand brake 
    sb a0, 0(t0)        #activate the hand brake
    j termina

    # a0 = adress, a7 = 12 
    Syscall_read_sensors:
    li t0, base         #Base
    addi t0, t0, 0x01   #line camera
    li t1, 1
    sw t1, 0(t0)        #triggers the line camera 
    li t2, wait
    1:
        addi t2, t2, -1
        bnez t2, 1b
    addi t0, t0, 0x23   #array where the image is stored
    li t2, 256
    1:
    lw t3, 0(t0)
    sw t3, 0(a0)
    addi t0, t0, 4
    addi a0, a0, 4
    addi t3, t3, -4
    bne t3, x0, 1b
    j termina

    # a0 (ret) = value , a7 = 13
    Syscall_read_sensor_distance:
    li t0, base         #Base
    addi t0, t0, 0x02   #ultrasonic sensors
    li t1, 1
    sw t1, 0(t0)        #triggers the ultrasonic sensors 
    li t2, wait         #busy wait
    1:
        addi t2, t2, -1
        bnez t2, 1b
    li t0, base         #Base
    addi t0, t0, 0x1c   #stores the distance 
    lw a0, 0(t0)
    j termina

    Syscall_get_position:
    li t0, base
    li t1, 1
    sw t1, 0(t0)
    li t2, wait         #busy wait
    1:
        addi t2, t2, -1
        bnez t2, 1b
    li t0, base
    addi t0, t0, 0x10   #stores the x-axis
    lw t2, 0(t0)
    sw t2, 0(a0)        #where the user wants to keep the X
    addi t0, t0, 0x4    #stores the y-axis
    lw t2, 0(t0)
    sw t2, 0(a1)        #where the user wants to keep the Y
    addi t0, t0, 0x4    #stores the z-axis
    lw t2, 0(t0)
    sw t2, 0(a2)        #where the user wants to keep the Z
    j termina

    Syscall_get_rotation:
    li t0, base
    li t1, 1
    sw t1, 0(t0)
    li t2, wait         #busy wait
    1:
        addi t2, t2, -1
        bnez t2, 1b
    li t0, base
    addi t0, t0, 0x04   #stores the x-angle
    lw t2, 0(t0)
    sw t2, 0(a0)        #where the user wants to keep the X
    addi t0, t0, 0x4    #stores the y-angle
    lw t2, 0(t0)
    sw t2, 0(a1)        #where the user wants to keep the Y
    addi t0, t0, 0x4    #stores the z-angle
    lw t2, 0(t0)
    sw t2, 0(a2)        #where the user wants to keep the Z
    j termina

    Syscall_read_serial:
    li t3, 0
    3:      #while t3 < a1
        li t0, 0xFFFF0502   #serial port read position
        li t1, 1
        sb t1, 0(t0)
        1:
            lb t2, 0(t0)
            bnez t2, 1b
        addi t0, t0, 0x01   #position  of the byte read
        lb t1, 0(t0)
        beq t1, x0, 1f
            sb t1, 0(a0)
            addi a0, a0, 1
            addi t3, t3, 1
            j 2f
        1:
            sb x0, 0(a0)
            mv a0, t3
            j termina
        2:
    blt t3, a1, 3b
    mv a0, t3
    j termina

    Syscall_write_seral:
    li t3, 0
    2:      #while t3 < a1
        li t0, 0xFFFF0501   #byte to be written
        lb t1, 0(a0)
        sb t1, 0(t0)
        addi t0, t0, -1     #write position
        li t1, 1
        sb t1, 0(t0)
        1:
            lb t2, 0(t0)        #busy wait
            bnez t2, 1b
        addi a0, a0, 1
        addi t3, t3, 1
    blt t3, a1, 2b
    j termina

    Syscall_get_systime:
    li t0, 0xFFFF0100
    li t1, 1
    sw t1, 0(t0)
    li t2, wait         #busy wait
    1:
        addi t2, t2, -1
        bnez t2, 1b
    addi t0, t0, 4
    lw a0, 0(t0)
    j termina

    termina:
    lw ra, 0(sp)
    addi sp, sp, 4
    csrrw sp, mscratch, sp # Troca sp com mscratch novamente
    csrr t0, mepc  # load return address (address of the instruction that invoked the syscall)
    addi t0, t0, 4 # adds 4 to the return address (to return after ecall) 
    csrw mepc, t0  # stores the return address back on mepc
    mret           # Recover remaining context (pc <- mepc)
#fim