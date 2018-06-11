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
#include <math.h>

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
    long count;
    long *mutcount;
} lineage;

typedef struct {
    lineage *mut_list;
} generation;


//FUNCTION TO DRAW FROM UNIFORM INTEGER DISTRIBUTION (STARTING AT ZERO)
long random_at_most(long max) {
  unsigned long
    // max <= RAND_MAX < ULONG_MAX, so this is okay.
    num_bins = (unsigned long) max + 1,
    num_rand = (unsigned long) RAND_MAX + 1,
    bin_size = num_rand / num_bins,
    defect   = num_rand % num_bins;

  long x;
  do {
   x = random();
  }
  // This is carefully written not to overflow
  while (num_rand - defect <= (unsigned long)x);

  // Truncated division is intentional
  return x/bin_size;
}


void replicate_basic(int *startpop, int *endpop) {
    
    *endpop = *startpop * 2;    
    
}




size_t survival(size_t count) {
    return (count * 2);
}

void mutate(lineage *line, int lineid, int linecount, int newlines) {
    
    

}

unsigned int bin_dist(double p, size_t n) {
    
    double log_q = log(1.0 - p);
    unsigned int x = 0;
    double sum = 0;
    
    for (;;) {
        sum += log(drand48())/ (n - x);
        if (sum < log_q)
            return x;
        ++x;
    }
}




void propagate(lineage startline, size_t *mutlist, int gencount, size_t genome_size, double mut_rate, double prop_rate, size_t *popsize, size_t *mutcount ) {
    
    lineage *line;
    line = malloc(sizeof(lineage));
    
    int _startcount = 1;
    // set the 1st member of the population to 1
    startline.mut_char = (mut *)malloc(sizeof(mut));
    startline.count = _startcount;
    startline.mutcount = calloc(1, sizeof(int));
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
             
  
            size_t base_size = survival((line+l)->count);
            
            (line+l)->count = 0;
            
                for (long m=0;m<base_size;++m) {
                    
                    //draw number of mutation events from binomial distribution
                    unsigned int event_count = bin_dist(mut_rate, genome_size);
                    
                    if (event_count ==  0)
                         ++(line+l)->count;
                    else
                    
                    for (int p = 0; p< event_count; ++p) { // if the mutation is triggered...
                        ++newlines; // increment the lineage count
                        
                        //EXPAND THE LINEAGE LIST
                        line = realloc(line, (linecount+newlines)*sizeof(lineage));
//                        printf("allocated for line %d\n",linecount);
                        newline = (line+(linecount+newlines-1));
                        memcpy(newline, (line+l),sizeof(lineage));
                        
                        newline->count = 1;// reset member count to one  
                        
                        //PERFORM THE MUTATION
                        long size_limit = genome_size;
                        long mutator_locus = random_at_most(size_limit);
                        ++*(mutlist + mutator_locus);
                        
                        //INCREMENT THE GLOBAL MUTATION COUNT                       
                        ++mutcounter;
                    }; 
                    
                }
            // increment the count of individuals
            ++memcount;
            printf("memcount is now set to %d\n", memcount);
        
        }   
        
        //update the global count of lineages
        linecount += newlines;
        
        size_t poptemp = *popsize;
    *popsize += memcount;
    *mutcount += mutcounter;
        
    }
    
   
    
        
}


void intitiate() {
    
}



int main(int argc, char** argv) {

    srand48(time(NULL));
    
    lineage seed;
    
    size_t pop_count;
    size_t mut_count;
    long gen_count = 3;
    double mut_rate;
    double prop_rate;
    int c;
    char *argtail;
    char *infile = "/dev/stdin";
    char *outfile = "/dev/stdout";
    size_t genomesize = 0;
    
    pop_count=0;
    mut_count=0;
    
    while ((c = getopt(argc, argv, "g:r:f:p:s:o:")) != -1)
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
            case 'f':
                infile = optarg;
                break;
            case 'o':
                outfile = optarg;
                break;
            case 's':
                sscanf(optarg, "%lf", &genomesize);
                break;
            default:
                abort();
        }
    
    printf("infile to be read from %s\n", infile);
    
    int *genome;
    
    if (genomesize > 0) { //if genome size is manually specified create a binary genome
        
        genome = malloc(sizeof(int)*genomesize);
        
        //populate the binary genome randomly
        for (int i=0; i<genomesize; ++i)
            *(genome + i) = rand() % 2;
        
    }
    
    //zero-initalize [**SIMPLE**] mutation list
    size_t* mutlocus = calloc(genomesize, sizeof(size_t));
    
    propagate(seed, mutlocus, gen_count, genomesize, mut_rate, prop_rate, &pop_count, &mut_count);
    
    //write the final output to a CSV
    FILE *output = fopen(outfile, "w");
    fprintf(output,"POS,FREQ\n");
    for (int i=0;i<genomesize;++i) {
        double mutfreq = *(genome + i)/pop_count;
        int pos = i + 1;
        if (*(genome + i) > 0) 
            fprintf(output,"%d,%.4f\n", pos, mutfreq);
    }
    
    
    
    printf("Final Population has %d individuals and %d total mutations after %d generations\n", 
            pop_count, 
            mut_count, 
            gen_count
    );
    
    return (EXIT_SUCCESS);
}

