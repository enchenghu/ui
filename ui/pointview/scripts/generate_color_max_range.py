r = []
g = []
b = []

with open("cyclops256.csv", 'r') as f:
	lines = f.readlines()
	for line in lines:
		range = line.split(',')[2]
		if range == '20':
		     r.append(255)
		     g.append(0)
		     b.append(0)
		if range == '100':
		     r.append(0)
		     g.append(255)
		     b.append(0)
		if range == '200':
		     r.append(255)
		     g.append(255)
		     b.append(0)
		if range == '300':
		     r.append(255)
		     g.append(255)
		     b.append(255)
print(r)
print(g)
print(b)
