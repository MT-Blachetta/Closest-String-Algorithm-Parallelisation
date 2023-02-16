import random
import sys

def count_all(duplicates,ratio=False,rounded=0):
    freq = {}
    #assert( isinstance(duplicates,list))
    
    if(ratio):
    
        d_size = len(duplicates)
        #if d_size == 0: d_size = 1

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


# 1. Erzeugen der Probleminstanz

#al = ('A','C','G','T')

#print('Alphabet der Eingabewörter: '+ str(al) )

# Instanzen für konstante Länge
# instance = [ ''.join( random.choices(al,k=dl ) ) for i in range(10) ]                    

#instance = ['ATCGT', 'TTA', 'AGC', 'ATCGATT']
argm = 1
instance = []

while argm < len(sys.argv):
	instance.append(sys.argv[argm])
	argm += 1
	
print('Problemargument (Eingabe): ' + str(instances) )

al = set([c for s in instance for c in s])
print('Alphabet der Eingabewörter: '+ str(al) )
print( 'Eingabewörter(Probleminstanz): '+ str(instance) )
#Konvertierung
instance = [list(inst) for inst in instance]
maxlen = max([ len(s) for s in instance ])
print('Höchste Länge der Wörter: ' + str(maxlen) )
instance = [ ( i,len(i) ) for i in instance ]
print('Wörter mit Länge gekennzeichnet: '+ str(instance) )




# 2. Normalisierung der Strings

# für erstes Zeichen bis Zeichen maxlen
mapping = []
map_len = []
maxAl = 0


print('\n Zeichen normalisieren')

for i in range(maxlen):

	print('POSITION '+ str(i) )
	common = count_all([ k[0][i] for k in instance if i < len(k[0]) ])
	#poset = [ (c,0) for c in set(al)-set(common) ]
	print('Häufigkeiten der Zeichen für Position '+str(i)+': '+str(common))
	common = list(common.items())
	print(str(common))
	common = sorted( common,key = lambda ai: ai[1],reverse=True )
	map_len.append( len(common) )
	maxAl = max(maxAl,len(common) )
	#common.extend(poset)
	print('Sortierte Häufigkeiten: ' + str(common) )
	charmap = {}
	tempm = {}
        
	for j in range(len(common)):
		charmap[chr(65+j)] = common[j][0]
		tempm[common[j][0]] = chr(65+j)
	print('Abbildung Häufigkeit-Alphabet: ' + str(charmap) )
	print('Abbildung Alphabet-Normalisierungszeichen: ' + str(tempm) )

	for elm in instance:
		if i < len(elm[0]): elm[0][i] = tempm[elm[0][i]]
    
	print('Es werden mindestens '+str(maxAl)+' Zeichen verwendet')
	# mapping += x verhält sich anders
	mapping.append(charmap)
	print('Zeichenabbildung bis Stelle '+str(i)+': '+str(mapping))
	
eqInstances = [ (l[0]+['@']*(maxlen-l[1]), l[1]) for l in instance ]
print('Umwandlung der Eingabewörter zu gleichen Längen (mit Angabe ursprünglicher Länge): '+str([''.join(k) for k in eqInstances]) )

# 3. initiale Lösungskandidaten

#initial = 'A'*dl
initial = [  ( ['A']*(j+1)+['@']*(maxlen-1-j) , j+1) for j in range(maxlen) ]
print('Initialisierung erster Lösungskandidaten: ' + str([''.join(k) for k in initial]) )

generation = 10

parents = initial[:]
print('initial Kopie: ' + str([''.join(k) for k in parents]) )

print('Elterngeneration "parents":' + str([''.join(k) for k in parents]) )

for init in initial:
	randomCharacter = random.randint(0,init[1]-1)
	rand = random.randint(1,map_len[randomCharacter])-1
	init[0][randomCharacter] = chr( (ord(init[0][randomCharacter])-65+rand)%map_len[randomCharacter]  + 65)

print('Mutationen initialer Kandidaten: '+ str([''.join(k) for k in initial]) )

parents.extend(initial)

print('Erste Generation im Evolutionären Algorithmus die getestet wird "parents": ' + str([''.join(k) for k in parents]) )

minima = m = 9999
winner = ''
clen = 0


print('START DES EVOLUTIONÄREN ALGORITHMUS \n')

for g in range(generation):

	print('GENERATION: '+ str(g) )

	fitness = list (map(lambda cand: max( map(lambda l: hamming_distance(cand,l,max(l[1],cand[1])) , eqInstances ) ) , parents) )
	print('Kandidaten: '+str([''.join(k) for k in parents]))
	print('Fitnesswerte: '+str(fitness))
	result = 0
	m = fitness[0]
	for i in range(len(fitness)):
		if m > fitness[i]: 
			result = i
			m = fitness[i]

			
	winner = parents[result]
	clen = parents[result][1]
	print('Bester Kandidat der Generation: '+ str(winner) )

# eine momentane Verschlechterung bedeutet nicht das g+1 Werte doch noch besser werden können
	print('vorheriges minimales k: '+ str(minima) )
	print('jetziges minimales k: '+ str(m) )
	
	if minima < m: 
		print('TERMINATION da keine Verbessung von k \n\n')
		break;

	minima = m

	parents = [ winner for a in range(10) ]
	print('neue Generation vervielfältigt: '+ str([''.join(k) for k in parents]) )
	
	for cand in parents:
		randomCharacter = random.randint(0,clen-1)
		rand = random.randint(1,map_len[randomCharacter])-1

		cand[0][randomCharacter] = chr( (ord(cand[0][randomCharacter])-65+rand)%map_len[randomCharacter]  + 65)
		if clen < maxlen: cand[0][clen] = chr ( ord(cand[0][clen]) + int(1/random.randint(0,4))*random.randint(0,map_len[randomCharacter]) )
	
	#candidates += winner
	parents.append(winner)
	print('Mutation durchgeführt\n')
	

print('ERGEBNISSE \n')
for c in range(clen):
    winner[0][c] = mapping[c][ winner[0][c] ]
print('Beste gefundene Lösung: '+ ''.join(winner[0]) )
print('k = '+ str(m) )



