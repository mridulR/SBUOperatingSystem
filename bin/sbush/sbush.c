#include <stdio.h>
#include <stdlib.h>
#include "syscall.h"

void dummy() {                                                                                                                      
    int a = 40;                                                                    
    int b = 80;                                                                    
    uint64_t ret = 0;                                                              
    __asm__ __volatile__                                                        
    (                                                                              
        "movq %1,%%rax\n"                                                          
        "movq %2,%%rbx\n"                                                          
        "int $0x80\n"                                                              
        "movq %%rax,%0\n"                                                          
        :"=r"(ret)                                                                 
        :"r"((uint64_t)a), "r"((uint64_t)b)                                        
        : "cc"                                                                     
    );                                                                             
    return;                                                                        
}                                                                                  
                                                                                   
void d2() {                                                                        
    int a = 40;                                                                    
    int b = 80;                                                                    
    __asm__ __volatile__                                                           
    (                                                                              
        "movq %0,%%rax\n"                                                          
        "movq %1,%%rbx\n"                                                          
        "int $0x80\n"                                                              
        :                                                                          
        :"g"((uint64_t)a), "g"((uint64_t)b)                                        
        : "cc"                                                                     
    );                                                                             
                                                                                   
}

void d3() {                                                                     
    int a = 40;                                                                 
    int b = 80;                                                                 
    //uint64_t ret = 0;                                                         
    __asm__ __volatile__                                                        
    (                                                                           
        "movq %0,%%rax\n"                                                       
        "movq %1,%%rbx\n"                                                       
        "int $0x80\n"                                                           
        :                                                                       
        :"g"((uint64_t)a), "g"((uint64_t)b)                                     
        : "cc"                                                                  
    );                                                                          
    return;                                                                     
}                                                                               
                                                                                
void d4() {                                                                     
    int a = 40;                                                                 
    int b = 80;                                                                 
    __asm__ __volatile__                                                        
    (                                                                           
        "movq %0,%%rax\n"                                                       
        "movq %1,%%rbx\n"                                                       
        "int $0x80\n"                                                           
        :                                                                       
        :"g"((uint64_t)a), "g"((uint64_t)b)                                     
        : "cc"                                                                  
    );                                                                          
                                                                                
} 


int main(int argc, char *argv[], char *envp[]) {
    
	
    char buf2[1024];
    char ch[10];
    ch[0] = 's';
    ch[1] = 'b';
    ch[2] = 'u';
    ch[3] = 's';
    ch[4] = 'h';
    ch[5] = '~';
    ch[6] = '>';
    ch[7] = ' ';
    ch[8] = '\0';

    for(int i = 0; i< 1024; ++i) {
       // buf[i] = '\0';
        buf2[i] = '\0';
    }

    while(1) {
      write(1, &ch, 8);
      uint64_t ret = read(0, buf2, 1024);
      write(1, buf2, ret);
    }


    mmap((uint64_t *)0xFFF, 0x10, 0x20, 0x30, 0x40, 0x50);

    munmap((uint64_t *)0xFF, 0x60);    
     
    //dummy(); 
    //d2();
    
    while(1)  { }
    return 0;
}

