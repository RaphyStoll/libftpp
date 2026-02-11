#!/usr/bin/env python3
import os

query = os.environ.get("QUERY_STRING", "")
method = os.environ.get("REQUEST_METHOD", "")
path = os.environ.get("PATH_INFO", "")
server = os.environ.get("SERVER_NAME", "")
port = os.environ.get("SERVER_PORT", "")


print("Content-Type: text/html")
print()
print("<html><body>")
print("<h1>Résultat de recherche</h1>")
print(f"<p>Tu as cherché : {query}</p>")
print(f"<p>method : {method}</p>")
print(f"<p>path : {path}</p>")
print(f"<p>server : {server}</p>")
print(f"<p>port : {port}</p>")
print("</body></html>")
