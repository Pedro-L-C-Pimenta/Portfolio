.data
barraN: .string "\n"

.text
.globl set_engine
.globl set_handbrake
.globl read_sensor_distance
.globl get_position
.globl get_rotation
.globl get_time
.globl puts
.globl gets
.globl atoi
.globl itoa
.globl strlen_custom
.globl approx_sqrt
.globl get_distance
.globl fill_and_pop
#int set_engine(int a0 = vertical, int a1 = horizontal);
set_engine:
    li a7, 10
    ecall
    ret
#fim

#int set_handbrake(char a0 = value);
set_handbrake:
    li t0, 48
    beq a0, t0, N_usar_freio
    li t0, 49
    beq a0, t0, Usar_freio
    li a0, -1
    ret
    N_usar_freio:
        li a0, 0
        j 1f
    Usar_freio:
        li a0, 1
    1:
    li a7, 11
    ecall
    li a0, 0
    ret
#fim

# int read_sensor_distance();
read_sensor_distance:
    li a7, 13
    ecall
    ret
#fim

#void get_position(a0 = int* x, a1 = int* y,a2 = int* z);
get_position:
    li a7, 15
    ecall
    ret
#fim

#void get_rotation(a0 = int* x, a1 = int* y,a2 = int* z);
get_rotation:
    li a7, 16
    ecall
    ret
#fim

#unsigned int get_time();
get_time:
    li a7, 20
    ecall
    ret
#fim

/*a0 = str para ser impressa*/
puts:
    mv s0, a0 
    1:
        lb t3, 0(s0)
        beq t3, x0, 2f      #quando chega no \0 estamos no final da string ent tem q por \n
            mv a0, s0
            li a1, 1
            li a7, 18   #write_serial
            ecall
            addi s0, s0, 1
            j 1b
        2:
        la a0, barraN
        li a1, 1
        li a7, 18    #write_serial
        ecall
        ret
#fim

/*a0 = buffer para guardar str*/
gets:
    mv s0, a0       #comeco da string
    mv s1, a0       #iteracao pelo vetor/string
    #null pointer = x0 = 0
    li s3, '\n'
    1:
        mv a0, s1       #buffer
        li a1, 1        #size
        li a7, 17       #read serial
        ecall
        lb t2, 0(s1)
        bne t2, s3, 2f
            sb x0, 0(s1)
            mv a0, s0
            ret
        2:
        addi s1, s1, 1
        j 1b
#fim

/*a0 = string para ser convertida*/
atoi:
    mv t0, a0
    li a0, 0
    li t2, '\n'
    li t5, '-'
    li t6, '+'
    li t4, 10
    li a1, 1 
    li a2, 48
    li a3, 58
    1:
        lb t3, 0(t0)
        bne t3, x0, 2f
            mul a0, a0, a1      #poe sinal
            ret
        2:
        bne t3, t5, 4f
            li a1, -1
            j 3f
        4:
        beq t3, t6, 3f
        blt t3, a2, 3f
        bge t3, a3, 3f
            addi t3, t3, -48    #passa pra numero
            mul a0, a0, t4      #poe na base certa
            add a0, a0, t3      #soma com oq já temos
        3:
            addi t0, t0, 1
            j 1b
#fim

/*a0 = value, a1 = buffer, a2 = base*/
itoa:
    mv t0, a1
    mv t3, sp
    li t2, 10
    bne a2, t2, 1f
    bge a0, x0, 1f      #quando o numero e negativo
        li t1, '-'
        sb t1, 0(a1)
        addi a1, a1, 1
        li t1, -1
        mul a0, a0, t1
    1:
    rem t1, a0, a2
    bge t1, t2, 2f          #se for maior que 10 pula
        addi t1, t1, 48     #numero de 0 a 9
        j 3f
    2:
        addi t1, t1, 55     #numero de 10 a 15
    3:
    addi sp, sp, -1
    sb t1, 0(sp)
    div a0, a0, a2
    beq a0, x0, tostr
        j 1b
    tostr:
        lb t1, 0(sp)
        addi sp, sp, 1
        sb t1, 0(a1)
        addi a1, a1, 1
        bne sp, t3, tostr
            li t1, 0
            sb t1, 0(a1)
            mv a0, t0
            ret
#fim

# int strlen_custom( a0 = char *str )
strlen_custom:
    mv t0, a0
    li a0, 0
    1:
    bne t0, x0, 1f
        addi a0, a0, 1
        j 1b
    1:
    ret
#fim

/*Passar a0 (y), a1 (n iteracoes)*/
approx_sqrt:
        #k = y/2
        mv t0, a0 #y:t0
        srai a0, a0, 1 #k:a0 k = y/2
    loop_do_K:
        #k = (k+y/k)/2
        div t1, t0, a0  # => y/k
        add t2, a0, t1 # => k + y/k
        srai a0, t2, 1 # => (k+y/k)/2
        #fizemos uma interacao
        addi a1, a1, -1 #subitraimos 1 do contador
        bne a1, x0, loop_do_K #recomecamos o loop
        ret
#fim

# int get_distance(a0 =int x_a, a1 =int y_a,a2 =int z_a, a3 =int x_b, a4 =int y_b, a5 =int z_b);
get_distance:
    addi sp, sp, -4
    sw ra, 0(sp)

    #xa-xb
    sub t0, a0, a3
    mul t0, t0, t0 #(xa-xb)²
    #ya-yb
    sub t1, a1, a4
    mul t1, t1, t1 #(ya-yb)²
    #za-zb
    sub t2, a2, a5
    mul t2, t2, t2 #(za-zb)²
    #(xa-xb)²+(ya-yb)²
    add a0, t0, t1
    #(xa-xb)²+(ya-yb)²+(za-zb)²
    add a0, a0, t2
    li a1, 10
    jal approx_sqrt     #pega a raiz aproximada 

    lw ra, 0(sp)
    addi sp, sp, 4
    ret
#fim

# A_0: 
#     .word -16 # x
#     .word 1   # y 
#     .word 7   # z
#     .word 0   # a_x
#     .word 93  # a_y
#     .word 0   # a_z
#     .word 0   # action
#     .word A_1 # *next
# Node *fill_and_pop(a0 = Node *head, a1 = Node *fill);
fill_and_pop:
    lw t0, 0(a0)    #x
    sw t0, 0(a1)    #copia_x
    
    lw t0, 4(a0)    #y
    sw t0, 4(a1)    #copia_y
    
    lw t0, 8(a0)    #z
    sw t0, 8(a1)    #copia_z
    
    lw t0, 12(a0)   #a_x
    sw t0, 12(a1)   #copia_a_x
    
    lw t0, 16(a0)   #a_y
    sw t0, 16(a1)   #copia_a_y
    
    lw t0, 20(a0)   #a_z
    sw t0, 20(a1)   #copia_a_z
    
    lw t0, 24(a0)   #action
    sw t0, 24(a1)   #copia_action

    lw t0, 28(a0)   #*next
    sw t0, 28(a1)   #*copia_next

    mv a0, t0       #pega o próximo node e da return
    ret
#fim