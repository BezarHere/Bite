
def rr():
	p = 0xEDB88320
	r = 0
	b = 0
	crc = [0]
	while (b := b + 1) & 0xff:
		r = b
		for bit in range(8, 0, -1):
			if r & 1:
				r = (r >> 1) ^ p
			else:
				r >>= 1
		crc.append(r)
	return crc

s = ''
for i, v in enumerate(rr()):
	if i % 8 == 0:
		s += '\n'
	n = hex(v)
	s += n + (' ' * (10 - len(n))) + ', '
print(s)