#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<time.h>
#include<assert.h>
 
#ifdef __APPLE__
#include <OpenCL/opencl.h>
#else
#include <CL/cl.h>
#endif
 
#define MAX_SOURCE_SIZE (0x100000)

/* IMPORTANT !!!  

1. THE LENGTH OF STRINGS AND THE NUMBER OF INPUTWORDS MUST BE AT LEAST =2 AND A POWER OF 2 ( e.g. 2,4,8,16,... )
2. maximal number of inputwords sn and max. length L belongs to sn*L = CL_DEVICE_MAX_WORK_GROUP_SIZE

*/


void quicksort(int *A, int len, int *freq) {
  if (len < 2) return;
 
  int pivot = freq[ A[len/2] ];
 
  int i, j;
  for (i = 0, j = len - 1; ; i++, j--) {
    while (freq[ A[i] ] > pivot) i++;
    while (freq[ A[j] ] < pivot) j--;
 
    if (i >= j) break;
 
    int temp = A[i];
    A[i]     = A[j];
    A[j]     = temp;
  }
 
  quicksort(A, i, freq);
  quicksort(A + i, len - i, freq);
}




 
int main(int argc, char **argv) { // TO DO: load the input data from command line into array "instances"

// Get platform and device information ------------------------------------------------------
 
    cl_platform_id platform_id = NULL;
    cl_device_id device_id = NULL;   
    cl_uint ret_num_devices;
    cl_uint ret_num_platforms;
    cl_int ret = clGetPlatformIDs(1, &platform_id, &ret_num_platforms);
    ret = clGetDeviceIDs( platform_id, CL_DEVICE_TYPE_GPU, 1, &device_id, &ret_num_devices);
    printf("ret = %d \n", ret);
    cl_uint computeUnits;
    cl_ulong size_local_mem;
    cl_ulong size_constant_buffer;
    cl_ulong size_max_mem_alloc;
    size_t size_max_work_groups; // max number of work items in a work group
    cl_uint max_work_item_dimension;
    size_t max_work_item_sizes[3];
    char infos[10240];	
    clGetPlatformInfo(platform_id,CL_PLATFORM_PROFILE,10240,infos,NULL);
    printf("PROFILE = %s\n",infos); 
    clGetPlatformInfo(platform_id,CL_PLATFORM_VERSION,10240,infos,NULL);
    printf("VERSION = %s\n",infos);
    clGetPlatformInfo(platform_id,CL_PLATFORM_NAME,10240,infos,NULL);
    printf("NAME = %s\n",infos);
    clGetPlatformInfo(platform_id,CL_PLATFORM_VENDOR,10240,infos,NULL);
    printf("VENDOR = %s\n",infos);
    clGetPlatformInfo(platform_id,CL_PLATFORM_EXTENSIONS,10240,infos,NULL);
    printf("EXTENSIONS = %s\n",infos);
    printf("Number of Devices: %u\n",ret_num_devices);
    clGetDeviceInfo(device_id,CL_DEVICE_NAME,sizeof(infos),infos,NULL);    
    printf("DEVICE NAME: %s\n",infos);
    clGetDeviceInfo(device_id,CL_DEVICE_VENDOR,sizeof(infos),infos,NULL);    
    printf("DEVICE VENDOR: %s\n",infos);
    clGetDeviceInfo(device_id,CL_DEVICE_VERSION,sizeof(infos),infos,NULL);    
    printf("DEVICE VERSION: %s\n",infos);
    clGetDeviceInfo(device_id,CL_DRIVER_VERSION,sizeof(infos),infos,NULL);    
    printf("DRIVER VERSION: %s\n",infos);
    clGetDeviceInfo(device_id,CL_DEVICE_MAX_COMPUTE_UNITS,sizeof(computeUnits),&computeUnits,NULL);    
    printf("DEVICE_MAX_COMPUTE_UNITS: %u\n",computeUnits);
    clGetDeviceInfo(device_id,CL_DEVICE_LOCAL_MEM_SIZE,sizeof(size_local_mem),&size_local_mem,NULL);    
    printf("CL_DEVICE_LOCAL_MEM_SIZE: %lu\n",size_local_mem);
    clGetDeviceInfo(device_id,CL_DEVICE_MAX_CONSTANT_BUFFER_SIZE,sizeof(size_constant_buffer),&size_constant_buffer,NULL);    
    printf("CL_DEVICE_MAX_CONSTANT_BUFFER_SIZE: %lu\n",size_constant_buffer);
    clGetDeviceInfo(device_id,CL_DEVICE_MAX_MEM_ALLOC_SIZE,sizeof(size_max_mem_alloc),&size_max_mem_alloc,NULL);    
    printf("CL_DEVICE_MAX_MEM_ALLOC_SIZE: %lu\n",size_max_mem_alloc);

    clGetDeviceInfo(device_id,CL_DEVICE_MAX_WORK_GROUP_SIZE,sizeof(size_max_work_groups),&size_max_work_groups,NULL);    
    printf("MAX_WORK_GROUP_SIZE: %lu\n",size_max_work_groups);
    clGetDeviceInfo(device_id,CL_DEVICE_MAX_WORK_ITEM_DIMENSIONS,sizeof(max_work_item_dimension),&max_work_item_dimension,NULL);    
    printf("CL_DEVICE_MAX_WORK_ITEM_DIMENSIONS: %u\n",max_work_item_dimension);

    clGetDeviceInfo(device_id,CL_DEVICE_MAX_WORK_ITEM_SIZES,sizeof(max_work_item_sizes),&max_work_item_sizes,NULL);    
    printf("CL_DEVICE_MAX_WORK_ITEM_SIZES[0]: %lu\n",max_work_item_sizes[0]);
    printf("CL_DEVICE_MAX_WORK_ITEM_SIZES[1]: %lu\n",max_work_item_sizes[1]);
    printf("CL_DEVICE_MAX_WORK_ITEM_SIZES[2]: %lu\n\n",max_work_item_sizes[2]);

	
// Random Numbers for Mutation
int randoms[computeUnits];
for(int c = 0; c < computeUnits; c++) { randoms[c] = rand()%1000000; }


// INPUT LOAD - INITIALISATION  -------------------------------------------------------------------------------------------------------

srand(time(NULL));

/* HALLO NILS, HIER MÜSSTE MAN DIE ZEICHEN AN IHREN POSITIONEN DURCH IHRE HÄUFIGKEITEN mit den Zeichen des Alphabetes A,B,.. ERSETZEN (A für die am häufigsten Vorkommen an Position 1,2,... danach B...)
 könntest du den Teil schreiben und die abgebildeten Strings in char instances[ (L*sn)+1 ] einlesen ? */

assert(argc > 2);
int L = strlen(argv[1]);
//printf("\nL = %d\n",L);
char instances[L*(argc-1)];

    for (int i = 1; i < argc; i++)
    {    
        assert( strlen(argv[i]) == L );
        printf("argv[%d]: %s\n", i, argv[i]);
        strcat(instances,argv[i]);
    }


printf("\ninstances: %s\n",instances);
printf("str len: %d\n",strlen(instances) );


int gen = 0;
int generations = 10; // variable for execution; choose you like
int lookahead = 2; // variable for execution; choose you like
//int L = 8;
int sn = argc-1;
const int GLOBALSIZE = L*sn*computeUnits;
//char instances[ (L*sn)+1 ];
int alphabet[8]; 
//= { 4, 3, 4, 3, 4, 4, 3, 3 };
int sizes[3] = {L,sn,computeUnits};
char mutation[computeUnits*L+1];
int results[computeUnits];
int charmatrix[L*sn];
//int randoms[16] = {524350000,736345342,862436745,435267372,851051618,767509761,409005456,714801623,850850845,296111739,918188915,766638501,140446746,856280074,319592828,699896283}; //only for amd gpu rx560
char candidate[L+1];
for(int m = 0; m < computeUnits*L; m++){mutation[m] = 'x';} 
mutation[computeUnits*L] = '\0';
for(int l = 0; l < L; l++) { candidate[l] ='A'; }
candidate[L] = '\0';
//strcpy(instances,"ACACBBBBDACAADAABABADAAACBDBCCCC");
// info: Number of compute Units equals checked candidates per generation






// NORMALISATION--------------------------------------------------------------------------------------------------

char positions[L*sn];
int frequencies[L*sn];

int *mapping[L];

for(int l = 0; l < L; l++){
 for(int n = 0; n < sn; n++){ 
frequencies[n+l*sn] = -1;
positions[n+l*sn] = instances [ n*L + l ]; }  

} 

for(int p = 0; p < L; p++){

int i, j, count; 
  for(i=0; i < sn; i++)
    {
        count = 1;
        for(j=i+1; j < sn; j++)
        {
            /* If duplicate element is found */
            if(positions[i+p*sn]==positions[j+p*sn])
            {
                count++;

                /* Make sure not to count frequency of same element again */
                frequencies[j+p*sn] = 0;
            }
        }

        /* If frequency of current element is not counted */
        if(frequencies[i+p*sn] != 0)
        {
            frequencies[i+p*sn] = count;
        }
    }

int map[128];
int mlen = 0;

for(i = 0; i < sn; i++)
    {
        if(frequencies[i+p*sn] != 0)
        {   
            map[mlen] = i+p*sn;
            mlen++;
            //printf("%d occurs %d times\n", arr[i], freq[i]);
        }
    }

alphabet[p] = mlen;
quicksort(map,mlen,frequencies);

mapping[p] = map;

for(int ch = 0; ch < sn; ch++ ){ 
	for(int w = 0; w < mlen; w++){  
		if(positions[ map[w] ]==instances[ch*L+p] ){ instances[ch*L+p] = w+65; }

} 
} 

                  } 

printf("\nNormalized instances:  %s\n",instances);




// Load the kernel source code into the array source_str -------------------------------------------------------------------------
FILE *fp;
char *source_str;
size_t source_size;
 
fp = fopen("kernel.cl", "r");
if (!fp) {
   fprintf(stderr, "Failed to load kernel.\n");
   exit(1);
         }
source_str = (char*)malloc(MAX_SOURCE_SIZE);
source_size = fread( source_str, 1, MAX_SOURCE_SIZE, fp);
fclose( fp );
 
// Create an OpenCL context
cl_context context = clCreateContext( NULL, 1, &device_id, NULL, NULL, &ret);
// Create a command queue
cl_command_queue command_queue = clCreateCommandQueue(context, device_id, 0, &ret);

// --*ORDER unknow--
// Create a program from the kernel source
cl_program program = clCreateProgramWithSource(context, 1, (const char **)&source_str, (const size_t *)&source_size, &ret);
// Build the program
ret = clBuildProgram(program, 1, &device_id, NULL, NULL, NULL);
cl_kernel kernel = clCreateKernel(program,"evo",&ret);
//--ORDER--



// DATA TRANSFER ------------------------------------------------------------------------------------------------------------------ 

// [Create] memory buffers
cl_mem gm_instances = clCreateBuffer(context, CL_MEM_READ_WRITE, ((L*sn)+1) * sizeof(char), NULL, &ret);
cl_mem gm_alphabet = clCreateBuffer(context, CL_MEM_READ_WRITE, L * sizeof(int), NULL, &ret);
cl_mem gm_sizes = clCreateBuffer(context, CL_MEM_READ_WRITE, 3 * sizeof(int), NULL, &ret);
cl_mem gm_candidate = clCreateBuffer(context, CL_MEM_READ_WRITE, L * sizeof(char), NULL, &ret);
cl_mem gm_mutation = clCreateBuffer(context, CL_MEM_READ_WRITE, ((L*computeUnits)+1) * sizeof(char), NULL, &ret); 
cl_mem gm_randoms = clCreateBuffer(context, CL_MEM_READ_WRITE, computeUnits * sizeof(int), NULL, &ret);
cl_mem gm_results = clCreateBuffer(context, CL_MEM_READ_WRITE, computeUnits * sizeof(int), NULL, &ret);
// write to Buffer
ret = clEnqueueWriteBuffer(command_queue, gm_instances, CL_TRUE, 0, ((L*sn)+1) * sizeof(char), instances, 0, NULL, NULL);
ret = clEnqueueWriteBuffer(command_queue, gm_alphabet, CL_TRUE, 0, L*sizeof(int), alphabet, 0, NULL, NULL);
ret = clEnqueueWriteBuffer(command_queue, gm_sizes, CL_TRUE, 0, 3*sizeof(int), sizes, 0, NULL, NULL);
ret = clEnqueueWriteBuffer(command_queue, gm_candidate, CL_TRUE, 0, L*sizeof(char), candidate, 0, NULL, NULL);
ret = clEnqueueWriteBuffer(command_queue, gm_mutation, CL_TRUE, 0, ((L*computeUnits)+1) * sizeof(char), mutation, 0, NULL, NULL);
ret = clEnqueueWriteBuffer(command_queue, gm_randoms, CL_TRUE, 0, computeUnits * sizeof(int), randoms, 0, NULL, NULL);
ret = clEnqueueWriteBuffer(command_queue, gm_results, CL_TRUE, 0, computeUnits * sizeof(int), results, 0, NULL, NULL);

// --*ORDER--
 
// Set the arguments of the kernel
ret = clSetKernelArg(kernel, 0, sizeof(cl_mem), (void *)&gm_instances);
ret = clSetKernelArg(kernel, 1, sizeof(cl_mem), (void *)&gm_alphabet);
ret = clSetKernelArg(kernel, 2, sizeof(cl_mem), (void *)&gm_sizes);
ret = clSetKernelArg(kernel, 3, sizeof(cl_mem), (void *)&gm_candidate);
ret = clSetKernelArg(kernel, 4, sizeof(cl_mem), (void *)&gm_mutation);
ret = clSetKernelArg(kernel, 5, sizeof(cl_mem), (void *)&gm_randoms);
ret = clSetKernelArg(kernel, 6, sizeof(cl_mem), (void *)&gm_results);
ret = clSetKernelArg(kernel, 7, L*sn*sizeof(int), NULL);



   


// INITIAL GENERATION OF RESULTS -----------------------------------------------------------------------------------------------------

// Execute the OpenCL kernel on the data
size_t global_item_size = GLOBALSIZE; // Process the entire arrays
size_t local_item_size = L*sn; // Divide work items into groups of L*sn size
ret = clEnqueueNDRangeKernel(command_queue, kernel, 1, NULL, &global_item_size, &local_item_size, 0, NULL, NULL);

// get the relevant calculated data for further sequencial processing
ret = clEnqueueReadBuffer(command_queue, gm_mutation, CL_TRUE, 0, ((L*computeUnits)+1) * sizeof(char), mutation, 0, NULL, NULL);
ret = clEnqueueReadBuffer(command_queue, gm_results, CL_TRUE, 0, computeUnits * sizeof(int), results, 0, NULL, NULL);


char winner[L+1];
int pre = 999999;


// get element with minimum distance [MIN Reduction] 
int winIndex = 0;
int smaller = 999999;
for(int i = 0; i < computeUnits; i++){

if(results[i] < smaller){
smaller = results[i];
winIndex = i;          }

}


// best candidate for next generation of soulution candidates
for(int s = winIndex*L; s < L*(winIndex+1); s++) { winner[s-winIndex*L] = mutation[s]; }
winner[L] = '\0';


// TERMINATION CONDITIONS
lookahead -= (int)(!strcmp(winner,candidate));

if(lookahead == 0){ 

printf("\n\nresults:\n"); 
printf("\n\nresults:\n\n"); 

//map the used frequency characters of result string back to their original
for (int l = 0; l < L; l++){
winner[l] = positions [ mapping[l][ winner[l]-65 ] ];	}

printf("\nbest found soulution: %s\n",winner);
printf("\nk = %d\n\n",smaller);

return 0; }

else {  
strcpy(candidate,winner);
pre = smaller;
gen++;   }
  







// LOOP next soulution populations ---------------------------------------------------------------------------------------------------------


for(int g = gen; g < generations; g++){

	
        for(int c = 0; c < computeUnits; c++) { randoms[c] = rand()%1000000; }

        // PARALLEL SECTION
	ret = clEnqueueWriteBuffer(command_queue, gm_candidate, CL_TRUE, 0, L*sizeof(char), candidate, 0, NULL, NULL); 
	ret = clEnqueueWriteBuffer(command_queue, gm_randoms, CL_TRUE, 0, computeUnits * sizeof(int), randoms, 0, NULL, NULL);
	
    ret = clSetKernelArg(kernel, 3, sizeof(cl_mem), (void *)&gm_candidate);  
	ret = clSetKernelArg(kernel, 5, sizeof(cl_mem), (void *)&gm_randoms); 
	
        

    ret = clEnqueueNDRangeKernel(command_queue, kernel, 1, NULL, &global_item_size, &local_item_size, 0, NULL, NULL); 
	

        // get the relevant calculated data for further sequencial processing
    ret = clEnqueueReadBuffer(command_queue, gm_mutation, CL_TRUE, 0, ((L*computeUnits)+1) * sizeof(char), mutation, 0, NULL, NULL);
	ret = clEnqueueReadBuffer(command_queue, gm_results, CL_TRUE, 0, computeUnits * sizeof(int), results, 0, NULL, NULL);




	

       
        // get element with minimum distance [MIN Reduction]
	winIndex = 0;
	smaller = 999999;

	for(int i = 0; i < computeUnits; i++){

	if(results[i] < smaller){
	smaller = results[i];
	winIndex = i;          }

	}



        // best candidate for next generation of soulution candidate
        for(int s = winIndex*L; s < L*(winIndex+1); s++) { winner[s-winIndex*L] = mutation[s]; } 
        winner[L] = '\0';





    // TERMINATION CONDITIONS
	lookahead -= (int)(!strcmp(winner,candidate));

	if(lookahead == 0){ 

	printf("\n\nresults:\n\n");
    
	//map the used frequency characters of result string back to their original
    for (int l = 0; l < L; l++){
    winner[l] = positions [ mapping[l][ winner[l]-65 ] ];	}	
	
	printf("\nbest found soulution: %s\n",winner);
	printf("\nk = %d\n\n",smaller);

	return 0; }

	else{  
	strcpy(candidate,winner);
	pre = smaller;
	}

 

}


//----------------------------------------------------------------------------------------------------------------------


//RESULTS

//map the used frequency characters of result string back to their original
for (int l = 0; l < L; l++){
 winner[l] = positions [ mapping[l][ winner[l]-65 ] ];	}


printf("\n\nresults:\n\n"); 

printf("\nbest found soulution: %s\n",winner);
printf("\nk = %d\n\n",smaller);



}
