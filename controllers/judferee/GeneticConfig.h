#ifndef GENE_CONFIG   /* Include guard */
#define GENE_CONFIG

#ifdef __linux__ 
  #include "../target_parameter.h"
#elif _WIN32
  #include "..\target_parameter.h"
#else

#endif

 // hyper-parameter

int GA_popu_size = 50;
double GA_Xover_rate = 0.8;
double GA_mutate_rate = 0.2;

int GA_max_gen = 100;
double GA_tourbin_size_rate = 0.2;




#endif 