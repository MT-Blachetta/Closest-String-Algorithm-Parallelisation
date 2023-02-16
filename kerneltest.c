#include<stdio.h>
#include<stdlib.h>
#include<string.h>

// This is a sequencial simulation of the kernel.cl
//proof correctness  

void evo ( const char *instances, const int *alphabet, const int *sizes, char *candidate, char *mutation, int *randoms,  int *results , int *charmatrix, FILE *info ); 

int main (void)
{ 

FILE *info;

info = fopen("kernel_info.txt","w");

int L = 4;
int sn = 4;

int computeUnits = 4;

char instances[(L*sn)+1];
//int alphabet[8] = {4, 3, 4, 3, 4, 4, 3, 3};
int alphabet[4] = {4, 3, 4, 3};
int sizes[3] = {L,sn,computeUnits};

char mutation[computeUnits*L+1];
int results[computeUnits];
int charmatrix[computeUnits*L*sn];
int randoms[16] = {524350000,736345342,862436745,435267372,851051618,767509761,409005456,714801623,850850845,296111739,918188915,766638501,140446746,856280074,319592828,699896283};

char candidate[L+1]; 
strcpy(mutation,"xxxxxxxxxxxxxxxx");
strcpy(candidate,"AAAA"); 
strcpy(instances,"DABACCDBBAAAABCC");

evo(instances,alphabet,sizes,candidate,mutation,randoms,results,charmatrix,info);

fprintf(info,"\n\n\ncharmatrix");

for(int j = 0; j < computeUnits; j++) { 
fprintf(info,"\n\n"); 
  for(int inp = 0; inp < sn; inp++) { 
  fprintf(info,"\n"); 
  for (int l = 0; l < L; l++){ fprintf(info,"%d ", charmatrix[ j*sn*L + inp*L + l] ); } 
  
  } 

}

fprintf(info,"\n\nresults:\n"); 

for (int i = 0; i < computeUnits; i++) { fprintf(info,"%d ",results[i]);  } 

fprintf(info,"\n\nmutation:\n");
for (int j = 0; j < computeUnits; j++){
  fprintf(info,"\n");
  fprintf(info,"%d ",j);
  for (int c = 0; c < L; c++){ fprintf(info,"%c",mutation[j*L + c]);   } 
}

fprintf(info,"\n\n");


/*
[{'B': 'i', 'A': 'z', 'D': 'h', 'C': 'd'}, {'B': 'f', 'A': 'r', 'C': 'd'}, {'B': 'g', 'A': 'f', 'D': 'h', 'C': 's'}, {'B': 'b', 'A': 'f', 'C': 'g'}, {'B': 'w', 'A': 'b', 'D': 'e', 'C': 'h'}, {'B': 'r', 'A': 'z', 'D': 'd', 'C': 's'}, {'B': 'b', 'A': 'j', 'C': 'h'}, {'B': 'f', 'A': 'e', 'C': 's'}]
*/


}

