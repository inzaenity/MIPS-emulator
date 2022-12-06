
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <errno.h>


#define MAX_LINE_LENGTH 256
#define INSTRUCTIONS_GROW 64


// ADD YOUR #defines HERE


void execute_instructions(int n_instructions,
                          uint32_t instructions[n_instructions],
                          int trace_mode);
char *process_arguments(int argc, char *argv[], int *trace_mode);
uint32_t *read_instructions(char *filename, int *n_instructions_p);
uint32_t *instructions_realloc(uint32_t *instructions, int n_instructions);


// ADD YOUR FUNCTION PROTOTYPES HERE
//functions for decoding instructions
int runInstruction(uint32_t command, uint32_t registers[]);
void syscall (uint32_t n, uint32_t registers[]);
void printOri (uint32_t command, uint32_t registers[]);
void printMul (uint32_t command, uint32_t registers[]);
void printAdd (uint32_t command, uint32_t registers[]);
void printSub (uint32_t command, uint32_t registers[]);
void printSlt (uint32_t command, uint32_t registers[]);
uint32_t printBeq (uint32_t command, uint32_t registers[]);
uint32_t printBne (uint32_t command, uint32_t registers[]);
void printAddi (uint32_t command, uint32_t registers[]);
void printLui(uint32_t command, uint32_t registers[]);
// functions for when r inputted
int rrunIfinnstruction(uint32_t command, uint32_t registers[]);
void rsyscall (uint32_t n, uint32_t registers[]);
void rprintOri (uint32_t command, uint32_t registers[]);
void rprintMul (uint32_t command, uint32_t registers[]);
void rprintAdd (uint32_t command, uint32_t registers[]);
void rprintSub (uint32_t command, uint32_t registers[]);
void rprintSlt (uint32_t command, uint32_t registers[]);
uint32_t rprintBeq (uint32_t command, uint32_t registers[]);
uint32_t rprintBne (uint32_t command, uint32_t registers[]);
void rprintAddi (uint32_t command, uint32_t registers[]);
void rprintLui(uint32_t command, uint32_t registers[]);

// YOU SHOULD NOT NEED TO CHANGE MAIN

int main(int argc, char *argv[]) {
    int trace_mode;
    char *filename = process_arguments(argc, argv, &trace_mode);

    int n_instructions;
    uint32_t *instructions = read_instructions(filename, &n_instructions);

    execute_instructions(n_instructions, instructions, trace_mode);

    free(instructions);
    return 0;
}


// simulate execution of  instruction codes in  instructions array
// output from syscall instruction & any error messages are printed
//
// if trace_mode != 0:
//     information is printed about each instruction as it executed
//
// execution stops if it reaches the end of the array
// going to need an array to track registers
void execute_instructions(int n_instructions,
                          uint32_t instructions[n_instructions],
                          int trace_mode) {
    // REPLACE CODE BELOW WITH YOUR CODE
   

    uint32_t registers[32] = {0};
    int pc = 0;

    while (pc < n_instructions) {
        //if r is not inputted
        if (trace_mode) {
            printf("%d: 0x%08X ", pc, instructions[pc]);
            int temp = pc;
            pc = pc + runInstruction(instructions[pc], registers);
            //if pc changed
            if (pc != temp) {
                printf(">>> branch taken to PC = %d\n", pc+1);
            }
            //if pc goes outside instructions array
            if (pc < 0 || pc > n_instructions) {
                printf("Illegal branch to address before instructions: PC = %d\n", pc+1);
                pc = temp;
            }
        }
        //if r is inputted
        else if (trace_mode == 0) {
            int temp = pc;
            pc = pc + rrunInstruction(instructions[pc], registers);
            if (pc < 0 || pc > n_instructions) {
                printf("Illegal branch to address before instructions: PC = %d\n", pc+1);
                pc = temp;
            }
        }
        pc++;
    }
}
//function which determines which command
//returns a number which is not 0 if PC is changed
int runInstruction(uint32_t command, uint32_t registers[]) {
    //syscall
    if (command == 12) {
        printf("syscall\n");
        printf(">>> syscall %d\n", registers[2]);
        syscall(registers[2], registers);
        return 0;
        
    }
    else {
        uint32_t firstSix = command;
        firstSix = command >> 26;
        if (firstSix == 0) {
            uint32_t lastEleven;
            lastEleven = command & 0x7ff;
            //add
            if(lastEleven == 32) {
                printAdd(command, registers);
                return 0;
                 
            }
            //sub
            if(lastEleven == 34) {
                printSub(command, registers);
                return 0;

            }
            //slt
            if(lastEleven == 42) {
                printSlt(command, registers);
                return 0;
            } 
            else {
                printf("invalid instruction code\n");
                exit(0);
            }
        }
        //mul
        else if (firstSix == 28) {
            printMul(command, registers);
            return 0;
        }
        //beq
        else if (firstSix == 4) {
            return printBeq(command, registers);

        }
        //bne
        else if (firstSix == 5) {
            return printBne(command, registers);
        }
        //addi
        else if (firstSix == 8) {
            printAddi(command, registers);
            return 0;

        }
        //ori
        else if (firstSix == 13) {
            printOri(command, registers);
            
            return 0;

        }
        //lui
        else if (firstSix == 15) {
            printLui(command, registers);
            return 0;
        }
        else {
            printf("invalid instruction code\n");
            exit(0);
        }
    }
    return 0;
}


