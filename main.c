/* 
 * File:   main.c
 * Author: sam
 *
 * Created on 17 September 2017, 14:34
 */

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <unistd.h>

/*
 * 
 */


typedef struct {
    int pos, type;
} mut;


typedef struct {
    int gen;
    char *seq;
    mut* mutlist[];
} indiv;

typedef struct {
    size_t mutcount;
    mut* mutlist;
} locus; 

typedef struct {
    mut *mut_char;
    long count, *mutcount;
} lineage;

typedef struct {
    lineage *mut_list;
} generation;




void replicate_basic(int *startpop, int *endpop) {
    
    *endpop = *startpop * 2;    
    
}




int survival(int count) {
    return (count * 2);
}

void mutate(lineage *line, int lineid, int linecount, int newlines) {
    
    

}




void propagate(lineage startline, int gencount, double mut_rate, double prop_rate, int *popsize, int *mutcount ) {
    
    lineage *line;
    line = malloc(sizeof(lineage));
    
    int _startcount = 1;
    // set the 1st member of the population to 1
    startline.mut_char = (mut *)malloc(sizeof(mut));
    startline.count = _startcount;
    *startline.mutcount = 0;
    startline.mut_char->pos = 3;
    startline.mut_char->type = 1;
    
    size_t linecount = 1;
    *popsize = startline.count;
    *mutcount = *startline.mutcount;
    
    
   
    // load the initial lineage into the lineage array
    memcpy(line, &startline, sizeof(lineage));
    

    // initialize various counters
    size_t memcount, mutcounter, checkcount;
    
    //run the main evolution loop
    for (int g=1;g <= gencount; ++g) {
        
        mutcounter = 0;
        memcount = 0;
        size_t newlines = 0;
        lineage *newline;
        

        printf("starting generation %d:\n",g);
         for (size_t l=0;l<linecount;++l) {
             
  
            long base_size = survival((line+l)->count);
            
            (line+l)->count = 0;
            
                for (long m=0;m<base_size;++m) {
                    if (drand48() < mut_rate) { // if the mutation is triggered...
                        ++newlines; // increment the lineage count
                        
                        //EXPAND THE LINEAGE LIST
                        line = realloc(line, (linecount+newlines)*sizeof(lineage));
//                        printf("allocated for line %d\n",linecount);
                        newline = (line+(linecount+newlines-1));
                        memcpy(newline, (line+l),sizeof(lineage));
                        
                        newline->count = 1;// reset member count to one       
                        
                        //increment the global mutation count                       
                        ++mutcounter;
                    } else {
                        ++(line+l)->count;
                    }
                    // increment the count of individuals
                    ++memcount;
                }
        
        }   
        
        //update the global count of lineages
        linecount += newlines;
        
    }
    
    *popsize = memcount;
    *mutcount = mutcounter;
    
        
}


void intitiate() {
    
}



int main(int argc, char** argv) {

    srand48(time(NULL));
    
    lineage seed;
    
    int pop_count;
    int mut_count;
    long gen_count;
    double mut_rate;
    double prop_rate;
    int c;
    char *argtail;
    
    pop_count=0;
    mut_count=0;
    
    while ((c = getopt(argc, argv, "g:r:p:")) != -1)
        switch (c) {
            case 'g':
                sscanf(optarg, "%ld", &gen_count);
                break;
            case 'r':
                sscanf(optarg, "%lf", &mut_rate);
                break;
            case 'p':
                sscanf(optarg, "%lf", &prop_rate);
                break;
            default:
                abort();
        }
    propagate(seed, gen_count, mut_rate, prop_rate, &pop_count, &mut_count);
    
    printf("Final Population has %d individuals and %d total mutations after %d generations\n", pop_count, mut_count, gen_count);
    
    return (EXIT_SUCCESS);
}

