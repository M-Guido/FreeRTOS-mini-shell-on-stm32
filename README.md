# FreeRTOS Mini Shell on STM32

Prosty projekt na STM32 z FreeRTOS-em i shellem przez UART.  
Komendy wpisuje się w terminalu szeregowym, np. PuTTY.

Projekt obsługuje sterowanie diodą wewnętrzną STM32 oraz zewnętrzną diodą LED podłączoną do GPIO.

## Funkcje

- FreeRTOS
- komunikacja UART
- prosty shell tekstowy
- obsługa komend przez PuTTY
- sterowanie diodą wewnętrzną
- sterowanie diodą zewnętrzną
- tryb migania LED

## Dzialanie 
STM32 odbiera znaki z UART-a przez przerwanie.
Po wciśnięciu Entera zebrany tekst jest traktowany jako komenda i przekazywany do prostego parsera

## UART

Domyślne ustawienia terminala:

```text
Baud rate:    115200
Data bits:    8
Parity:       None
Stop bits:    1
Flow control: None

```

## Komendy 

```text
help
led blink     --miganie dioda stm-ki
led wewn on   --zaświecenie diody stm-ki
led wewn off  -- wyłączenie diody stm-ki
led zewn on   --zaświecenie diody podłączonej zewnętrznie do PA1 STM32-NUCLEO411RE
led zewn off  --wyłączenie diody zewnetrznej
```

## Uruchomienie

```text
sklonuj repozytorium do katalogu roboczego
Otwórz projekt w STM32CubeIDE.
Zbuduj projekt.
Wgraj program na STM32.(Mozliwe że projekt od razu po wgraniu nie bedzie działać należy wtedy wyłączyć debugger i zobaczyć czy wtedy PuTTy zadziała :D )
Otwórz PuTTY.
Wybierz odpowiedni port COM.
Ustaw UART na 115200 8N1.
Wpisz help.
Wpisuj komendy ktore chcesz zobaczyc
```
