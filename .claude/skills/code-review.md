# Skill: Code Review
Aktivierung: Wenn jemand einen Code-Review, Feedback zum Code oder eine Code-Prüfung anfragt.
## Review-Prozess
### Schritt 1: Überblick verschaffen
Lies zuerst alle relevanten Dateien bevor du kommentierst.
### Schritt 2: Prüfe nach diesen Kriterien
Korrektheit: Logikfehler, nicht behandelte Randfälle, falsche Annahmen
Robustheit: Fehlerbehandlung, ungültige Eingaben, Grenzwerte
Sicherheit: Unsichere Operationen, mögliche Abstürze, keine Validierung
Lesbarkeit: Benennung, Komplexität, fehlende Kommentare
Tests: Fehlen wichtige Test-Cases? Testen die Tests wirklich etwas?
### Schritt 3: Report erstellen
KRITISCH (sofort beheben – kein Merge ohne Fix):
[Liste mit Zeilen-Referenz und konkretem Fix-Vorschlag]
WARNUNG (vor nächstem Release):
[Liste]
EMPFEHLUNGEN:
[Liste]
GUT gemacht:
[Was positiv aufgefallen ist]
## Nach dem Review
Zeige immer den konkreten Fix, nicht nur die Beschreibung des Problems.
