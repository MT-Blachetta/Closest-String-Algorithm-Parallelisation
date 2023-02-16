#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<time.h>
 
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

 
int main(void) { // TO DO: load the input data from command line into array "instances"

// Get platform and device information
 
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


srand(time(NULL));

// HALLO NILS, HIER MÜSSTE MAN DIE ZEICHEN AN IHREN POSITIONEN DURCH IHRE HÄUFIGKEITEN mit den Zeichen des Alphabetes A,B,.. ERSETZEN (A für die am häufigsten Vorkommen an Position 1,2,... danach B...)
// könntest du den Teil schreiben und die abgebildeten Strings in char instances[ (L*sn)+1 ] einlesen ?

int gen = 0;
int generations = 10;
int lookahead = 3;

int L = 8;
int sn = 4;


const int GLOBALSIZE = L*sn*computeUnits;

char instances[ (L*sn)+1 ];
int  alphabet[8] = { 4, 3, 4, 3, 4, 4, 3, 3 };

int sizes[3] = {L,sn,computeUnits};

char mutation[computeUnits*L+1];
int results[computeUnits];
int charmatrix[L*sn];
int randoms[16] = {524350000,736345342,862436745,435267372,851051618,767509761,409005456,714801623,850850845,296111739,918188915,766638501,140446746,856280074,319592828,699896283}; //only for amd gpu rx560


char candidate[L+1];
for(int m = 0; m < computeUnits*L; m++){mutation[m] = 'x';} 
mutation[computeUnits*L] = '\0';

strcpy(candidate,"AAAAAAAA"); 
strcpy(instances,"ACACBBBBDACAADAABABADAAACBDBCCCC");



 // Load the kernel source code into the array source_str
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
 
    // [Create] memory buffers

cl_mem gm_instances = clCreateBuffer(context, CL_MEM_READ_WRITE, ((L*sn)+1) * sizeof(char), NULL, &ret);
cl_mem gm_alphabet = clCreateBuffer(context, CL_MEM_READ_WRITE, L * sizeof(int), NULL, &ret);
cl_mem gm_sizes = clCreateBuffer(context, CL_MEM_READ_WRITE, 3 * sizeof(int), NULL, &ret);
cl_mem gm_candidate = clCreateBuffer(context, CL_MEM_READ_WRITE, L * sizeof(char), NULL, &ret);
cl_mem gm_mutation = clCreateBuffer(context, CL_MEM_READ_WRITE, ((L*computeUnits)+1) * sizeof(char), NULL, &ret); 
cl_mem gm_randoms = clCreateBuffer(context, CL_MEM_READ_WRITE, computeUnits * sizeof(int), NULL, &ret);
cl_mem gm_results = clCreateBuffer(context, CL_MEM_READ_WRITE, computeUnits * sizeof(int), NULL, &ret);
 
    // writeBuffer
ret = clEnqueueWriteBuffer(command_queue, gm_instances, CL_TRUE, 0, ((L*sn)+1) * sizeof(char), instances, 0, NULL, NULL);
ret = clEnqueueWriteBuffer(command_queue, gm_alphabet, CL_TRUE, 0, L*sizeof(int), alphabet, 0, NULL, NULL);
ret = clEnqueueWriteBuffer(command_queue, gm_sizes, CL_TRUE, 0, 3*sizeof(int), sizes, 0, NULL, NULL);
ret = clEnqueueWriteBuffer(command_queue, gm_candidate, CL_TRUE, 0, L*sizeof(char), candidate, 0, NULL, NULL);
ret = clEnqueueWriteBuffer(command_queue, gm_mutation, CL_TRUE, 0, ((L*computeUnits)+1) * sizeof(char), mutation, 0, NULL, NULL);
ret = clEnqueueWriteBuffer(command_queue, gm_randoms, CL_TRUE, 0, computeUnits * sizeof(int), randoms, 0, NULL, NULL);
ret = clEnqueueWriteBuffer(command_queue, gm_results, CL_TRUE, 0, computeUnits * sizeof(int), results, 0, NULL, NULL);


    // Create a program from the kernel source
    cl_program program = clCreateProgramWithSource(context, 1, (const char **)&source_str, (const size_t *)&source_size, &ret);
 
    // Build the program
    ret = clBuildProgram(program, 1, &device_id, NULL, NULL, NULL);
    printf("ret = %d \n", ret);
    // Create the OpenCL kernel
    cl_kernel kernel = clCreateKernel(program,"evo",&ret);
    printf("ret = %d \n", ret);
 
    // Set the arguments of the kernel
ret = clSetKernelArg(kernel, 0, sizeof(cl_mem), (void *)&gm_instances);
printf("ret = %d \n", ret); 
ret = clSetKernelArg(kernel, 1, sizeof(cl_mem), (void *)&gm_alphabet);
printf("ret = %d \n", ret); 
ret = clSetKernelArg(kernel, 2, sizeof(cl_mem), (void *)&gm_sizes);
printf("ret = %d \n", ret); 
ret = clSetKernelArg(kernel, 3, sizeof(cl_mem), (void *)&gm_candidate);
printf("ret = %d \n", ret); 
ret = clSetKernelArg(kernel, 4, sizeof(cl_mem), (void *)&gm_mutation);
printf("ret = %d \n", ret); 
ret = clSetKernelArg(kernel, 5, sizeof(cl_mem), (void *)&gm_randoms);
printf("ret = %d \n", ret);  
ret = clSetKernelArg(kernel, 6, sizeof(cl_mem), (void *)&gm_results);
printf("ret = %d \n", ret);
ret = clSetKernelArg(kernel, 7, L*sn*sizeof(int), NULL);
printf("ret = %d \n", ret);


    
    // Execute the OpenCL kernel on the list
    size_t global_item_size = GLOBALSIZE; // Process the entire lists
    size_t local_item_size = L*sn; // Divide work items into groups of L*sn
    ret = clEnqueueNDRangeKernel(command_queue, kernel, 1, NULL, &global_item_size, &local_item_size, 0, NULL, NULL);
    printf("ret = %d \n", ret);    



ret = clEnqueueReadBuffer(command_queue, gm_mutation, CL_TRUE, 0, ((L*computeUnits)+1) * sizeof(char), mutation, 0, NULL, NULL);
printf("ret = %d \n", ret);
//ret = clEnqueueReadBuffer(command_queue, gm_randoms, CL_TRUE, 0, computeUnits * sizeof(int), randoms, 0, NULL, NULL);
ret = clEnqueueReadBuffer(command_queue, gm_results, CL_TRUE, 0, computeUnits * sizeof(int), results, 0, NULL, NULL);
printf("ret = %d \n", ret);

printf("\n\nFIRST generation: %d\n",gen);

int pre = 999999;

int winIndex = 0;
int smaller = 999999;
char winner[L+1];

for(int i = 0; i < computeUnits; i++){
printf("\nMIN TEST %d - winIndex: %d\n",i,winIndex);
printf("\nsmaller (before): %d\n",smaller);
if(results[i] < smaller){
smaller = results[i];
winIndex = i;          }
printf("\nsmaller (after): %d\n",smaller);

}

printf("\nFINISHED winIndex: %d\n",winIndex);

printf("\nmutation: %s\n",mutation);
printf("\nresults:\n"); 
for (int i = 0; i < computeUnits; i++) { printf("%d ",results[i]);  } 

for(int s = winIndex*L; s < L*(winIndex+1); s++) { winner[s-winIndex*L] = mutation[s]; }
winner[L] = '\0';

printf("\nwinner: %s\n",winner);
printf("\npre: %d\n",pre);
printf("\nsmaller: %d\n",smaller);
printf("\npre<=smaller: %d\n",(pre<=smaller));
printf("\n!strcmp(winner,candidate): %d\n",!strcmp(winner,candidate));
printf("\n(int) (!strcmp(winner,candidate) || pre <= smaller): %d\n",(int) (!strcmp(winner,candidate) || pre <= smaller) );
printf("\nlookahead (before): %d\n",lookahead);
lookahead -= (int) (!strcmp(winner,candidate) || pre <= smaller);
printf("\nlookahead (after): %d\n",lookahead);

if(lookahead == 0){ 
printf("\nENTERED lookahead = 0\n");
printf("\n\nresults:\n"); 

for (int i = 0; i < computeUnits; i++) { printf("%d ",results[i]);  } 

printf("\n\nmutation:\n");
for (int j = 0; j < computeUnits; j++){
  printf("\n");
  printf("%d ",j);
  for (int c = 0; c < L; c++){ printf("%c",mutation[j*L + c]);   } 
}

printf("\n\n");



return 0; }

else {  
printf("\nENTERED first else\n");
printf("candidate (before): %s\n",candidate);
strcpy(candidate,winner);
printf("\ncandidate (after): %s\n",candidate);
        pre = smaller;
        gen++; 
     }
  
printf("\ngen: %d\n",gen);
printf("\ngenerations: %d\n",generations);
// LOOP


	for(int g = gen; g < generations; g++){

printf("\ngeneration: %d\n",g);
for(int c = 0; c < computeUnits; c++) { randoms[c] = rand()%1000000;
printf("randoms[%d]= %d\n",c,randoms[c]);  }

ret = clEnqueueWriteBuffer(command_queue, gm_candidate, CL_TRUE, 0, L*sizeof(char), candidate, 0, NULL, NULL);
printf("ret = %d \n", ret); 
ret = clEnqueueWriteBuffer(command_queue, gm_randoms, CL_TRUE, 0, computeUnits * sizeof(int), randoms, 0, NULL, NULL);
printf("ret = %d \n", ret); 
ret = clSetKernelArg(kernel, 3, sizeof(cl_mem), (void *)&gm_candidate);
printf("ret = %d \n", ret);  
ret = clSetKernelArg(kernel, 5, sizeof(cl_mem), (void *)&gm_randoms); 
printf("ret = %d \n", ret);
ret = clEnqueueNDRangeKernel(command_queue, kernel, 1, NULL, &global_item_size, &local_item_size, 0, NULL, NULL);
printf("ret = %d \n", ret);  
ret = clEnqueueReadBuffer(command_queue, gm_mutation, CL_TRUE, 0, ((L*computeUnits)+1) * sizeof(char), mutation, 0, NULL, NULL);
printf("ret = %d \n", ret);
ret = clEnqueueReadBuffer(command_queue, gm_results, CL_TRUE, 0, computeUnits * sizeof(int), results, 0, NULL, NULL);
printf("ret = %d \n", ret);  




winIndex = 0;
smaller = 999999;

for(int i = 0; i < computeUnits; i++){

if(results[i] < smaller){
smaller = results[i];
winIndex = i;          }


}

printf("\nmutation: %s\n",mutation);
printf("\nresults:\n"); 
for (int i = 0; i < computeUnits; i++) { printf("%d ",results[i]);  } 

for(int s = winIndex*L; s < L*(winIndex+1); s++) { winner[s-winIndex*L] = mutation[s]; }
winner[L] = '\0';

printf("\nwinner: %s\n",winner);
printf("\npre: %d\n",pre);
printf("\nsmaller: %d\n",smaller);
printf("\npre<=smaller: %d\n",(pre<=smaller));
printf("\n!strcmp(winner,candidate): %d\n",!strcmp(winner,candidate));
printf("\n(int) (!strcmp(winner,candidate) || pre <= smaller): %d\n",(int) (!strcmp(winner,candidate) || pre <= smaller) );
printf("\nlookahead (before): %d\n",lookahead);
lookahead -= (int) (!strcmp(winner,candidate) || pre <= smaller);
printf("\nlookahead (after): %d\n",lookahead);



if(lookahead == 0){ 

// || gen >= generations

printf("\n\nresults:\n"); 

for (int i = 0; i < computeUnits; i++) { printf("%d ",results[i]);  } 

printf("\n\nmutation:\n");
for (int j = 0; j < computeUnits; j++){
  printf("\n");
  printf("%d ",j);
  for (int c = 0; c < L; c++){ printf("%c",mutation[j*L + c]);   } 
}
printf("\nwinner: %s\n",winner);
printf("\npre: %d\n",pre);
printf("\nsmaller: %d\n",smaller);
printf("\n\nTERMINATION, no improvement\n\n");

return 0; }

else{  
printf("candidate (before): %s\n",candidate);
strcpy(candidate,winner);
printf("\ncandidate (after): %s\n",candidate);
pre = smaller;
//gen++; 
}

 



}

/*

printf("\n\n\ncharmatrix");

for(int j = 0; j < computeUnits; j++) { 
printf("\n\n"); 
  for(int inp = 0; inp < sn; inp++) { 
  printf("\n"); 
  for (int l = 0; l < L; l++){ printf("%d ", charmatrix[ j*sn*L + inp*L + l] ); } 
  
  } 

}


*/

printf("\n\nresults:\n"); 

for (int i = 0; i < computeUnits; i++) { printf("%d ",results[i]);  } 

printf("\n\nmutation:\n");
for (int j = 0; j < computeUnits; j++){
  printf("\n");
  printf("%d ",j);
  for (int c = 0; c < L; c++){ printf("%c",mutation[j*L + c]);   } 
}
 


printf("\nwinner: %s\n",winner);
printf("\npre: %d\n",pre);
printf("\nsmaller: %d\n",smaller);

}
