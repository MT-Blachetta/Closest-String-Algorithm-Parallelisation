import functools
import random
import sys
import profile

def count_all(duplicates,ratio=False,rounded=0):
    freq = {}
    
    if(ratio):
    
        d_size = len(duplicates)

        for k in duplicates:
            num = freq.setdefault(k,[0,0])
            freq[k][0] = num[0]+1

        if(rounded):

            for k in freq.keys():
                freq[k][1] = str( round(100*freq[k][0]/d_size,rounded) )+' %'
                freq[k] = tuple(freq[k])
        else: 
            
            for k in freq.keys():
                freq[k][1] = str(100*freq[k][0]/d_size)+' %'
                freq[k] = tuple(freq[k])
    
    else:
         for k in duplicates:
            num = freq.setdefault(k,0)
            freq[k] = num+1
    
    return freq
	
def hamming_distance(sa, sb):

    assert(len(sa)==len(sb))
    dis = 0

    for i in range(len(sa)):
        if sa[i] != sb[i]: dis += 1

    return dis

@profile
def execution():                  

	random.seed(None)
	argm = 1
	instance = []


	while argm < len(sys.argv):
		instance.append(sys.argv[argm])
		argm += 1
		

	al = set([c for s in instance for c in s])
	#Conversion
	instance = [list(inst) for inst in instance]

	maxlen = len(instance[0])

	def mutate(arg,map_len,c_len=maxlen):
		random.seed(None)
		randomCharacter = random.randint(0,c_len-1)
		rand = random.randint(1,map_len[randomCharacter])
		arg[randomCharacter] = chr( (ord(arg[randomCharacter])-65+rand)%map_len[randomCharacter]  + 65)

	mapping = []
	map_len = []

	maxAl = 0



	for i in range(maxlen):


		common = count_all([ k[i] for k in instance ])
		common = list(common.items())

		common = sorted( common,key = lambda ai: ai[1],reverse=True )
		map_len.append( len(common) )
		maxAl = max( maxAl,len(common) )


		charmap = {}
		tempm = {}
			
		for j in range(len(common)):
			charmap[chr(65+j)] = common[j][0]
			tempm[common[j][0]] = chr(65+j)

		for elm in instance:
			elm[i] = tempm[ elm[i] ]
		

		mapping.append(charmap)


	combinations = functools.reduce(lambda x,y: x*y, map_len)


	candidate = ['A' for c in range(maxlen) ]

	best = candidate[:]
	best_k = maxlen


	for com in range(combinations):

		pos = com
		
		for c in range( len(map_len)-1 ):

			offset = functools.reduce(lambda x,y: x*y, map_len[c+1:])
			candidate[c] = chr ( int(pos/offset) + 65 )
			pos = pos % offset

		candidate[len(map_len)-1] = chr(pos + 65)

		k = max( map(lambda l: hamming_distance(candidate,l),instance)  )


		if( k < best_k ): 
			best = candidate[:]
			best_k = k 



	print("\nRESULT: \n")
	print( str(best) )
	for c in range(maxlen):
		best[c] = mapping[c][ best[c] ]
	print('Best finding solution: '+ ''.join(best) )
	print('k = '+ str(best_k)+'\n')

if __name__ == "__main__":
	execution()




      
