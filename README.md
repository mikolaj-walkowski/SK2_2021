# SK2_2021

## Protokół:
Wysłanie wiadomości serwer -> klient:
- [s]msg %ip %treść - wiadomość

Wysłanie wiadomości klient -> serwer:
- [k]msg_size %d - wielkość wiadomości
- [k]msg %treść - wiadomość

Kickowanie admin -> serwer
- [k] kick %ip
- [s/err] failed kick
- [s/noerr] success kick

Kickowanie serwer -> client
- [s] kick

Dołączenie do pokoju: (jeżeli jesteś już w pokoju to automatycznie wyrzucam cie z poprzedniego)
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


