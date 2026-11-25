
#include <iostream>
#include <string>
#include <vector>
#include <conio.h>
#include <windows.h>
#include <cctype>

using namespace std;

// ==========================================
// CZASY DZIALANIA ZAPOR
// ==========================================

const DWORD CZAS_ZAMYKANIA = 5000;
const DWORD CZAS_OTWIERANIA = 5000;

// ==========================================
// STANY MASZYNY
// ==========================================

enum StanPrzejazdu {
    OTWARTE,
    ZAMYKANIE,
    ZAMKNIETE,
    OTWIERANIE,
    ZATRZYMANE_PRZY_ZAMYKANIU,
    ZATRZYMANE_PRZY_OTWIERANIU
};

// ==========================================
// ZAMIANA STANU NA TEKST
// ==========================================

string nazwaStanu(StanPrzejazdu stan) {
    switch (stan) {
        case OTWARTE:
            return "OTWARTE";

        case ZAMYKANIE:
            return "ZAMYKANIE";

        case ZAMKNIETE:
            return "ZAMKNIETE";

        case OTWIERANIE:
            return "OTWIERANIE";

        case ZATRZYMANE_PRZY_ZAMYKANIU:
            return "ZATRZYMANE W POLOWIE (ZAMYKANIE)";

        case ZATRZYMANE_PRZY_OTWIERANIU:
            return "ZATRZYMANE W POLOWIE (OTWIERANIE)";
    }

    return "NIEZNANY";
}

// ==========================================
// WYSWIETLANIE STATUSU
// ==========================================

void pokazStatus(
    StanPrzejazdu stan,
    bool alarm,
    bool obiektNaPrzejezdzie,
    const vector<string>& mandaty
) {
    cout << "\n---------- STATUS PRZEJAZDU ----------\n";

    cout << "Stan zapor:       " << nazwaStanu(stan) << endl;

    cout << "Czujnik ruchu:    ";

    if (obiektNaPrzejezdzie)
        cout << "WYKRYTO OBIEKT\n";
    else
        cout << "BRAK OBIEKTU\n";

    cout << "Alarm:            ";

    if (alarm)
        cout << "WLACZONY!\n";
    else
        cout << "WYLACZONY\n";

    cout << "Liczba mandatow:  " << mandaty.size() << endl;

    cout << "--------------------------------------\n";
}

// ==========================================
// MENU STEROWANIA
// ==========================================

void pokazMenu() {
    cout << "\n========== STEROWANIE ==========\n";
    cout << "[P] Zbliza sie pociag - zamykanie\n";
    cout << "[T] Pociag przejechal - otwieranie\n";
    cout << "[M] Czujnik ruchu\n";
    cout << "[V] Symulacja wjazdu pojazdu\n";
    cout << "[A] Uruchomienie alarmu\n";
    cout << "[G] Sterowanie straznika (start/stop/wznow)\n";
    cout << "[H] Historia mandatow\n";
    cout << "[R] Reset alarmu\n";
    cout << "[?] Ponowne wyswietlenie menu\n";
    cout << "[Q] Wyjscie z programu\n";
    cout << "================================\n";
}

// ==========================================
// HISTORIA MANDATOW
// ==========================================

void pokazMandaty(const vector<string>& mandaty) {

    cout << "\n========== HISTORIA MANDATOW ==========\n";

    if (mandaty.empty()) {

        cout << "Brak wystawionych mandatow.\n";

    }
    else {

        for (size_t i = 0; i < mandaty.size(); i++) {

            cout << "\nMandat nr " << i + 1 << endl;
            cout << "Powod: " << mandaty[i] << endl;

        }
    }

    cout << "\nLaczna liczba mandatow: " << mandaty.size() << endl;

    cout << "=======================================\n";
}

// ==========================================
// GLOWNY PROGRAM
// ==========================================