// ADD YOUR FUNCTIONS HERE
//FUNCTIONS TO PRINT AND ALLOCATE REGISTERS
void syscall (uint32_t n, uint32_t registers[]) {
    if (n == 1) {
        printf("<<< %d\n", registers[4]);    
    }
    else if (n == 10) {
        exit(0);
    }
    else if (n == 11) {
        printf("<<< %c\n", registers[4]);
    }
    else {
        printf("Unknown system call: %d\n", n);
        exit(0);
    }
}

void printOri (uint32_t command, uint32_t registers[]) {
    printf("ori  ");
    //Bit pattern is 001101ssssstttttIIIIIIIIIIIIIIII
    uint32_t regNumbers = command >> 16;
    
    //decode t the $t register
    uint32_t t = regNumbers & 0x1f;
    printf("$%d, ", t);
    //decode s for $s register
    regNumbers = regNumbers >> 5;
    uint32_t s = regNumbers & 0x1f;
    printf("$%d, ", s);

    //find immediate value
    uint32_t immediate = command & 0xffff;
    int sign = command >> 15;
    sign = sign & 1;
    if (sign == 0) {
         printf("%d \n", immediate);
    }
    else {
    immediate = ((immediate ^ 0xffff) + 1) * -1;
    printf("%d \n", immediate);
    }
    //change registers 
    int32_t value = registers[s] | immediate;
    registers[t] = value;
    printf(">>> $%d = %d\n", t, registers[t]);
    registers[0] = 0;
}

void printMul (uint32_t command, uint32_t registers[]) {
    printf("mul  ");
    //Bit pattern is 011100ssssstttttddddd00000000010
    uint32_t regNumbers = command >> 11;
    //decode d the $d register
    uint32_t d = regNumbers & 0x1f;
    printf("$%d, ", d);
    
    //decode s for $s register
    regNumbers = regNumbers >> 10;
    uint32_t s = regNumbers & 0x1f;
    printf("$%d, ", s);
    
    //decode t the $t register
    regNumbers = command >> 16;
    uint32_t t = regNumbers & 0x1f;
    printf("$%d\n", t);

    //change registers
    uint32_t value = registers[t] * registers[s];
    registers[d] = value;
    printf(">>> $%d = %d\n", d, registers[d]);
    registers[0] = 0;
}

void printAdd (uint32_t command, uint32_t registers[]) {
    printf("add  ");
    //Bit pattern is 000000ssssstttttddddd00000100000
    uint32_t regNumbers = command >> 11;
    //decode d the $d register
    uint32_t d = regNumbers & 0x1f;
    printf("$%d, ", d);
    
    //decode s for $s register
    regNumbers = regNumbers >> 10;
    uint32_t s = regNumbers & 0x1f;
    printf("$%d, ", s);
    
    //decode t the $t register
    regNumbers = command >> 16;
    uint32_t t = regNumbers & 0x1f;
    printf("$%d\n", t);

    //change registers
    uint32_t value = registers[t] + registers[s];
    registers[d] = value;
    printf(">>> $%d = %d\n", d, registers[d]);
    registers[0] = 0;
}

