import random
import sys
import profile

# This version is for variable lengths

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
	
def hamming_distance(sa, sb, maxls):

    assert(len(sa)==len(sb))
    dis = 0

    for i in range(len(sa)):
        if sa[i] != sb[i]: dis += 1

    return dis/maxls

def mutate(arg,map_len):
    random.seed(None)
    randomCharacter = random.randint(0,arg[1]-1)
    rand = random.randint(1,map_len[randomCharacter])
    arg[0][randomCharacter] = chr( (ord(arg[0][randomCharacter])-65+rand)%map_len[randomCharacter]  + 65)

# 1. Generate the problem instance

@profile
def execution():                   

	random.seed(None)
	argm = 10
	instance = []
	generation = int(sys.argv[2])
	mutation_rate = int(sys.argv[4])
	best_counter = int(sys.argv[6])
	number_cand = int(sys.argv[8])

	while argm < len(sys.argv):
		instance.append(sys.argv[argm])
		argm += 1
		
	

	al = set([c for s in instance for c in s])
	#Conversion
	instance = [list(inst) for inst in instance]
	maxlen = max([ len(s) for s in instance ])
	instance = [ ( i,len(i) ) for i in instance ]


	# 2. Normalization of the strings

	# for the first character up to the character 'maxlen'
	mapping = []
	map_len = []

	maxAl = 0


	for i in range(maxlen):


		common = count_all([ k[0][i] for k in instance if i < len(k[0]) ])
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
			if i < len(elm[0]): elm[0][i] = tempm[elm[0][i]]
		

		mapping.append(charmap)
		
	eqInstances = [ (l[0]+['@']*(maxlen-l[1]), l[1]) for l in instance ]

	# 3. initial solution candidates


	initial = [  ( ['A']*(j+1)+['@']*(maxlen-1-j) , j+1) for j in range(maxlen) ]



	parents = initial[:]


	for init in initial:
		for mr in range(mutation_rate): mutate(init,map_len)
		


	parents.extend(initial)


	minima = m = 9999
	winner = ['',0]



	for g in range(generation):


		fitnessDistanz = list (map(lambda candid: max( map(lambda l: hamming_distance(candid,l,max(l[1],candid[1])) , eqInstances ) ) , parents) )
		result = 0
		m = fitnessDistanz[0]
		for i in range(len(fitnessDistanz)):
			if m > fitnessDistanz[i]: 
				result = i
				m = fitnessDistanz[i]

		predecessor = ''.join(winner[0])	
		winner = parents[result]
		
		DELETE'Best candidate of the generation: '+ str(winner) )


		
		best_counter -= int(minima <= m)
		
		if (not best_counter) or predecessor==''.join(winner[0]): 

			break;

		minima = m

		parents = [ [ winner[0][:],winner[1] ] for a in range(number_cand) ]

		
		for cand in parents:
			for mr in range(mutation_rate):
				mutate(cand,map_len)

				if cand[1] > 1: 

					cand[0][cand[1]-1] = chr ( ord(cand[0][cand[1]-1]) + probability*(-ord( cand[0][cand[1]-1] )+ord('@')) )
					cand[1] -= probability

				if cand[1] < maxlen:
 		
					probability = int(1/random.randint(1,4))
					cand[0][cand[1]] = chr ( ord(cand[0][cand[1]]) + probability*random.randint(1,maxAl-1) )
					cand[1] += probability
 

		parents.append([ winner[0][:],winner[1] ])
		

	winner[0] = list( ''.join(winner[0]).replace('@','') )

	for c in range(winner[1]):
		winner[0][c] = mapping[c][ winner[0][c] ]
	print('Best finding solution: '+ ''.join(winner[0]) )
	print('k = '+ str(m) )

if __name__ == "__main__":
	execution()