int main() {

    // ZMIENNE MASZYNY

    StanPrzejazdu stan = OTWARTE;

    bool alarm = false;
    bool obiektNaPrzejezdzie = false;

    vector<string> mandaty;

    char wybor;
    bool dziala = true;

    // ZMIENNE CZASOWE

    DWORD koniecRuchu = 0;
    DWORD pozostalyCzas = 0;

    // URUCHOMIENIE PROGRAMU

    cout << "========================================\n";
    cout << "       SYSTEM PRZEJAZDU KOLEJOWEGO\n";
    cout << "========================================\n";

    cout << "System zostal uruchomiony poprawnie.\n";

    cout << "Czas zamykania: " << CZAS_ZAMYKANIA / 1000 << " sekund\n";
    cout << "Czas otwierania: " << CZAS_OTWIERANIA / 1000 << " sekund\n";

    pokazStatus(stan, alarm, obiektNaPrzejezdzie, mandaty);
    pokazMenu();

    cout << "\nNasluchiwanie klawiatury...\n";

    // ======================================
    // GLOWNA PETLA PROGRAMU
    // ======================================

    while (dziala) {

        DWORD teraz = GetTickCount();

        // ==================================
        // AUTOMATYCZNE ZAMKNIECIE ZAPOR
        // ==================================

        if (stan == ZAMYKANIE && static_cast<LONG>(teraz - koniecRuchu) >= 0) {

            stan = ZAMKNIETE;
            pozostalyCzas = 0;

            cout << "\n[SYSTEM] Zapory zostaly automatycznie zamkniete.\n";

            if (obiektNaPrzejezdzie) {

                alarm = true;

                cout << "[ALARM] Obiekt znajduje sie na zamknietym przejezdzie!\n";
            }

            pokazStatus(stan, alarm, obiektNaPrzejezdzie, mandaty);

        }

        // ==================================
        // AUTOMATYCZNE OTWARCIE ZAPOR
        // ==================================

        else if (stan == OTWIERANIE && static_cast<LONG>(teraz - koniecRuchu) >= 0) {

            stan = OTWARTE;
            pozostalyCzas = 0;

            cout << "\n[SYSTEM] Zapory zostaly automatycznie otwarte.\n";

            pokazStatus(stan, alarm, obiektNaPrzejezdzie, mandaty);

        }

        // ==================================
        // NASLUCHIWNIE KLAWIATURY
        // ==================================

        if (_kbhit()) {

            wybor = _getch();

            // IGNOROWANIE KLAWISZY SPECJALNYCH

            if (wybor == 0 || wybor == 224) {
                _getch();
                continue;
            }

            wybor = tolower(static_cast<unsigned char>(wybor));

            cout << "\n";

            switch (wybor) {

                // ==================================
                // ROZPOCZECIE ZAMYKANIA
                // ==================================

                case 'p':

                    if (stan == OTWARTE) {

                        stan = ZAMYKANIE;

                        pozostalyCzas = CZAS_ZAMYKANIA;
                        koniecRuchu = GetTickCount() + pozostalyCzas;

                        cout << "[SYSTEM] Rozpoczeto zamykanie zapor.\n";
                        cout << "[SYSTEM] Czas zamykania: "
                             << CZAS_ZAMYKANIA / 1000 << " sekund.\n";

                        if (obiektNaPrzejezdzie) {

                            alarm = true;

                            cout << "[ALARM] Obiekt znajduje sie na przejezdzie!\n";
                        }
                    }
                    else {

                        cout << "[BLAD] Nie mozna rozpoczac zamykania zapor.\n";
                    }

                    break;

                // ==================================
                // POCIAG PRZEJECHAL - OTWIERANIE
                // ==================================

                case 't':

                    if (stan == ZAMKNIETE) {

                        stan = OTWIERANIE;

                        pozostalyCzas = CZAS_OTWIERANIA;
                        koniecRuchu = GetTickCount() + pozostalyCzas;

                        cout << "[SYSTEM] Rozpoczeto otwieranie zapor.\n";
                        cout << "[SYSTEM] Czas otwierania: "
                             << CZAS_OTWIERANIA / 1000 << " sekund.\n";
                    }
                    else {

                        cout << "[BLAD] Przejazd nie jest zamkniety.\n";
                    }

                    break;

                // ==================================
                // CZUJNIK RUCHU
                // ==================================

                case 'm':

                    obiektNaPrzejezdzie = !obiektNaPrzejezdzie;

                    if (obiektNaPrzejezdzie) {

                        cout << "[CZUJNIK] Wykryto obiekt na przejezdzie!\n";

                        if (stan != OTWARTE) {

                            alarm = true;

                            cout << "[ALARM] Obiekt znajduje sie na przejezdzie!\n";
                        }
                    }
                    else {

                        cout << "[CZUJNIK] Obiekt opuscil przejazd.\n";
                        cout << "[SYSTEM] Mozna zresetowac alarm.\n";
                    }

                    break;

                // ==================================
                // WJAZD POJAZDU I MANDAT
                // ==================================

                case 'v':

                    if (
                        stan == ZAMYKANIE ||
                        stan == OTWIERANIE ||
                        stan == ZATRZYMANE_PRZY_ZAMYKANIU ||
                        stan == ZATRZYMANE_PRZY_OTWIERANIU
                    ) {

                        alarm = true;

                        mandaty.push_back(
                            "Wjazd podczas ruchu lub zatrzymania zapor (" +
                            nazwaStanu(stan) + ")"
                        );

                        cout << "\n!!! ALARM !!!\n";
                        cout << "[SYSTEM] Wykryto nielegalny wjazd pojazdu!\n";

                        cout << "[MANDAT] Wystawiono mandat nr: "
                             << mandaty.size() << endl;
                    }
                    else if (stan == ZAMKNIETE) {

                        alarm = true;

                        cout << "[ALARM] Proba wjazdu na zamkniety przejazd!\n";
                    }
                    else {

                        cout << "[SYSTEM] Przejazd otwarty. Wjazd dozwolony.\n";
                    }

                    break;

                // ==================================
                // RECZNE URUCHOMIENIE ALARMU
                // ==================================

                case 'a':

                    alarm = true;

                    cout << "[ALARM] URUCHOMIONO ALARM RECZNIE!\n";

                    break;

                // ==================================
                // STEROWANIE STRAZNIKA
                // START / STOP / WZNOWIENIE
                // ==================================

                case 'g':

                    // OTWARTY PRZEJAZD - ROZPOCZECIE ZAMYKANIA

                    if (stan == OTWARTE) {

                        stan = ZAMYKANIE;

                        pozostalyCzas = CZAS_ZAMYKANIA;
                        koniecRuchu = GetTickCount() + pozostalyCzas;

                        cout << "[STRAZNIK] Rozpoczeto zamykanie zapor.\n";

                        if (obiektNaPrzejezdzie) {

                            alarm = true;

                            cout << "[ALARM] Obiekt znajduje sie na przejezdzie!\n";
                        }
                    }

                    // ZAMKNIETY PRZEJAZD - ROZPOCZECIE OTWIERANIA

                    else if (stan == ZAMKNIETE) {

                        stan = OTWIERANIE;

                        pozostalyCzas = CZAS_OTWIERANIA;
                        koniecRuchu = GetTickCount() + pozostalyCzas;

                        cout << "[STRAZNIK] Rozpoczeto otwieranie zapor.\n";
                    }

                    // ZATRZYMANIE PODCZAS ZAMYKANIA

                    else if (stan == ZAMYKANIE) {

                        teraz = GetTickCount();

                        if (static_cast<LONG>(koniecRuchu - teraz) > 0) {

                            pozostalyCzas = koniecRuchu - teraz;

                            stan = ZATRZYMANE_PRZY_ZAMYKANIU;

                            cout << "[STRAZNIK] Zatrzymano zapory podczas zamykania!\n";
                        }
                        else {

                            stan = ZAMKNIETE;
                            pozostalyCzas = 0;

                            cout << "[SYSTEM] Zapory zostaly zamkniete.\n";
                        }
                    }

                    // ZATRZYMANIE PODCZAS OTWIERANIA

                    else if (stan == OTWIERANIE) {

                        teraz = GetTickCount();

                        if (static_cast<LONG>(koniecRuchu - teraz) > 0) {

                            pozostalyCzas = koniecRuchu - teraz;

                            stan = ZATRZYMANE_PRZY_OTWIERANIU;

                            cout << "[STRAZNIK] Zatrzymano zapory podczas otwierania!\n";
                        }
                        else {

                            stan = OTWARTE;
                            pozostalyCzas = 0;

                            cout << "[SYSTEM] Zapory zostaly otwarte.\n";
                        }
                    }

                    // WZNOWIENIE ZAMYKANIA

                    else if (stan == ZATRZYMANE_PRZY_ZAMYKANIU) {

                        stan = ZAMYKANIE;

                        koniecRuchu = GetTickCount() + pozostalyCzas;

                        cout << "[STRAZNIK] Wznowiono zamykanie zapor.\n";
                    }

                    // WZNOWIENIE OTWIERANIA

                    else if (stan == ZATRZYMANE_PRZY_OTWIERANIU) {

                        stan = OTWIERANIE;

                        koniecRuchu = GetTickCount() + pozostalyCzas;

                        cout << "[STRAZNIK] Wznowiono otwieranie zapor.\n";
                    }

                    break;

                // ==================================
                // HISTORIA MANDATOW
                // ==================================

                case 'h':

                    pokazMandaty(mandaty);

                    break;

                // ==================================
                // RESET ALARMU
                // ==================================

                case 'r':

                    if (!obiektNaPrzejezdzie) {

                        alarm = false;

                        cout << "[SYSTEM] Alarm zostal zresetowany.\n";
                    }
                    else {

                        cout << "[BLAD] Nie mozna zresetowac alarmu!\n";
                        cout << "[SYSTEM] Obiekt nadal znajduje sie na przejezdzie.\n";
                    }

                    break;

                // ==================================
                // PONOWNE WYSWIETLENIE MENU
                // ==================================

                case '?':

                    pokazMenu();

                    break;

                // ==================================
                // ZAKONCZENIE PROGRAMU
                // ==================================

                case 'q':

                    cout << "[SYSTEM] Zamykanie programu...\n";

                    dziala = false;

                    break;

                // ==================================
                // NIEZNANY KLAWISZ
                // ==================================

                default:

                    cout << "[SYSTEM] Nieznany klawisz. Wcisnij ? aby zobaczyc menu.\n";

                    break;
            }

            // AKTUALIZACJA STATUSU

            if (dziala) {

                pokazStatus(
                    stan,
                    alarm,
                    obiektNaPrzejezdzie,
                    mandaty
                );

                cout << "Oczekiwanie na kolejny przycisk...\n";
            }
        }

        // KROTKIE OPOZNIENIE PETLI

        Sleep(10);
    }

    cout << "\nSystem zostal wylaczony.\n";

    return 0;
}
