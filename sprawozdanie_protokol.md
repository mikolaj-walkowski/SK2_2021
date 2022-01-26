## Protokół:
Wysłanie wiadomości serwer -> klient:
- [s]msg %ip %treść - wiadomość

Wysłanie wiadomości klient -> serwer:
- [k]msg %treść - wiadomość

Wyrzucanie z pokoju admin -> serwer
- [k] kick %ip
- [s/err] failed kick
- [s/noerr] success kick

Wyrzucanie z pokoju serwer -> wyrzucany klient
- [s] kick

Dołączenie do pokoju: (jeżeli jesteś już w pokoju to automatycznie serwer wyrzuca cie z poprzedniego)
- [k] join %nazwa
- [s/err] failed join
- [s/noerr] success

Opuszcenie pokoju:
- [k] leave
- [s/err] failed
- [s/noerr] success

Tworzenie pokoju: 
- [k] create %nazwa
- [s/err] failed create
- [s/noerr] success

Wszystkie wiadomości/polecenia kończą się znakiem null w ASCII ('/0').
