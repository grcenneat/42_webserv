cp ./www/content/ourindex_copy.html ./www/content/ourindex.html
curl -X TRACE -I http://localhost:8080/
curl -X CONNECT -I http://localhost:8080/
curl -X OPTIONS -I http://localhost:8080/