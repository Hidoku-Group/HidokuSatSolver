#Bedingungen für ein gültiges Hidoku
n=Länge des Hidokus
i=Zelle
-------------
|   |   |   |
| 1 | 2 | 3 |
|   |   |   |
-------------
|   |   |   |
| 4 | 5 | 6 |
|   |   |   |
-------------
|   |   |   |
| 7 | 8 | 9 |
|   |   |   |
-------------
j=Wert der aktuellen Zelle

1. Für j!=n^2: Genau 1 Nachbar hat eine um eins größere Zahl als die aktuelle Zelle selbst
2. Für j!=1: Genau 1 Nachbar hat eine um eins kleiner Zahl asl die aktuelle Zelle selbst
