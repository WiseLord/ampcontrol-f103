import json

File = open('karadio_playlist', 'r')
Lines = File.readlines()

for line in Lines:
	ld = json.loads(line)
	if ld['URL']:
		print('http://' + ld['URL'] + ':' + ld['Port'] + ld['File'] + '#' + ld['Name'])

