#include <stdio.h>
#include <stdlib.h>
#include <sys/syscall.h>

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
    
	/*
    char buf[1024];
    uint64_t arg1 = 0;
    uint64_t arg2 = (uint64_t)&buf;
    uint64_t arg3 = 1024;
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
    
    while(1) {
	  write(arg1,(void *)&ch, 7);
	  read(arg1, (void *)arg2, arg3);
	  write(arg1, (void *)arg2, 20);
	  //getpid();
    }


	mmap(0, 0, 0, 0, 0, 0);

    munmap(0, 0);	

	*/
     
    dummy(); 
    d2();
    while(1)  { }
    return 0;
}

