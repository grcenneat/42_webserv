import requests
from requests.auth import HTTPBasicAuth

def execRequest(sentence, url, text):
	print(sentence, ", wanna execute? (y/n)")
	if (input() == 'y'):
		tmp = sentence.split()[0]
		if tmp == 'GET':
			r = requests.get(url)
		elif tmp == 'HEAD':
			r = requests.head(url)
		elif tmp == 'POST':
			r = requests.post(url, data=text)
		elif tmp == 'PUT':
			r = requests.put(url)
		elif tmp == 'DELETE':
			r = requests.delete(url)
		getResponse(r)

def getResponse(r):
	print("*************************")
	print("* StatusCoce :: ", end="")
	print(r.status_code)
	print("* Headers    :: ", end="")
	print(r.headers)
	print("* Body       :: ")
	print(r.text)
	print("*************************")

over100 = "00000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"

tcs = [['GET on 8080/', 'http://localhost:8080/', 0],
['HEAD on 8080/', 'http://localhost:8080/', 0],
['POST on 8080/', 'http://localhost:8080/', "hello"],
['PUT on 8080/', 'http://localhost:8080/', 0],
['POST on 8080/maxbody', 'http://localhost:8080/maxbody', "hi"],
['POST on 8080/maxbody', 'http://localhost:8080/maxbody', over100],
['DELETE on 8080/', 'http://localhost:8080/', 0]
]

for idx, tc in enumerate(tcs):
	print("(", idx + 1, "/", len(tcs), ")")
	execRequest(tc[0], tc[1], tc[2])

print("Auth test")
r = requests.get('http://localhost:8080/auth/auth.html', headers={'Authorization': 'dGVzdDp0ZXN0'})
getResponse(r)