void evo ( const char *instances, const int *alphabet, const int *sizes, char *candidate, char *mutation, int *randoms,  int *results , int *charmatrix, FILE *info) {

  
int L = sizes[0];
int sn = sizes[1];
int computeUnits = sizes[2];
   

int GLOBALSIZE = sizes[0]*sizes[1]*sizes[2];
fprintf(info,"\nGLOBALSIZE = %d\n",GLOBALSIZE);

fprintf(info,"\nHamming Distance COMPARESION\n");

for(int globalID = 0; globalID < GLOBALSIZE; globalID++) {

fprintf(info,"\n\n-----------------globalID  %d\n",globalID);

int wgID  =  (int)( (float)globalID/(sizes[0]*sizes[1])  );
fprintf(info,"wgID = %d\n",wgID);



int lid = globalID % (sizes[0]*sizes[1]);
fprintf(info,"lid = %d\n",lid);


int col =  lid % sizes[0];
fprintf(info,"col = %d\n",col);
int row =  (int)( (float)lid/sizes[0] );
fprintf(info,"row = %d\n",row);

fprintf(info,"\nMutation:\n"); 
int randomC = randoms[wgID]%( (wgID+1)*77777 ) % 34256;
fprintf(info,"randomC = %d\n",randomC); 
int bl = ( randomC + lid ) % sizes[0] ;
fprintf(info,"bl = %d\n",bl);
int change = ((int)!bl);
fprintf(info,"change = %d\n",change); 

fprintf(info,"\ncandidate char (before): %c\n",candidate[col]);
char individual = (candidate[col]-65 + change*randomC*wgID) % alphabet[col] + 65;
int compare = (int)(individual == candidate[col]);
individual = (individual-65+compare*change)% alphabet[col] + 65;
fprintf(info,"candidate char (after): %c\n",individual);

mutation[wgID*sizes[0] + col] = individual;
fprintf(info,"mutation: %s\n",mutation);

charmatrix[wgID*sizes[0]*sizes[1] + lid] = (int)(individual != instances[sizes[0]*row + col]);

}

fprintf(info,"\n\ncharmatrix");

for(int j = 0; j < computeUnits; j++) { 

  fprintf(info,"\n\n"); 
  for(int inp = 0; inp < sn; inp++) { 
  fprintf(info,"\n"); 
  for (int l = 0; l < L; l++){ fprintf(info,"%d ", charmatrix[ j*sn*L + inp*L + l] ); } 
  
  } 

                                      }


fprintf(info,"\n\nCLK_LOCAL_MEM_FENCE 1 (sum-reduction starts)\n"); //--------------------


for(int offset = 1; offset < sizes[0]; offset <<= 1) {
    
    fprintf(info,"\nOFFSET = %d\n",offset);

    for(int globalID = 0; globalID < GLOBALSIZE; globalID++) {

    fprintf(info,"\nglobalID %d\n",globalID);
    int wgID  =  (int)( (float)globalID/(sizes[0]*sizes[1])  );
    fprintf(info,"wgID = %d\n",wgID);
    int lid = globalID % (sizes[0]*sizes[1]);
    fprintf(info,"lid = %d\n",lid);
    int col =  lid % sizes[0];
    int row =  (int)( (float)lid/sizes[0] );


    int mask = (offset << 1) - 1;
    fprintf(info,"\ncol= %d\n",col);
    fprintf(info,"mask= %d\n",mask);
    if ((col & mask) == 0) { 
    fprintf(info,"OK ! (sum)\n");
    charmatrix[wgID*sizes[0]*sizes[1] +  lid] += charmatrix[wgID*sizes[0]*sizes[1] +  lid + offset]; }
    else { fprintf(info,"do nothing\n"); }
    
    

                                                            }
fprintf(info,"\n\npartial sum done - CLK_LOCAL_MEM_FENCE\n");
}



//------------------

fprintf(info,"\n\ncharmatrix");

for(int j = 0; j < computeUnits; j++) { 
fprintf(info,"\n\n"); 
  for(int inp = 0; inp < sn; inp++) { 
  fprintf(info,"\n"); 
  for (int l = 0; l < L; l++){ fprintf(info,"%d ", charmatrix[ j*sn*L + inp*L + l] ); } 
  
  } 

}

fprintf(info,"\n\nCLK_LOCAL_MEM_FENCE 2 (max-reduction starts)\n");


/*
int ix = 2;
int offset = 1
int mask = (offset << 1) - 1;
    if ( ((row & mask) == 0) && (col == 0) ) { 
    
   
     int prime = charmatrix[wgID*sizes[0]*sizes[1] +   row*sizes[0] ]; 
     int second = charmatrix[wgID*sizes[0]*sizes[1] +  row*sizes[0] + offset*sizes[0] ]; 
    charmatrix[wgID*sizes[0]*sizes[1] +   row*sizes[0] ] = (prime > second) ? prime : second;
    index[(int)((float)row/ix)] = (prime > second) ? row*sizes[0] : row*sizes[0] + offset*sizes[0]; // !!ACHTUNG , korrekte Indizes verwenden!!

fprintf(info,"\nCLK_LOCAL_MEM_FENCE\n");

}

ix *= 2;

*/




for(int offset = 1 ;offset < sizes[1]; offset <<= 1)  {

for(int globalID = 0; globalID < GLOBALSIZE; globalID++) {

fprintf(info,"\nOFFSET = %d\n",offset);

fprintf(info,"\n\nglobalID  %d\n",globalID);

int wgID  =  (int)( (float)globalID/(sizes[0]*sizes[1])  );
fprintf(info,"wgID = %d\n",wgID);



int lid = globalID % (sizes[0]*sizes[1]);
fprintf(info,"lid = %d\n",lid);


int col =  lid % sizes[0];
fprintf(info,"col = %d\n",col);
int row =  (int)( (float)lid/sizes[0] );
    
    
    int mask = (offset << 1) - 1;
    fprintf(info,"row= %d\n",row);
    fprintf(info,"mask= %d\n",mask);
    
    if ( ((row & mask) == 0) && (col == 0) ) { 
    
    fprintf(info,"OK ! (compare)\n");
    int prime = charmatrix[wgID*sizes[0]*sizes[1] +   row*sizes[0] ]; 
    int second = charmatrix[wgID*sizes[0]*sizes[1] +  row*sizes[0] + offset*sizes[0] ];
    fprintf(info,"\nprime= %d\n",prime);
    fprintf(info,"second= %d\n",second); 
    charmatrix[wgID*sizes[0]*sizes[1] +   row*sizes[0] ] = (prime > second) ? prime : second;
    fprintf(info,"max: %d\n",charmatrix[wgID*sizes[0]*sizes[1]+row*sizes[0]]);
    //index[(int)((float)row/ix)] = (prime > second) ? index[(int)(row/ix)] : index[ (int)((row+offset)/ix) ] ;

                                            }  else { fprintf(info,"do nothing\n"); }
 
   
                                                        }
   fprintf(info,"\n\nmax-comparesion done CLK_LOCAL_MEM_FENCE\n");
}




for(int globalID = 0; globalID < GLOBALSIZE; globalID++) {


int lid = globalID % (sizes[0]*sizes[1]);
int wgID  =  (int)((float)globalID/(sizes[0]*sizes[1]));


if (lid == 0){  
fprintf(info,"\n\nOK lid=0, result for work group\n");
results[wgID] = charmatrix[wgID*sizes[0]*sizes[1] +  lid]; }

// charmatrix[wgID*sizes[0]*sizes[1] +  col + row*sizes[0] ]

                                                         }


}




