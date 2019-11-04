# measure
Projekt działa na Arduino Leonardo
potrzebne są także:
jeden przycisk zwykły, przycisk dotykowy, czujnik ultradźwiękowy do mierzenia odległości, moduł bluetooth HC-06, wyświetlacz LCD 2x16, ultradźwiękowy czujnik odległości, itp.

Po uruchomieniu arduino pyta o hasło. Hasło wpisuje się alfabetem morse'a korzystając z przycisku dotykowego, a po skończeniu wpisywania należy wcisnąć zwykły przycisk (obecne hasło to ... )
Następnie po zalogowaniu pyta czy chcemy zmienić hasło . Zmienione hasło zapisuje w wewnętrznej pamięci płytki. Jeśli hasła nie zmieniamy to przechodzi do pomiaru odległości. Wynik pomiaru wysyła przez moduł bluetooth na włączoną komórkę z aplikacją, która odczytuje na głos wynik (aplikacja: Arduino Bluetooth Text to Speech ). Mierzenie odległości można zawiesić klikając zwykły przycisk.
