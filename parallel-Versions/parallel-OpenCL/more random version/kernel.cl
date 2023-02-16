__kernel void evo ( __global const char *instances, __global const int *alphabet, __global const int *sizes, __global char *candidate, __global char *mutation, __global int *randoms, __global int *results , __local int *charmatrix) {


// INIT 
size_t lid = get_local_id(0);
size_t wgID = get_group_id(0);
//int wgID  =  (int)( (float)globalID/(L*sn)  );
//int lid = globalID % (L*sn);
//int wgID = 0;
int col =  lid % sizes[0];
int row =  (int)( (float)lid/sizes[0] );
//------------------------------------------------------


// MUTATION 
int randomC = randoms[wgID]%(34256+42*col); 
int change = (int)( (float)(randomC%1000)/400);
char individual = ( (candidate[col]-65) + change*randomC*wgID) % alphabet[col] + 65;
//int compare = (int)(individual == candidate[col]);
//individual = (individual-65+compare*change*wgID)% alphabet[col] + 65;
mutation[wgID*sizes[0] + col] = individual;
//-----------------------------------------------------


// HAMMING_DISTANCE 

// :Comparsion
charmatrix[lid] = (int)(individual != instances[sizes[0]*row + col]);

barrier(CLK_LOCAL_MEM_FENCE); //-----------------------


// [REDUCTION] sum(hamming_distance)
 for(int offset = 1;
      offset < sizes[0];
      offset <<= 1) {
    int mask = (offset << 1) - 1;
    if ((col & mask) == 0) { charmatrix[lid] += charmatrix[ lid + offset ]; }
    barrier(CLK_LOCAL_MEM_FENCE);

}

barrier(CLK_LOCAL_MEM_FENCE); //-----------------------

// [REDUCTION] maximal(sum) of k


for(int offset = 1 ;offset < sizes[1]; offset <<= 1)  {
    
    int mask = (offset << 1) - 1;
    if ( ((row & mask) == 0) && (col == 0) ) { 
    
   
    uint prime = charmatrix[ row*sizes[0] ]; 
    uint second = charmatrix[row*sizes[0] + offset*sizes[0] ]; 
    charmatrix[ row*sizes[0] ] = (prime > second) ? prime : second;
    //index[(int)((float)row/ix)] = (prime > second) ? index[(int)(row/ix)] : index[ (int)((row+offset)/ix) ] ;

}
    
barrier(CLK_LOCAL_MEM_FENCE);
    

}

//-------------------------------------------------

if (lid == 0){  

results[wgID] = charmatrix[lid]; //
  
}

// charmatrix[col + row*sizes[0] ]



 



}
