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


argm = 1
instance = []

while argm < len(sys.argv):
	instance.append(sys.argv[argm])
	argm += 1
	

#Conversion
instance = [list(inst) for inst in instance]

maxlen = len(instance[0])

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

print("./cprog ",end='')
print(str(maxlen)+" ",end='')

for l in map_len:
	print(str(l)+" ",end='')	

for inst in [ ''.join(s) for s in instance] : 
	print(inst+" ",end = "")

