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
	# 1. Generate the problem INPUT_strings

	random.seed(None)
	console_arguments= 10
	INPUT_strings = []
	soulution_search_generations = int(sys.argv[2])
	NumberCharMutations = int(sys.argv[4])
	forceSearchContinuation = int(sys.argv[6])
	searchPopulationSize = int(sys.argv[8])

	while console_arguments< len(sys.argv):
		INPUT_strings.append(sys.argv[argm])
		console_arguments+= 1
		

	al = set([c for s in INPUT_strings for c in s])
	#Conversion
	INPUT_strings = [list(inst) for inst in INPUT_strings]

	stringLength = len(INPUT_strings[0])


	def mutate(arg,numberCharactersAtPosition,lengthInCharacters=stringLength):
		random.seed(None)
		randomCharacter = random.randint(0,lengthInCharacters-1)
		rand = random.randint(1,numberCharactersAtPosition[randomCharacter])
		arg[randomCharacter] = chr( (ord(arg[randomCharacter])-65+rand)%numberCharactersAtPosition[randomCharacter]  + 65)

	characterMapping = []
	numberCharactersAtPosition = []

	maxMappedChars = 0



	for i in range(stringLength):


		charByFrequencies = count_all([ k[i] for k in INPUT_strings ])
		charByFrequencies = list(charByFrequencies.items())
		charByFrequencies = sorted( charByFrequencies,key = lambda ai: ai[1],reverse=True )
		numberCharactersAtPosition.append( len(charByFrequencies) )
		maxMappedChars = max( maxMappedChars,len(charByFrequencies) )
		charMapforPosition = {}
		TranslateCharacters = {}
			
		for j in range(len(charByFrequencies)):
			charMapforPosition[chr(65+j)] = charByFrequencies[j][0]
			TranslateCharacters[charByFrequencies[j][0]] = chr(65+j)

		for elm in INPUT_strings:
			elm[i] = TranslateCharacters[ elm[i] ]
		
		characterMapping.append(charMapforPosition)

	minima = 9999
	predecessorSoulution = [ 'x' for x in range(stringLength) ]
	generationWinner = [ 'A' for a in range(stringLength) ]
	parentGeneration = []

	k_distanceValue = max( map( lambda l: hamming_distance(generationWinner,l) , INPUT_strings ) )


	for g in range(soulution_search_generations):


		
		forceSearchContinuation -= int(minima <= k_distanceValue)
		
		if (not forceSearchContinuation) or predecessorSoulution==''.join(generationWinner): 
			break;

		minima = k_distanceValue
		predecessorSoulution = ''.join(generationWinner)

		parentGeneration = [ generationWinner[:] for a in range(searchPopulationSize) ]
		
		for cand in parentGeneration:
			for mr in range(NumberCharMutations):
				mutate(cand,numberCharactersAtPosition)

		parentGeneration.append( generationWinner[:] )
		distanceFitness = list (map(lambda candid: max( map(lambda l: hamming_distance(candid,l) , INPUT_strings ) ) , parentGeneration) )
		result = 0
		k_distanceValue = distanceFitness[0]
		for i in range(len(distanceFitness)):
			if k_distanceValue > distanceFitness[i]: 
				result = i
				k_distanceValue = distanceFitness[i]

		generationWinner = parentGeneration[result]



	print('\nRESULT\n\n')
	print( str(generationWinner) )
	for c in range(stringLength):
		generationWinner[c] = characterMapping[c][ generationWinner[c] ]
	print('\nBest finding solution: '+ ''.join(generationWinner) )
	print('k = '+ str(k_distanceValue) )

if __name__ == "__main__":
	execution()	
