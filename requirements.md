# Specyfikacja Funkcjonalnoœci

## 1. Zarz¹dzanie Rozgrywk¹ (Game Management)
* **Nowa Gra:** U¿ytkownik ma mo¿liwoœæ utworzenia nowej gry.
* **Wybór Koloru:**
    * U¿ytkownik wybiera kolor bierek.
    * Alternatywnie system przydziela kolor automatycznie.
* **Inicjalizacja:** System poprawnie inicjalizuje planszê do stanu pocz¹tkowego.
* **Poddanie (Surrender):** Dostêpny i widoczny przycisk umo¿liwiaj¹cy u¿ytkownikowi poddanie partii.
* **Zegar (Timer):** System limituje czas u¿ytkownika na wykonanie ruchu.

## 2. Logika i Zasady (Game Logic & Validation)
* **Monitorowanie Stanu:** System ci¹gle weryfikuje status gry pod k¹tem Szacha, Mata oraz Remisu (Draw).
* **Zaawansowana Walidacja:**
    * Wykrywanie i obs³uga bicia w przelocie (En-passant).
    * Wykrywanie figur zwi¹zanych (Pinned figures) i blokowanie nielegalnych ruchów ods³aniaj¹cych króla.

## 3. Tryby Gry (Game Modes)
* **Gra z AI:** Rozgrywka przeciwko komputerowi.
    * Obs³uga poziomów trudnoœci (Easy, Medium, Hard).
* **Lokalne PvP (Opcjonalne):** Mo¿liwoœæ gry dla dwóch graczy na jednym urz¹dzeniu.

## 4. Interfejs i UX
* **Obs³uga B³êdów:** Aplikacja obs³uguje b³êdy w sposób przyjazny dla u¿ytkownika (graceful error handling), nie przerywaj¹c dzia³ania programu.
* **GUI (Etap póŸniejszy):** Implementacja interfejsu graficznego (okienkowego).

## 5. Zapewnienie Jakoœci (Quality Assurance)
* **Testy:** Zaimplementowane testy weryfikuj¹ce poprawnoœæ:
    * Reprezentacji planszy.
    * Generowania i walidacji ruchów.
    * Dzia³ania silnika gry.