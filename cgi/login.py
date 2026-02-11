#!/usr/bin/env python3
import sys
import os

data = sys.stdin.read()   # <-- là tu récupères le POST

print("Content-Type: text/html")
print()

print("<html><body>")
print("<h1>Réponse serveur</h1>")
print(f"<p>Données reçues : {data}</p>")
print("</body></html>")