void printSub (uint32_t command, uint32_t registers[]) {
    printf("sub  ");
    //Bit pattern is 000000ssssstttttddddd00000100010
    uint32_t regNumbers = command >> 11;
    //decode d the $d register
    uint32_t d = regNumbers & 0x1f;
    printf("$%d, ", d);
    
    //decode s for $s register
    regNumbers = regNumbers >> 10;
    uint32_t s = regNumbers & 0x1f;
    printf("$%d, ", s);
    
    //decode t the $t register
    regNumbers = command >> 16;
    uint32_t t = regNumbers & 0x1f;
    printf("$%d\n", t);
    //change registers
    uint32_t value = registers[s] - registers[t];
    registers[d] = value;
    printf(">>> $%d = %d\n", d, registers[d]);
    registers[0] = 0;
}

void printSlt (uint32_t command, uint32_t registers[]) {
    printf("slt  ");
    //Bit pattern is 000000ssssstttttddddd00000101010
    uint32_t regNumbers = command >> 11;
    //decode d the $d register
    uint32_t d = regNumbers & 0x1f;
    printf("$%d, ", d);
    
    //decode s for $s register
    regNumbers = regNumbers >> 10;
    uint32_t s = regNumbers & 0x1f;
    printf("$%d, ", s);
    
    //decode t the $t register
    regNumbers = command >> 16;
    uint32_t t = regNumbers & 0x1f;
    printf("$%d\n", t);
    
    //change registers
    if (registers[s] < registers[t]) {
        registers[d] = 1;
    }
    else {
        registers[d] = 0;
    }
    printf(">>> $%d = %d\n", d, registers[d]);
    registers[0] = 0;
}

uint32_t printBeq (uint32_t command, uint32_t registers[]) {
    printf("beq  ");
    //Bit pattern is 000100ssssstttttIIIIIIIIIIIIIIII
    uint32_t regNumbers = command >> 16;
    //decode s for $s register
    regNumbers = regNumbers >> 5;
    uint32_t s = regNumbers & 0x1f;
    printf("$%d, ", s);
    //decode t the $t register
    regNumbers = command >> 16;
    uint32_t t = regNumbers & 0x1f;
    printf("$%d, ", t);
    
    
    //find immediate value
    uint32_t immediate = command & 0xffff;
    int sign = command >> 15;
    sign = sign & 1;
    if (sign == 0) {
         printf("%d \n", immediate);
    }
    else {
    immediate = ((immediate ^ 0xffff) + 1) * -1;
    printf("%d \n", immediate);
    }
    //return change to pc value
    if (registers[t] == registers[s]) {
        return immediate - 1;
    }
    else {
        printf("branch not taken\n");
        return 0;
    }
}

uint32_t printBne (uint32_t command, uint32_t registers[]) {
    printf("bne  ");
    //Bit pattern is 000101ssssstttttIIIIIIIIIIIIIIII
    uint32_t regNumbers = command >> 16;
    
     //decode s for $s register
    regNumbers = regNumbers >> 5;
    uint32_t s = regNumbers & 0x1f;
    printf("$%d, ", s);

    //decode t the $t register
    regNumbers = command >> 16;
    uint32_t t = regNumbers & 0x1f;
    printf("$%d, ", t);

    //find immediate value
    uint32_t immediate = command & 0xffff;
    int sign = command >> 15;
    sign = sign & 1;
    if (sign == 0) {
         printf("%d \n", immediate);
    }
    else {
    immediate = ((immediate ^ 0xffff) + 1) * -1;
    printf("%d \n", immediate);
    }
    //return change to pc value
    if (registers[t] != registers[s]) {
        return immediate - 1;
    }
    else {
        printf(">>> branch not taken\n");
        return 0;
    }
}

