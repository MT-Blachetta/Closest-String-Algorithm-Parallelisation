
L : WE USE EQUAL LENGTH STRINGS FOR INPUT AND SOULUTION ! 
sn : Number of strings in input list
computeUnits : THE NUMBER OF COMPUTE UNITS IS EQUAL TO THE POPOLATION SIZE OF SOULUTION CANDIDATES CALCULATED IN EACH WORK GROUP ONE

char instances : THE INPUT STRING LIST MAPPED TO A SINGLE ARRAY, string starts at L*( list index of input string) within the array
int generations : Number of candidate generations to be checked for best fitness

char instances :  an array with the input strings of the problem instance  
int alphabet : an array with the number of used normalized (mapped by frequency) characters in a position
int sizes : sizes[0] = L, lenght of the used strings; sizes[1] = sn, number of inputstrings; sizes[3] = Number of compute Units equals checked candidates per generation
char candidate : the initial or actual soulution candidate to check for is maximum distance to cover all strings
char mutation : collection of checked strings from the new mutated generation of soulution candidates
int randoms : random numbers to generate mutation from the last calculated best candidate
int results : hamming distances of the new (mutated) generation strings
int charmatrix : the string comparesion results of each character are represented in a 2D-field(array) matrix
int col : colums of the charmatrix represents a certain character position (all strings here has equal lenght)
int row : rows corresponds to the index of one inputstrings from the list (of all)
int lookahead : after the best found soulution of an iteration is the same as before and distance (fitness) is not improved
                this variable is decremented. If it becomes 0, the search terminates.

int change : determines wether the character at the position is changed by mutation
int bl: intern for mutation calculation
int individual : the new character after mutation process


variables used due to reduction (sum , maximum):
int offset, int mask

as explained in: https://developer.amd.com/resources/articles-whitepapers/opencl-optimization-case-study-simple-reductions/ 

