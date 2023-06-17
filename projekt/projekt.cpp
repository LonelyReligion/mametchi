#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>

#include <iostream>
#include <string>
#include <map>
#include <vector>
#include <sstream>
#include <regex> //sprawdzanie logina i hasla
#include <filesystem> //lokalizacja tla, mozliwie pozostalych spriteow

import zwierzak;
import interfejs;
import jedzenie;
import pole_tekstowe;
import guzik;

bool DEBUG = true;

/*
TODO
> logowanie jako metoda klasy interface
> wczytywanie bazy uzytkownikow
> wczytywanie baz zwierzakow
> staty
> wczytywanie zwierzakow
> wczytaj sprite moze byc wykonane tylko raz przy tworzeniu
> moze niech osobny watek generuje pozycje bobasa? 
> watki do aktualizowania statystyk
> wskazowka/instrukcja/strzaleczka z esc
*/

int main()
{
    std::regex puste(".*\\s+.*"); //cos spacja cos

    interfejs inter;

    sf::Color rozowy(237, 88, 177);
    sf::Color wanilia(252, 234, 154);
    sf::Color pomarancza(247, 182, 101);

    //wczytaj bazy
    std::map<std::string, uzytkownik> baza_uzytkownikow; //nazwa uzytkownika, uzytkownik
    std::map<std::string, stworzenie *> baza_zwierzakow; //nazwa uzytkownika, wzkaznik na zwierzatko (konieczne do zastosowania polimorfizmu, tak aby wykonywaly sie odpowiednie wersje metod)

    if (!inter.wczytaj_baze_uzytkownikow()) {
        std::cout << "Ladowanie bazy uzytkownikow nie powiodlo sie. Nastapi zakonczenie pracy programu." << std::endl;
        std::cout << "Sprawdz poprawnosc danych w pliku i sprobuj ponownie." << std::endl;
        std::cout << "WSKAZOWKA: Sprawdz czy wszystkie wiersze zawieraja nazwe uzytkownika i haslo. " << std::endl;
        std::cout << "WSKAZOWKA: Sprawdz czy nazwy uzytkownikow sie nie powtarzaja. " << std::endl;
        return 0;
    };

    if (!inter.wczytaj_baze_zwierzakow()) {
        std::cout << "Ladowanie bazy zwierzakow nie powiodlo sie. Nastapi zakonczenie pracy programu." << std::endl;
        std::cout << "Sprawdz poprawnosc danych w pliku i sprobuj ponownie." << std::endl;
        return 0;
    };

    uzytkownik testowy("admin", "admin");
    stworzenie testowe();

    baza_uzytkownikow[testowy.zwroc_nazwa_uzytkownika()] = testowy;
    baza_zwierzakow[testowy.zwroc_nazwa_uzytkownika()] = new stworzenie();

    /*The OFL allows the licensed fonts to be used, studied, modified and redistributed freely as
    long as they are not sold by themselves. The fonts, including any derivative works, can be bundled,
    embedded, redistributed and/or sold with any software provided that any reserved names are not used
    by derivative works. The fonts and derivatives, however, cannot be released under any other type of license.
    The requirement for fonts to remain under this license does not apply to any document created using
    the fonts or their derivatives.*/

    sf::Font font;
    if (!font.loadFromFile("munro.ttf"))
    {
        std::cout << "Ladowanie fonta zakonczone niepowodzeniem" << std::endl;
    };

    sf::Clock czas;
    sf::Time czas_od_wlaczenia_programu = czas.getElapsedTime(); //.restart() robi to samo i dodatkowo zeruje zegar, nie dziala poprawnie
    stworzenie zwierze;
    
    //ladowanie bobasa, powinno byc zamkniete w klasie, zostawione w mainie na potrzeby testowania
    Bobas bobasek;
    bobasek.wczytaj_sprite();

    //tla //zampknac w interface?
    sf::Text bufet("Bufet", font, 50);
    bufet.setFillColor(rozowy);
    bufet.setOrigin(sf::Vector2f(-350.f, -150.f));

    

    ekran ekran_pokoju("obrazki/pokoj.png", {});

    /////////////
    
    //menu 
    sf::Color kolor_tla_przyciskow = wanilia;
    sf::Color kolor_tekstu_przyciskow = rozowy;
    sf::Color kolor_tla_wcisniete = pomarancza;
    sf::Vector2f rozmiar_przyciskow = { 200, 50 };

    //glowne menu
    przycisk staty("Statystyki", rozmiar_przyciskow, 20, kolor_tla_przyciskow, kolor_tekstu_przyciskow, { 50, 20 }, font); //wezszy, inne kolory
    przycisk lodow("Bufet", rozmiar_przyciskow, 20, kolor_tla_przyciskow, kolor_tekstu_przyciskow, { 300, 20 }, font);
    przycisk zabaw("Zabawa", rozmiar_przyciskow, 20, kolor_tla_przyciskow, kolor_tekstu_przyciskow, { 550, 20 }, font);
    przycisk sprza("Sprzatanie", rozmiar_przyciskow, 20, kolor_tla_przyciskow, kolor_tekstu_przyciskow, { 550, 500 }, font);
    przycisk wczyt("Wczytaj", rozmiar_przyciskow, 20, kolor_tla_przyciskow, kolor_tekstu_przyciskow, { 300, 500 }, font);
    przycisk zapis("Zapisz", rozmiar_przyciskow, 20, kolor_tla_przyciskow, kolor_tekstu_przyciskow, { 50, 500 }, font);

    /// jedzenie
    przycisk dania("Dania", rozmiar_przyciskow, 20, kolor_tla_przyciskow, kolor_tekstu_przyciskow, { 300, 250 }, font);
    przycisk desery("Desery", rozmiar_przyciskow, 20, kolor_tla_przyciskow, kolor_tekstu_przyciskow, { 300, 350 }, font);
    przycisk sklep("Sklep", rozmiar_przyciskow, 20, kolor_tla_przyciskow, kolor_tekstu_przyciskow, { 300, 450 }, font);
    /// okno
    sf::RenderWindow okno(sf::VideoMode(800, 600), "Moje zwierzatko!", sf::Style::Default); //titlebar, resize, close
    okno.setVerticalSyncEnabled(true); //synchronizacja czasu odswierzania z monitorem
    okno.setKeyRepeatEnabled(false);

    //zaloguj
    bool zalogowany = 0;
    bool mamylogin = 0;
    std::string nazwa_uzytkownika;
    std::string kod;

    Pole login(15, rozowy, false);
    login.ustawFont(font);
    login.ustawLimit(true, 20);
    login.ustawPozycje({50, 200});

    sf::Text kontrolki_logowania("Enter (wybierz pole tekstowe) | Shift (zatwierdz) | Escape (odznacz pole tekstowe)", font, 20);
    kontrolki_logowania.setFillColor(rozowy);
    kontrolki_logowania.setOrigin(sf::Vector2f(-50.f, -500.f));

    sf::Text instrukcja_logowania("Podaj swoja nazwe uzytkownika", font, 25);
    instrukcja_logowania.setFillColor(rozowy);
    instrukcja_logowania.setOrigin(sf::Vector2f(-50.f, -100.f));

    ekran ekran_logowania("obrazki/logowanie.png", { instrukcja_logowania, kontrolki_logowania });

    //// staty
    bool wyswietl_statystyki = 0;

    sf::Text imie("", font, 30);
    imie.setFillColor(sf::Color::White);
    imie.setOrigin(sf::Vector2f(-150.f, -100.f));

    sf::Text wiek("", font, 30);
    wiek.setFillColor(sf::Color::White);
    wiek.setOrigin(sf::Vector2f(-150.f, -150.f));

    sf::Text statystyki("STATYSTYKI", font, 50);
    statystyki.setFillColor(sf::Color::White);
    statystyki.setOrigin(sf::Vector2f(-300.f, 0.f));
    ekran ekran_statystyk("obrazki/statystyki/statystyki.png", {statystyki, imie, wiek});

    sf::Texture glodny;
    if (!glodny.loadFromFile("obrazki/statystyki/glodny_ikonka.png")) {
        std::cout << "ladowanie tekstury glodny_ikonka zakonczone niepowodzeniem" << std::endl;
    };
    glodny.setSmooth(false);

    sf::Texture syty;
    if (!syty.loadFromFile("obrazki/statystyki/najedzony_ikonka.png")) {
        std::cout << "ladowanie tekstury najedzony_ikonka zakonczone niepowodzeniem" << std::endl;
    };
    syty.setSmooth(false);

    sf::Texture smutny;
    if (!smutny.loadFromFile("obrazki/statystyki/smutas.png")) {
        std::cout << "ladowanie tekstury ikonka smutas zakonczone niepowodzeniem" << std::endl;
    };
    glodny.setSmooth(false);

    sf::Texture radosny;
    if (!radosny.loadFromFile("obrazki/statystyki/chichrok.png")) {
        std::cout << "ladowanie tekstury ikonka chichrok zakonczone niepowodzeniem" << std::endl;
    };
    syty.setSmooth(false);

    std::vector<std::vector <sf::Sprite>> szczescie;
    std::vector<std::vector <sf::Sprite>> glod;

    std::vector<bool>wybor_glodu;
    std::vector<bool> wybor_szczescia;

    auto first = -95.f;

    for (int i = 0; i < 5; i++) {
        sf::Sprite nasycony(syty);
        sf::Sprite wyglodzony(glodny);

        sf::Sprite uszczesliwiony(radosny);
        sf::Sprite zasmucony(smutny);

        nasycony.setOrigin(sf::Vector2f(-100.f + i * first, -200.f));
        wyglodzony.setOrigin(sf::Vector2f(-100.f + i * first, -200.f));

        uszczesliwiony.setOrigin(sf::Vector2f(-100.f + i * first, -350.f));
        zasmucony.setOrigin(sf::Vector2f(-100.f + i * first, -350.f));

        glod.push_back({nasycony, wyglodzony});
        szczescie.push_back({ uszczesliwiony, zasmucony });
    };

    //jedzenie
    bool jedzenie_tf = 0;
    bool jemy_dania = 0;
    bool jemy_slodycze = 0;
    
    sf::Text informacje_o_daniu_0("INFO O DANIU", font, 20);
    sf::Text informacje_o_daniu_1("INFO O DANIU DRUGA LINIJKA", font, 20);
    informacje_o_daniu_0.setOrigin(sf::Vector2f(-50.f, -450.f));
    informacje_o_daniu_1.setOrigin(sf::Vector2f(-50.f, -500.f));

    sf::Text informacje_o_przekasce_0("INFO O PRZEKASCE", font, 20);
    sf::Text informacje_o_przekasce_1("INFO O PRZEKASCE DRUGA LINIJKA", font, 20);
    informacje_o_przekasce_0.setOrigin(sf::Vector2f(-350.f, -50.f));
    informacje_o_przekasce_1.setOrigin(sf::Vector2f(-350.f, -100.f));

    ekran ekran_jedzenia("OBRAZKI/kantyna/tlo.png", { bufet });
    ekran ekran_dan("OBRAZKI/kantyna/lodowka.png", { informacje_o_daniu_0, informacje_o_daniu_1 });
    ekran ekran_slodyczy("OBRAZKI/kantyna/taca.png", { informacje_o_przekasce_0, informacje_o_przekasce_1 });

    przycisk salatka_zaznaczenie("Salatka", rozmiar_przyciskow, 20, kolor_tla_przyciskow, kolor_tekstu_przyciskow, { 350, 200 }, font);
    przycisk truskawka_zaznaczenie("Truskawka", rozmiar_przyciskow, 20, kolor_tla_przyciskow, kolor_tekstu_przyciskow, { 150, 275 }, font);

    produkt truskawka(2, 2, "OBRAZKI/kantyna/truskawka.png");
    produkt salatka(3, 0, "OBRAZKI/kantyna/salatka.png");

    while (okno.isOpen()) {
        sf::Event zdarzenie;

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Return)) { //enter
            login.ustawZaznaczenie(true);
        }
        else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Escape)) {
            if (login.zwrocZaznaczenie())
                login.ustawZaznaczenie(false);
            else if (wyswietl_statystyki)
                wyswietl_statystyki = 0;
            else if (jedzenie_tf)
                jedzenie_tf = 0;
            else if (jemy_dania) {
                jemy_dania = 0;
            }
            else if (jemy_slodycze) {
                jemy_slodycze = 0;
            }
            else
                ;//logika dotyczaca wyjscia z gry
        };

        while (okno.pollEvent(zdarzenie)) { //zwraca true, jezeli jakies zdarzenie oczekuje
            switch (zdarzenie.type)
            {
            ///
            case sf::Event::MouseMoved:
                if (jedzenie_tf) {
                    if (dania.myszanad(okno)) {
                        dania.ustawkolortla(kolor_tla_wcisniete);
                    }
                    else if (desery.myszanad(okno)) {
                        desery.ustawkolortla(kolor_tla_wcisniete);
                    }
                    else if (sklep.myszanad(okno)) {
                        sklep.ustawkolortla(kolor_tla_wcisniete);
                    }
                    else {
                        dania.ustawkolortla(kolor_tla_przyciskow);
                        desery.ustawkolortla(kolor_tla_przyciskow);
                        sklep.ustawkolortla(kolor_tla_przyciskow);
                    };
                }
                else if (jemy_dania) {
                    if (salatka_zaznaczenie.myszanad(okno))
                        salatka_zaznaczenie.ustawkolortla(kolor_tla_wcisniete);
                    else
                        salatka_zaznaczenie.ustawkolortla(kolor_tla_przyciskow);
                }
                else if (jemy_slodycze) {
                    if (truskawka_zaznaczenie.myszanad(okno))
                        truskawka_zaznaczenie.ustawkolortla(kolor_tla_wcisniete);
                    else
                        truskawka_zaznaczenie.ustawkolortla(kolor_tla_przyciskow);
                }
                else if (staty.myszanad(okno)) {
                    staty.ustawkolortla(kolor_tla_wcisniete);
                }
                else if (lodow.myszanad(okno)) {
                    lodow.ustawkolortla(kolor_tla_wcisniete);
                }
                else if (zabaw.myszanad(okno)) {
                    zabaw.ustawkolortla(kolor_tla_wcisniete);
                }
                else if (sprza.myszanad(okno)) {
                    sprza.ustawkolortla(kolor_tla_wcisniete);
                }
                else if (wczyt.myszanad(okno)) {
                    wczyt.ustawkolortla(kolor_tla_wcisniete);
                }
                else if (zapis.myszanad(okno)) {
                    zapis.ustawkolortla(kolor_tla_wcisniete);
                }
                else {
                    staty.ustawkolortla(kolor_tla_przyciskow);
                    lodow.ustawkolortla(kolor_tla_przyciskow);
                    zabaw.ustawkolortla(kolor_tla_przyciskow);
                    sprza.ustawkolortla(kolor_tla_przyciskow);
                    wczyt.ustawkolortla(kolor_tla_przyciskow);
                    zapis.ustawkolortla(kolor_tla_przyciskow);
                };
                break;
            case sf::Event::MouseButtonPressed:
                if (jedzenie_tf) {
                    if (dania.myszanad(okno)) {
                        if(DEBUG) std::cout << "Bedziemy jesc dania glowne" << std::endl;
                        jemy_dania = 1;
                        jedzenie_tf = 0;
                    }
                    else if (desery.myszanad(okno)) {
                        if (DEBUG) std::cout << "Bedziemy jesc desery" << std::endl;
                        jemy_slodycze = 1;
                        jedzenie_tf = 0;
                    }
                    else if (sklep.myszanad(okno)) {
                        if (DEBUG) std::cout << "Bedziemy kupowac jedzenie" << std::endl;
                    }
                    else {
                        dania.ustawkolortla(kolor_tla_przyciskow);
                        desery.ustawkolortla(kolor_tla_przyciskow);
                        sklep.ustawkolortla(kolor_tla_przyciskow);
                    };
                }
                else if (staty.myszanad(okno)) {
                    if (DEBUG) std::cout << "staty przycisniete" << std::endl;

                    std::cout << "Poziom najedzenia zwierzaka " << (*baza_zwierzakow.at(inter.pobierzzalogowany())).zwroc_glod() << std::endl;
                    std::cout << "Poziom szczescia zwierzaka " << (*baza_zwierzakow.at(inter.pobierzzalogowany())).zwroc_szczescie() << std::endl;

                    for (int i = 1; i < 6; i++) {
                        wybor_glodu.push_back((*baza_zwierzakow.at(inter.pobierzzalogowany())).zwroc_glod() < i);
                        wybor_szczescia.push_back((*baza_zwierzakow.at(inter.pobierzzalogowany())).zwroc_szczescie() < i);
                    };

                    wyswietl_statystyki = 1;
                    
                } 
                else if (lodow.myszanad(okno)) {
                    std::cout << "lodow przycisniety" << std::endl;
                    jedzenie_tf = 1;
                }
                else if (zabaw.myszanad(okno)) {
                    std::cout << "zabaw przycisniety" << std::endl;
                }
                else if (sprza.myszanad(okno)) {
                    std::cout << "sprza przycisnieta" << std::endl;
                }
                else if (wczyt.myszanad(okno)) {
                    std::cout << "wczyt przycisniety" << std::endl;
                }
                else if (zapis.myszanad(okno)) {
                    std::cout << "zapis przycisniety" << std::endl;
                };
                break;
            case sf::Event::Closed : {
                okno.close();
                break;
            }
            case sf::Event::Resized : {
                std::cout << "Nowe wymiary to " << zdarzenie.size.width << "x" << zdarzenie.size.height << std::endl;
                break;
            }
            case sf::Event::TextEntered : {
                login.wpisany(zdarzenie);
                break;
             }
            case sf::Event::KeyPressed : {
                if (!zalogowany) {
                    if (zdarzenie.key.code == sf::Keyboard::LShift || zdarzenie.key.code == sf::Keyboard::RShift) { //zatwierdzony
                        if (!mamylogin) {
                            std::cout << "Mamy login: " << login.zwroctekst() << std::endl;
                            nazwa_uzytkownika = login.zwroctekst();
                            if (!std::regex_match(login.zwroctekst(), puste) && !login.zwroctekst().empty()) {
                                if (baza_uzytkownikow.contains(nazwa_uzytkownika))
                                    instrukcja_logowania.setString("Witamy ponownie. Podaj haslo.");
                                else 
                                    instrukcja_logowania.setString("Witamy. Aby zalozyc konto podaj haslo.");
                                mamylogin = 1;
                            }
                            else {
                                instrukcja_logowania.setString("Podano nieprawidlowy login. Sproboj jeszcze raz.");
                            };
                            ekran_logowania.ustaw_napis(0, instrukcja_logowania);
                        }
                        else {
                            if (baza_uzytkownikow.contains(nazwa_uzytkownika)) {
                                //powracajacy uzytkownik
                                std::cout << "Mamy haslo: " << login.zwroctekst() << std::endl;
                                kod = login.zwroctekst();
                                if (kod == baza_uzytkownikow.at(nazwa_uzytkownika).zwroc_haslo()) {
                                    inter.ustawzalogowany(baza_uzytkownikow.at(nazwa_uzytkownika).zwroc_nazwa_uzytkownika());
                                    zalogowany = 1;
                                    if ((*baza_zwierzakow.at(inter.pobierzzalogowany())).zwroc_imie() == "") {
                                        instrukcja_logowania.setString("Nadaj imie swojemu pupilowi!");
                                        ekran_logowania.ustaw_napis(0, instrukcja_logowania);
                                    };

                                    imie.setString("imie: " + (*baza_zwierzakow.at(inter.pobierzzalogowany())).zwroc_imie());
                                    wiek.setString("wiek: " + std::to_string((*baza_zwierzakow.at(inter.pobierzzalogowany())).zwroc_wiek()));
                                    
                                    ekran_statystyk.ustaw_napis(1, imie);
                                    ekran_statystyk.ustaw_napis(2, wiek);
                                }
                                else {
                                    instrukcja_logowania.setString("NIEPOPRAWNE HASLO. Podaj nazwe uzytkownika.");
                                    ekran_logowania.ustaw_napis(0, instrukcja_logowania);
                                    mamylogin = 0;
                                };
                            }
                            else {
                                //nowy uzytkownik
                                std::cout << "Mamy haslo: " << login.zwroctekst() << std::endl;
                                kod = login.zwroctekst();
                                if (!std::regex_match(kod, puste) && !kod.empty()) {
                                    std::cout << "kod niepusty bez bialych znakow" << std::endl;
                                    baza_uzytkownikow[nazwa_uzytkownika] = uzytkownik(nazwa_uzytkownika, kod);
                                    baza_zwierzakow[nazwa_uzytkownika] = new Bobas();
                                    inter.ustawzalogowany(nazwa_uzytkownika);
                                    stworzenie* tmp = baza_zwierzakow.at(inter.pobierzzalogowany());
                                    (*tmp).wczytaj_sprite();

                                    zalogowany = 1;
                                    instrukcja_logowania.setString("Nadaj imie swojemu pupilowi!");
                                    ekran_logowania.ustaw_napis(0, instrukcja_logowania);
                                }
                                else {
                                    std::cout << "W hasle wystepuja niedozwolone znaki!" << std::endl;
                                    instrukcja_logowania.setString("NIEPOPRAWNE HASLO Usun biale znaki i sproboj ponownie.");
                                    ekran_logowania.ustaw_napis(0, instrukcja_logowania);
                                    zalogowany = 0;
                                };
                            };
                        };
                    };
                }
                else if ((*baza_zwierzakow.at(inter.pobierzzalogowany())).zwroc_imie() == "") {
                    if (zdarzenie.key.code == sf::Keyboard::LShift || zdarzenie.key.code == sf::Keyboard::RShift) { //zatwierdzony
                        (*baza_zwierzakow.at(inter.pobierzzalogowany())).ustaw_imie(login.zwroctekst());
                        if (DEBUG) std::cout << "Twoj zwierzak ma na imie " << (*baza_zwierzakow.at(inter.pobierzzalogowany())).zwroc_imie() << std::endl;

                        imie.setString("imie: " + (*baza_zwierzakow.at(inter.pobierzzalogowany())).zwroc_imie());
                        wiek.setString("wiek: " + std::to_string((*baza_zwierzakow.at(inter.pobierzzalogowany())).zwroc_wiek()));

                        ekran_statystyk.ustaw_napis(1, imie);
                        ekran_statystyk.ustaw_napis(2, wiek);
                    };
                };
                break;
            }
            default: {
                break;
            };
            ///
            };
        };

        okno.clear();

        //wyswietlamy rzeczy
        if (!zalogowany) {
            ekran_logowania.rysuj_tlo(okno);
            login.drukuj_do(okno);
        }
        else if ((*baza_zwierzakow.at(inter.pobierzzalogowany())).zwroc_imie() == "") {
            ekran_logowania.rysuj_tlo(okno);
            login.drukuj_do(okno);
        }
        else if (wyswietl_statystyki) {
            ekran_statystyk.rysuj_tlo(okno);

            for (int i = 0; i < 5; i++) {
                okno.draw(glod[i][wybor_glodu[i]]);
                okno.draw(szczescie[i][wybor_szczescia[i]]);
            };
        
        }
        else if (jedzenie_tf) {
            ekran_jedzenia.rysuj_tlo(okno);
            okno.draw(bufet);

            dania.drukujdo(okno);
            desery.drukujdo(okno);
            sklep.drukujdo(okno);
        }
        else if (jemy_dania) {
            ekran_dan.rysuj_tlo(okno);
            salatka.rysuj(okno, sf::Vector2f( - 350.f, -25.f ));
            salatka_zaznaczenie.drukujdo(okno);
        }
        else if (jemy_slodycze) {
            ekran_slodyczy.rysuj_tlo(okno);
            truskawka.rysuj(okno, sf::Vector2f(-150.f, -100.f));
            truskawka_zaznaczenie.drukujdo(okno);
        }
        else {
            static int i = 0;
            ekran_pokoju.rysuj_tlo(okno);

            (*baza_zwierzakow.at(inter.pobierzzalogowany())).idle_animation();
            (* baza_zwierzakow.at(inter.pobierzzalogowany())).drukuj_do(okno);

            staty.drukujdo(okno);
            lodow.drukujdo(okno);
            zabaw.drukujdo(okno);
            sprza.drukujdo(okno);
            wczyt.drukujdo(okno);
            zapis.drukujdo(okno);
        };

        okno.display(); //zrzut z bufora
    };

    std::cout << "Minelo " << czas_od_wlaczenia_programu.asSeconds() << " sekund od uruchomienia programu." << std::endl;
    return 0;
}