void printAddi (uint32_t command, uint32_t registers[]) {
    printf("addi ");
    //Bit pattern is 001000ssssstttttIIIIIIIIIIIIIIII
    uint32_t regNumbers = command >> 16;
    
    //decode t the $t register
    uint32_t t = regNumbers & 0x1f;
    printf("$%d, ", t);
    //decode s for $s register
    regNumbers = regNumbers >> 5;
    uint32_t s = regNumbers & 0x1f;
    printf("$%d, ", s);

    //find immediate value
    uint32_t immediate = command & 0xffff;
    int sign = command >> 15;
    sign = sign & 1;
    if (sign == 0) {
         printf("%d \n", immediate);
    }
    else {
    immediate = ((immediate ^ 0xffff) + 1) * -1;
    printf("%d \n", immediate);
    }
    //change registers value
    uint32_t value = registers[s] + immediate;
    registers[t] = value;
    printf(">>> $%d = %d\n", t, registers[t]);
    registers[0] = 0;
}
void printLui(uint32_t command, uint32_t registers[]) {
     //Bit pattern is 00111100000tttttIIIIIIIIIIIIIIII
     printf("lui  ");
    
    uint32_t regNumbers = command >> 16;
    
    //decode t the $t register
    uint32_t t = regNumbers & 0x1f;
    printf("$%d, ", t);

    //find immediate value
    uint32_t immediate = command & 0xffff;
    int sign = command >> 15;
    sign = sign & 1;
    if (sign == 0) {
         printf("%d \n", immediate);
    }
    else {
        immediate = ((immediate ^ 0xffff) + 1) * -1;
        printf("%d \n", immediate);
    }
    //change registers value
    uint32_t value = immediate << 16;
    registers[t] = value;
    registers[0] = 0;
    printf(">>> $%d = %d\n", t, registers[t]);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                        Functions for when r is inputted   (same but with print function removed)                              //
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int rrunInstruction(uint32_t command, uint32_t registers[]) {
    //syscall
    if (command == 12) {
        
        rsyscall(registers[2], registers);
        return 0;
        
    }
    else {
        uint32_t firstSix = command;
        firstSix = command >> 26;
        if (firstSix == 0) {
            uint32_t lastEleven;
            lastEleven = command & 0x7ff;
            //add
            if(lastEleven == 32) {
                rprintAdd(command, registers);
                return 0;
                 
            }
            //sub
            if(lastEleven == 34) {
                rprintSub(command, registers);
                return 0;

            }
            //slt
            if(lastEleven == 42) {
                rprintSlt(command, registers);
                return 0;
            } 
            else {
                printf("invalid instruction code\n");
                exit(0);
            }
        }
        //mul
        else if (firstSix == 28) {
            rprintMul(command, registers);
            return 0;
        }
        //beq
        else if (firstSix == 4) {
            return rprintBeq(command, registers);

        }
        //bne
        else if (firstSix == 5) {
            return rprintBne(command, registers);
        }
        //addi
        else if (firstSix == 8) {
            rprintAddi(command, registers);
            return 0;

        }
        //ori
        else if (firstSix == 13) {
            rprintOri(command, registers);
            
            return 0;

        }
        //lui
        else if (firstSix == 15) {
            rprintLui(command, registers);
            return 0;
        }
    }
    return 0;
}


// ADD YOUR FUNCTIONS HERE
void rsyscall (uint32_t n, uint32_t registers[]) {
    if (n == 1) {
        printf("%d", registers[4]);    
    }
    else if (n == 10) {
        exit(0);
    }
    else if (n == 11) {
        printf("%c", registers[4]);
    }
    else {
        printf("Unknown system call: %d\n", n);
        exit(0);
    }
}

void rprintOri (uint32_t command, uint32_t registers[]) {
    
    //Bit pattern is 001101ssssstttttIIIIIIIIIIIIIIII
    uint32_t regNumbers = command >> 16;
    
    //decode t the $t register
    uint32_t t = regNumbers & 0x1f;
    
    //decode s for $s register
    regNumbers = regNumbers >> 5;
    uint32_t s = regNumbers & 0x1f;
    

    //find immediate value
    uint32_t immediate = command & 0xffff;
    int sign = command >> 15;
    sign = sign & 1;

    if(sign == 1) {
        immediate = ((immediate ^ 0xffff) + 1) * -1;
    }
    //change registers 
    int32_t value = registers[s] | immediate;
    registers[t] = value;
    
    registers[0] = 0;
}

void rprintMul (uint32_t command, uint32_t registers[]) {
   
    //Bit pattern is 011100ssssstttttddddd00000000010
    uint32_t regNumbers = command >> 11;
    //decode d the $d register
    uint32_t d = regNumbers & 0x1f;
    
    
    //decode s for $s register
    regNumbers = regNumbers >> 10;
    uint32_t s = regNumbers & 0x1f;
    
    
    //decode t the $t register
    regNumbers = command >> 16;
    uint32_t t = regNumbers & 0x1f;
   

    //change registers
    uint32_t value = registers[t] * registers[s];
    registers[d] = value;
    
    registers[0] = 0;
}

void rprintAdd (uint32_t command, uint32_t registers[]) {
    
    //Bit pattern is 000000ssssstttttddddd00000100000
    uint32_t regNumbers = command >> 11;
    //decode d the $d register
    uint32_t d = regNumbers & 0x1f;
    
    
    //decode s for $s register
    regNumbers = regNumbers >> 10;
    uint32_t s = regNumbers & 0x1f;
    
    
    //decode t the $t register
    regNumbers = command >> 16;
    uint32_t t = regNumbers & 0x1f;
    

    //change registers
    uint32_t value = registers[t] + registers[s];
    registers[d] = value;
    
    registers[0] = 0;
}

void rprintSub (uint32_t command, uint32_t registers[]) {
    
    //Bit pattern is 000000ssssstttttddddd00000100010
    uint32_t regNumbers = command >> 11;
    //decode d the $d register
    uint32_t d = regNumbers & 0x1f;
   
    
    //decode s for $s register
    regNumbers = regNumbers >> 10;
    uint32_t s = regNumbers & 0x1f;
    
    
    //decode t the $t register
    regNumbers = command >> 16;
    uint32_t t = regNumbers & 0x1f;
    
    //change registers
    uint32_t value = registers[s] - registers[t];
    registers[d] = value;
    
    registers[0] = 0;
}

void rprintSlt (uint32_t command, uint32_t registers[]) {
    
    //Bit pattern is 000000ssssstttttddddd00000100010
    uint32_t regNumbers = command >> 11;
    //decode d the $d register
    uint32_t d = regNumbers & 0x1f;
    
    
    //decode s for $s register
    regNumbers = regNumbers >> 10;
    uint32_t s = regNumbers & 0x1f;
    
    
    //decode t the $t register
    regNumbers = command >> 16;
    uint32_t t = regNumbers & 0x1f;
    
    
    //change registers
    if (registers[s] < registers[t]) {
        registers[d] = 1;
    }
    else {
        registers[d] = 0;
    }
    
    registers[0] = 0;
}

uint32_t rprintBeq (uint32_t command, uint32_t registers[]) {
    
    //Bit pattern is 000100ssssstttttIIIIIIIIIIIIIIII
    uint32_t regNumbers = command >> 16;
    //decode s for $s register
    regNumbers = regNumbers >> 5;
    uint32_t s = regNumbers & 0x1f;
    
    //decode t the $t register
    regNumbers = command >> 16;
    uint32_t t = regNumbers & 0x1f;
    
    
    
    //find immediate value
    uint32_t immediate = command & 0xffff;
    int sign = command >> 15;
    sign = sign & 1;
    if (sign == 0) {
         
    }
    else {
    immediate = ((immediate ^ 0xffff) + 1) * -1;
    
    }
    if (registers[t] == registers[s]) {
        return immediate - 1;
    }
    else {
        
        return 0;
    }
}

uint32_t rprintBne (uint32_t command, uint32_t registers[]) {
    
    //Bit pattern is 000100ssssstttttIIIIIIIIIIIIIIII
    uint32_t regNumbers = command >> 16;
    
     //decode s for $s register
    regNumbers = regNumbers >> 5;
    uint32_t s = regNumbers & 0x1f;
    

    //decode t the $t register
    regNumbers = command >> 16;
    uint32_t t = regNumbers & 0x1f;
    

    //find immediate value
    uint32_t immediate = command & 0xffff;
    int sign = command >> 15;
    sign = sign & 1;
    if (sign == 0) {
         
    }
    else {
    immediate = ((immediate ^ 0xffff) + 1) * -1;
    
    }

    if (registers[t] != registers[s]) {
        return immediate - 1;
    }
    else {
        
        return 0;
    }
}

void rprintAddi (uint32_t command, uint32_t registers[]) {
    
    //Bit pattern is 001101ssssstttttIIIIIIIIIIIIIIII
    uint32_t regNumbers = command >> 16;
    
    //decode t the $t register
    uint32_t t = regNumbers & 0x1f;
   
    //decode s for $s register
    regNumbers = regNumbers >> 5;
    uint32_t s = regNumbers & 0x1f;
    

    //find immediate value
    uint32_t immediate = command & 0xffff;
    int sign = command >> 15;
    sign = sign & 1;

    if (sign == 1) {
    immediate = ((immediate ^ 0xffff) + 1) * -1;
    
    }

    uint32_t value = registers[s] + immediate;
    registers[t] = value;
    
    registers[0] = 0;
}
void rprintLui(uint32_t command, uint32_t registers[]) {
     //Bit pattern is 00111100000tttttIIIIIIIIIIIIIIII
     
    
    uint32_t regNumbers = command >> 16;
    
    //decode t the $t register
    uint32_t t = regNumbers & 0x1f;
    

    //find immediate value
    uint32_t immediate = command & 0xffff;
    int sign = command >> 15;
    sign = sign & 1;

    if(sign == 1) {
        immediate = ((immediate ^ 0xffff) + 1) * -1;
        
    }
    uint32_t value = immediate << 16;
    registers[t] = value;
    registers[0] = 0;
    
}

// YOU DO NOT NEED TO CHANGE CODE BELOW HERE


// check_arguments is given command-line arguments
// it sets *trace_mode to 0 if -r is specified
//          *trace_mode is set to 1 otherwise
// the filename specified in command-line arguments is returned

char *process_arguments(int argc, char *argv[], int *trace_mode) {
    if (
        argc < 2 ||
        argc > 3 ||
        (argc == 2 && strcmp(argv[1], "-r") == 0) ||
        (argc == 3 && strcmp(argv[1], "-r") != 0)) {
        fprintf(stderr, "Usage: %s [-r] <file>\n", argv[0]);
        exit(1);
    }
    *trace_mode = (argc == 2);
    return argv[argc - 1];
}


// read hexadecimal numbers from filename one per line
// numbers are return in a malloc'ed array
// *n_instructions is set to size of the array

uint32_t *read_instructions(char *filename, int *n_instructions_p) {
    FILE *f = fopen(filename, "r");
    if (f == NULL) {
        fprintf(stderr, "%s: '%s'\n", strerror(errno), filename);
        exit(1);
    }

    uint32_t *instructions = NULL;
    int n_instructions = 0;
    char line[MAX_LINE_LENGTH + 1];
    while (fgets(line, sizeof line, f) != NULL) {

        // grow instructions array in steps of INSTRUCTIONS_GROW elements
        if (n_instructions % INSTRUCTIONS_GROW == 0) {
            instructions = instructions_realloc(instructions, n_instructions + INSTRUCTIONS_GROW);
        }

        char *endptr;
        instructions[n_instructions] = strtol(line, &endptr, 16);
        if (*endptr != '\n' && *endptr != '\r' && *endptr != '\0') {
            fprintf(stderr, "%s:line %d: invalid hexadecimal number: %s",
                    filename, n_instructions + 1, line);
            exit(1);
        }
        n_instructions++;
    }
    fclose(f);
    *n_instructions_p = n_instructions;
    // shrink instructions array to correct size
    instructions = instructions_realloc(instructions, n_instructions);
    return instructions;
}


// instructions_realloc is wrapper for realloc
// it calls realloc to grow/shrink the instructions array
// to the speicfied size
// it exits if realloc fails
// otherwise it returns the new instructions array
uint32_t *instructions_realloc(uint32_t *instructions, int n_instructions) {
    instructions = realloc(instructions, n_instructions * sizeof *instructions);
    if (instructions == NULL) {
        fprintf(stderr, "out of memory");
        exit(1);
    }
    return instructions;
}
