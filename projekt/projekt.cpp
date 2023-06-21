#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>

#include <iostream>
#include <string>
#include <map>
#include <vector>
#include <sstream>

#include <regex> //sprawdzanie logina i hasla
#include <filesystem> //lokalizacja tla, mozliwie pozostalych spriteow
#include <thread> //pozycja bobasa
#include <future>

import zwierzak;
import interfejs;
import jedzenie;
import pole_tekstowe;
import guzik;
import gra;

bool DEBUG = true;

/*
TODO
> miganie przy budzeniu (bug)
> najezdzanie mysza na dobranoc (kosmetyczne)(latwe)
> logowanie jako metoda klasy interface 
> wczytywanie bazy uzytkownikow (konieczne)
> wczytywanie baz zwierzakow (konieczne)
> wczytywanie zwierzakow (konieczne)
> watki do aktualizowania statystyk (przydatne)(trune)
> wskazowka/instrukcja/strzaleczka z esc (niekonieczne)
> przekazywac i trzymac teksty jako referencje zamiast ustawiania (przydatne)(inwazyjne)
> tabela wynikow w grze, z najlepszymi zwierzakami? (ranges)(wakacje)
> rozwazyc dodanie wagi do zwierzaka i jedzenia (wakacje)
> im wiecej wygranych pod rzad tym wieksza wygrana
*/
void idle_animation(std::promise<sf::Vector2f> & prom, bool restart) {
    static std::vector<sf::Vector2f> pobierzpozycjebobasa = { { 3.f, -3.f }, { 3.f, 3.f }, { 3.f, -3.f }, { -3.f, 3.f }, { -3.f, -3.f }, { -3.f, 3.f },
                                                              { -3.f, -3.f }, { -3.f, 3.f }, { -3.f, -3.f }, { 3.f, 3.f }, { 3.f, -3.f }, { 3.f, 3.f } }; //f bo to floaty
    static int i = 0; //iterator
    static int ctr = 0; //liczy do 30

    if (restart)
    {
        i = 0; //iterator
        ctr = 0; //liczy do 30
    };

    if (ctr != 30) {
        ctr += 1;
        prom.set_value(pobierzpozycjebobasa[i]);
    }
    else {
        if (i == pobierzpozycjebobasa.size() - 1) {
            i = 0;
        }
        else {
            i++;
        };
        ctr = 0;
        idle_animation(prom, 0);
    };
};

void pozycja_slonca(std::promise<sf::Vector2f>&& prom, stworzenie & stwor, sf::Clock &czas_od_poludnia) {
    if (czas_od_poludnia.getElapsedTime().asSeconds() < 230) //230
        prom.set_value(sf::Vector2f(0.015f, 0.01f));
    else {
        stwor.ustaw_wyspany(false);
        prom.set_value(sf::Vector2f(0.f, 0.f));
    };
        
};

int main()
{
    sf::Clock czas_od_poludnia; //zrobic zegar i sprawdzac interwaly zamiast mnozyc zegary?
    sf::Clock czas; //_od_wlaczenia_programu;
    sf::Clock budzik;

    //.restart() robi to samo i dodatkowo zeruje zegar, nie dziala poprawnie

    sf::Font font;
    if (!font.loadFromFile("munro.ttf"))
    {
        std::cout << "Ladowanie fonta zakonczone niepowodzeniem" << std::endl;
    };

    std::regex puste(".*\\s+.*"); //cos spacja cos

    interfejs inter;

    sf::Color rozowy(237, 88, 177);
    sf::Color wanilia(252, 234, 154);
    sf::Color pomarancza(247, 182, 101);

    prawo_lewo pl("OBRAZKI/POSTACI/NIEMOWLE_LEWO.png", font);
    bool spimy = 0;
    bool gramy = 0;


    //wczytaj bazy
    std::map<std::string, produkt> baza_dan;
    produkt truskawka(2, 2, "OBRAZKI/kantyna/truskawka.png", "truskawka");
    produkt salatka(3, 0, "OBRAZKI/kantyna/salatka.png", "salatka");
    baza_dan["truskawka"] = truskawka;
    baza_dan["salatka"] = salatka;

    std::map<std::string, uzytkownik> baza_uzytkownikow; //nazwa uzytkownika, uzytkownik
    std::map<std::string, stworzenie *> baza_zwierzakow; //nazwa uzytkownika, wzkaznik na zwierzatko (konieczne do zastosowania polimorfizmu, tak aby wykonywaly sie odpowiednie wersje metod)

    if (!inter.wczytaj_baze_uzytkownikow("bazy/baza_uzytkownikow.txt")) { //musi byc pierwsza!
        std::cout << "Ladowanie bazy uzytkownikow nie powiodlo sie. Nastapi zakonczenie pracy programu." << std::endl;
        std::cout << "Sprawdz poprawnosc danych w pliku i sprobuj ponownie." << std::endl;
        std::cout << "WSKAZOWKA: Sprawdz czy wszystkie wiersze zawieraja nazwe uzytkownika i haslo. " << std::endl;
        std::cout << "WSKAZOWKA: Sprawdz czy nazwy uzytkownikow sie nie powtarzaja. " << std::endl;
        return 0;
    };

    if (!inter.wczytaj_baze_zwierzakow("bazy/baza_stworzen.txt", baza_dan)) {
        std::cout << "Ladowanie bazy zwierzakow nie powiodlo sie. Nastapi zakonczenie pracy programu." << std::endl;
        std::cout << "Sprawdz poprawnosc danych w pliku i sprobuj ponownie." << std::endl;
        return 0;
    };

    baza_uzytkownikow = *(inter.zwroc_baze_uzytkownikow());
    //baza_zwierzakow = *(inter.zwroc_baze_zwierzakow());

    uzytkownik testowy("admin1", "admin1", 0);
    stworzenie testowe();

    baza_uzytkownikow[testowy.zwroc_nazwa_uzytkownika()] = testowy;
    baza_zwierzakow[testowy.zwroc_nazwa_uzytkownika()] = new stworzenie();

    /*The OFL allows the licensed fonts to be used, studied, modified and redistributed freely as
    long as they are not sold by themselves. The fonts, including any derivative works, can be bundled,
    embedded, redistributed and/or sold with any software provided that any reserved names are not used
    by derivative works. The fonts and derivatives, however, cannot be released under any other type of license.
    The requirement for fonts to remain under this license does not apply to any document created using
    the fonts or their derivatives.*/

    stworzenie zwierze;
    
    //ladowanie bobasa, powinno byc zamkniete w klasie, zostawione w mainie na potrzeby testowania
    Bobas bobasek;
    bobasek.wczytaj_sprite();

    //tla //zampknac w interface?
    sf::Color kolor_tla_przyciskow = wanilia;
    sf::Color kolor_tekstu_przyciskow = rozowy;
    sf::Color kolor_tla_wcisniete = pomarancza;
    sf::Vector2f rozmiar_przyciskow = { 200, 50 };

    przycisk dobranoc("DOBRANOC", { 200,50 }, 20, sf::Color(48, 90, 255), sf::Color(250, 233, 135), {300, 400}, font);

    przycisk staty("Statystyki", rozmiar_przyciskow, 20, kolor_tla_przyciskow, kolor_tekstu_przyciskow, { 50, 20 }, font); //wezszy, inne kolory
    przycisk lodow("Bufet", rozmiar_przyciskow, 20, kolor_tla_przyciskow, kolor_tekstu_przyciskow, { 300, 20 }, font);
    przycisk zabaw("Zabawa", rozmiar_przyciskow, 20, kolor_tla_przyciskow, kolor_tekstu_przyciskow, { 550, 20 }, font);
    przycisk sprza("Sprzatanie", rozmiar_przyciskow, 20, kolor_tla_przyciskow, kolor_tekstu_przyciskow, { 550, 500 }, font);
    przycisk wczyt("Wczytaj", rozmiar_przyciskow, 20, kolor_tla_przyciskow, kolor_tekstu_przyciskow, { 300, 500 }, font);
    przycisk zapis("Zapisz", rozmiar_przyciskow, 20, kolor_tla_przyciskow, kolor_tekstu_przyciskow, { 50, 500 }, font);
    ekran ekran_pokoju("obrazki/pokoj.png", {}, { &staty, &lodow, &zabaw, &sprza, &wczyt, &zapis });

    sf::Texture chmury;
    sf::Sprite duszek_chmur;
    if (!chmury.loadFromFile("obrazki/chmury.png")) {
        std::cout << "ladowanie tekstury chmur zakonczone niepowodzeniem" << std::endl;
    };
    chmury.setSmooth(false);
    duszek_chmur.setTexture(chmury);

    sf::Texture gwiazdy;
    sf::Sprite duszek_gwiazd;
    if (!gwiazdy.loadFromFile("obrazki/gwiazdy.png")) {
        std::cout << "ladowanie tekstury gwiazd zakonczone niepowodzeniem" << std::endl;
    };
    gwiazdy.setSmooth(false);
    duszek_gwiazd.setTexture(gwiazdy);

    sf::Texture slonce;
    sf::Sprite duszek_slonca;
    if (!slonce.loadFromFile("obrazki/slonce.png")) {
        std::cout << "ladowanie tekstury slonca zakonczone niepowodzeniem" << std::endl;
    };
    slonce.setSmooth(false);
    duszek_slonca.setTexture(slonce);
    duszek_slonca.setOrigin(sf::Vector2f(-425.f, -100.f));

    sf::Text wyjscie("Czy na pewno chcesz wyjsc z gry? \n Pamietaj, twoje dane nie zostana \nzapisane automatycznie!", font, 20);
    wyjscie.setFillColor(rozowy);
    wyjscie.setOrigin(sf::Vector2f(-250.f, -250.f));

    przycisk tak("TAK", { 100,50 }, 20, kolor_tla_przyciskow, kolor_tekstu_przyciskow, { 300, 350 }, font);
    przycisk nie("NIE", { 100,50 }, 20, kolor_tla_przyciskow, kolor_tekstu_przyciskow, { 425, 350 }, font);
    ekran ekran_popupu("OBRAZKI/popup.png", { wyjscie }, { &tak, &nie });

    /////////////
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
    bool zaklad = 0;

    sf::Text imie("", font, 30);
    imie.setFillColor(sf::Color::White);
    imie.setOrigin(sf::Vector2f(-150.f, -100.f));

    sf::Text wiek("", font, 30);
    wiek.setFillColor(sf::Color::White);
    wiek.setOrigin(sf::Vector2f(-150.f, -150.f));

    sf::Text suma("", font, 30);
    suma.setFillColor(sf::Color::Black);
    suma.setOrigin(sf::Vector2f(-525.f, -525.f));

    sf::Text statystyki("STATYSTYKI", font, 50);
    statystyki.setFillColor(sf::Color::White);
    statystyki.setOrigin(sf::Vector2f(-300.f, 0.f));
    ekran ekran_statystyk("obrazki/statystyki/statystyki.png", {statystyki, imie, wiek, suma});

    sf::Texture mamona;
    if (!mamona.loadFromFile("obrazki/statystyki/ects.png")) {
        std::cout << "ladowanie tekstury pieniazka zakonczone niepowodzeniem" << std::endl;
    };
    mamona.setSmooth(false);
    sf::Sprite s_mamona(mamona);
    s_mamona.setOrigin(sf::Vector2f(-400.f, -500.f));

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
    smutny.setSmooth(false);

    sf::Texture radosny;
    if (!radosny.loadFromFile("obrazki/statystyki/chichrok.png")) {
        std::cout << "ladowanie tekstury ikonka chichrok zakonczone niepowodzeniem" << std::endl;
    };
    radosny.setSmooth(false);

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
    bool wychodzimy = 0;

    std::vector<sf::Text> informacje_o_daniu = {sf::Text("", font, 20), sf::Text("", font, 20)};
    informacje_o_daniu[0].setOrigin(sf::Vector2f(-10.f, -475.f));
    informacje_o_daniu[1].setOrigin(sf::Vector2f(-10.f, -500.f));

    std::vector<sf::Text> informacje_o_przekasce = { sf::Text("", font, 20), sf::Text("", font, 20), sf::Text("", font, 20) };
    informacje_o_przekasce[0].setOrigin(sf::Vector2f(-300.f, -25.f));
    informacje_o_przekasce[1].setOrigin(sf::Vector2f(-300.f, -50.f));
    informacje_o_przekasce[2].setOrigin(sf::Vector2f(-300.f, -75.f));

    przycisk dania("Dania", rozmiar_przyciskow, 20, kolor_tla_przyciskow, kolor_tekstu_przyciskow, { 300, 250 }, font);
    przycisk desery("Desery", rozmiar_przyciskow, 20, kolor_tla_przyciskow, kolor_tekstu_przyciskow, { 300, 350 }, font);
    przycisk sklep("Sklep", rozmiar_przyciskow, 20, kolor_tla_przyciskow, kolor_tekstu_przyciskow, { 300, 450 }, font);
    
    sf::Text bufet("Bufet", font, 50);
    bufet.setFillColor(rozowy);
    bufet.setOrigin(sf::Vector2f(-350.f, -150.f));

    ekran ekran_jedzenia("OBRAZKI/kantyna/tlo.png", { bufet }, { &dania, &desery, &sklep });

    przycisk salatka_zaznaczenie("Salatka", rozmiar_przyciskow, 20, kolor_tla_przyciskow, kolor_tekstu_przyciskow, { 350, 200 }, font);
    przycisk truskawka_zaznaczenie("Truskawka", rozmiar_przyciskow, 20, kolor_tla_przyciskow, kolor_tekstu_przyciskow, { 150, 275 }, font);

    ekran ekran_dan("OBRAZKI/kantyna/lodowka.png", { informacje_o_daniu[0], informacje_o_daniu[1] }, {&salatka_zaznaczenie });
    ekran ekran_slodyczy("OBRAZKI/kantyna/taca.png", { informacje_o_przekasce[0], informacje_o_przekasce[1], informacje_o_przekasce[2] }, { &truskawka_zaznaczenie });

    sf::Clock opoznienie;

    while (okno.isOpen()) {
        sf::Event zdarzenie;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Return) && !wychodzimy) { //enter
            login.ustawZaznaczenie(true);
        }
        else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Escape)) {
            if (login.zwrocZaznaczenie())
                login.ustawZaznaczenie(false);
        };
        while (okno.pollEvent(zdarzenie)) { //zwraca true, jezeli jakies zdarzenie oczekuje
            switch (zdarzenie.type)
            {
            ///
            case sf::Event::KeyReleased:
                if (zdarzenie.key.code == sf::Keyboard::Escape && !jedzenie_tf && !wyswietl_statystyki && !jemy_slodycze && !jemy_dania && !gramy)//wychodzimy
                {
                    if (DEBUG) std::cout << "wywolano sekwencje wyjscia" << std::endl;
                    wychodzimy = !wychodzimy;
                }
                else if (wyswietl_statystyki)
                    wyswietl_statystyki = 0;
                else if (jedzenie_tf)
                    jedzenie_tf = 0;
                else if (jemy_dania)
                    jemy_dania = 0;
                else if (jemy_slodycze)
                    jemy_slodycze = 0;
                else if (gramy) {
                    gramy = 0;
                    zaklad = 0;
                    opoznienie.restart();
                    pl.wczytaj_sprite("OBRAZKI/POSTACI/NIEMOWLE_LEWO.png");
                };
            case sf::Event::MouseMoved:
                if (wychodzimy) {
                    if (tak.myszanad(okno))
                        tak.ustawkolortla(kolor_tla_wcisniete);
                    else if (nie.myszanad(okno))
                        nie.ustawkolortla(kolor_tla_wcisniete);
                    else {
                        nie.ustawkolortla(kolor_tla_przyciskow);
                        tak.ustawkolortla(kolor_tla_przyciskow);
                    }
                }
                else if (jedzenie_tf) {
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
                    if (salatka_zaznaczenie.myszanad(okno)) {
                        salatka_zaznaczenie.ustawkolortla(kolor_tla_wcisniete);
                        informacje_o_daniu[0].setString("Salatka");
                        informacje_o_daniu[1].setString("Wartosc odzywcza: " + std::to_string(salatka.zwroc_wo()));
                    }
                    else {
                        salatka_zaznaczenie.ustawkolortla(kolor_tla_przyciskow);
                        informacje_o_daniu[0].setString("");
                        informacje_o_daniu[1].setString("");
                    };
                    ekran_dan.ustaw_napis(0, informacje_o_daniu[0]);
                    ekran_dan.ustaw_napis(1, informacje_o_daniu[1]);
                }
                else if (jemy_slodycze) {
                    if (truskawka_zaznaczenie.myszanad(okno)) {
                        truskawka_zaznaczenie.ustawkolortla(kolor_tla_wcisniete);
                        informacje_o_przekasce[0].setString("Truskawka");
                        if (DEBUG) std::cout << "Wartosc odzywcza truskawki: " << truskawka.zwroc_wo() << std::endl;
                        informacje_o_przekasce[1].setString("Wartosc odzywcza: " + std::to_string(truskawka.zwroc_wo()));
                        informacje_o_przekasce[2].setString("Szczescie: " + std::to_string(truskawka.zwroc_r()));
                    }
                    else {
                        truskawka_zaznaczenie.ustawkolortla(kolor_tla_przyciskow);
                        informacje_o_przekasce[0].setString("");
                        informacje_o_przekasce[1].setString("");
                        informacje_o_przekasce[2].setString("");
                    };
                    ekran_slodyczy.ustaw_napis(0, informacje_o_przekasce[0]);
                    ekran_slodyczy.ustaw_napis(1, informacje_o_przekasce[1]);
                    ekran_slodyczy.ustaw_napis(2, informacje_o_przekasce[2]);
                }
                else if (gramy && !zaklad) {
                    if ((*pl.zwroc_przyciski()[0]).myszanad(okno)) {
                        (*pl.zwroc_przyciski()[0]).ustawkolortla(sf::Color(238, 255, 204));
                    }
                    else if ((*pl.zwroc_przyciski()[1]).myszanad(okno)) {
                        (*pl.zwroc_przyciski()[1]).ustawkolortla(sf::Color(238, 255, 204));
                    };
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
                    (*pl.zwroc_przyciski()[1]).ustawkolortla(sf::Color(17, 26, 0));
                    (*pl.zwroc_przyciski()[0]).ustawkolortla(sf::Color(17, 26, 0));
                };
                break;
            case sf::Event::MouseButtonPressed:
                if (wychodzimy) {
                    if (tak.myszanad(okno) && !jedzenie_tf && !wyswietl_statystyki && !jemy_slodycze && !jemy_dania && !gramy)
                        return 0;
                    else if (nie.myszanad(okno) && !jedzenie_tf && !wyswietl_statystyki && !jemy_slodycze && !jemy_dania && !gramy)
                        wychodzimy = 0;
                }
                else if (jedzenie_tf) {
                    if (dania.myszanad(okno) && !wychodzimy && !wyswietl_statystyki && !gramy) {
                        if(DEBUG) std::cout << "Bedziemy jesc dania glowne" << std::endl;
                        jemy_dania = 1;
                        jedzenie_tf = 0;
                    }
                    else if (desery.myszanad(okno) && !wychodzimy && !wyswietl_statystyki && !gramy) {
                        if (DEBUG) std::cout << "Bedziemy jesc desery" << std::endl;
                        jemy_slodycze = 1;
                        jedzenie_tf = 0;
                    }
                    else if (sklep.myszanad(okno) && !wychodzimy && !wyswietl_statystyki && !gramy) {
                        if (DEBUG) std::cout << "Bedziemy kupowac jedzenie" << std::endl;
                    }
                    else {
                        dania.ustawkolortla(kolor_tla_przyciskow);
                        desery.ustawkolortla(kolor_tla_przyciskow);
                        sklep.ustawkolortla(kolor_tla_przyciskow);
                    };
                }
                else if (jemy_slodycze && truskawka_zaznaczenie.myszanad(okno) && !wychodzimy) {
                    (*baza_zwierzakow.at(inter.pobierzzalogowany())).nakarm(truskawka);
                    if (DEBUG) std::cout << "Nasz zwierzak zjadl truskawke" << std::endl;
                    jemy_slodycze = 0;
                }
                else if (jemy_dania && salatka_zaznaczenie.myszanad(okno) && !wychodzimy) {
                    (*baza_zwierzakow.at(inter.pobierzzalogowany())).nakarm(salatka);
                    if (DEBUG) std::cout << "Nasz zwierzak zjadl salatke" << std::endl;
                    jemy_dania = 0;
                }
                else if (staty.myszanad(okno) && !wychodzimy && !jemy_slodycze && !wyswietl_statystyki && !jemy_slodycze && !jemy_dania && !gramy && !jedzenie_tf && !jemy_dania) {
                    if (DEBUG) std::cout << "staty przycisniete" << std::endl;
                    wybor_glodu.clear();
                    wybor_szczescia.clear();
                    std::cout << "Poziom najedzenia zwierzaka " << (*baza_zwierzakow.at(inter.pobierzzalogowany())).zwroc_glod() << std::endl;
                    std::cout << "Poziom szczescia zwierzaka " << (*baza_zwierzakow.at(inter.pobierzzalogowany())).zwroc_szczescie() << std::endl;
                    stworzenie* zal = baza_zwierzakow.at(inter.pobierzzalogowany());
                    for (int i = 1; i < 6; i++) {
                        wybor_glodu.push_back((*zal).zwroc_glod() < i);
                        wybor_szczescia.push_back((*zal).zwroc_szczescie() < i);
                    };
                    suma.setString(std::to_string(baza_uzytkownikow[inter.pobierzzalogowany()].zwrocects()));
                    ekran_statystyk.ustaw_napis(3, suma);
                    wyswietl_statystyki = true;
                } 
                else if (lodow.myszanad(okno) && !wychodzimy && !jemy_slodycze && !wyswietl_statystyki && !jemy_slodycze && !jemy_dania && !gramy && !jedzenie_tf && !jemy_dania) {
                    std::cout << "lodow przycisniety" << std::endl;
                    jedzenie_tf = 1;
                }
                else if (zabaw.myszanad(okno) && !wychodzimy && !jemy_slodycze && !wyswietl_statystyki && !jemy_slodycze && !jemy_dania && !gramy && !jedzenie_tf && !jemy_dania) {
                    std::cout << "zabaw przycisniety" << std::endl;
                    gramy = 1;
                }
                else if (sprza.myszanad(okno) && !wychodzimy && !jemy_slodycze && !wyswietl_statystyki && !jemy_slodycze && !jemy_dania && !gramy && !jedzenie_tf && !jemy_dania) {
                    std::cout << "sprza przycisnieta" << std::endl;
                }
                else if (wczyt.myszanad(okno) && !wychodzimy && !jemy_slodycze && !wyswietl_statystyki && !jemy_slodycze && !jemy_dania && !gramy && !jedzenie_tf && !jemy_dania) {
                    std::cout << "wczyt przycisniety" << std::endl;
                }
                else if (zapis.myszanad(okno) && !wychodzimy && !jemy_slodycze && !wyswietl_statystyki && !jemy_slodycze && !jemy_dania && !gramy && !jedzenie_tf && !jemy_dania) {
                    std::cout << "zapis przycisniety" << std::endl;
                }
                else if (dobranoc.myszanad(okno) && !wychodzimy) {
                    std::cout << "spimy przycisniete" << std::endl;
                    spimy = 1;
                }
                else if (gramy && !zaklad) {
                    opoznienie.restart();
                    if ((*pl.zwroc_przyciski()[0]).myszanad(okno)) {
                        baza_uzytkownikow[inter.pobierzzalogowany()].dodajects(pl.zwroc_nagrode(false, baza_zwierzakow[inter.pobierzzalogowany()])); //int
                        zaklad = 1;
                    }
                    else if ((*pl.zwroc_przyciski()[1]).myszanad(okno)) {
                        baza_uzytkownikow[inter.pobierzzalogowany()].dodajects(pl.zwroc_nagrode(true, baza_zwierzakow[inter.pobierzzalogowany()])); //int
                        zaklad = 1;
                    };
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
                if (!zalogowany && !wychodzimy) {
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
                                    try { 
                                        (*baza_zwierzakow.at(inter.pobierzzalogowany()));
                                    }
                                    catch(const std::out_of_range& oor) {
                                        instrukcja_logowania.setString("Nadaj imie swojemu pupilowi!");
                                        ekran_logowania.ustaw_napis(0, instrukcja_logowania);
                                        baza_zwierzakow[inter.pobierzzalogowany()] = new Bobas();
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
                                    baza_uzytkownikow[nazwa_uzytkownika] = uzytkownik(nazwa_uzytkownika, kod, 0);
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
                else if (zalogowany && (*baza_zwierzakow.at(inter.pobierzzalogowany())).zwroc_imie() == "") {
                    if (zdarzenie.key.code == sf::Keyboard::LShift || zdarzenie.key.code == sf::Keyboard::RShift) { //zatwierdzony
                        (*baza_zwierzakow.at(inter.pobierzzalogowany())).ustaw_imie(login.zwroctekst());
                        if (DEBUG) std::cout << "Twoj zwierzak ma na imie " << (*baza_zwierzakow.at(inter.pobierzzalogowany())).zwroc_imie() << std::endl;

                        imie.setString("imie: " + (*baza_zwierzakow.at(inter.pobierzzalogowany())).zwroc_imie());
                        wiek.setString("wiek: " + std::to_string((*baza_zwierzakow.at(inter.pobierzzalogowany())).zwroc_wiek()));

                        ekran_statystyk.ustaw_napis(1, imie);
                        ekran_statystyk.ustaw_napis(2, wiek);
                    };
                };
                czas_od_poludnia.restart();
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
            okno.draw(s_mamona);
            for (int i = 0; i < 5; i++) {
                bool g = wybor_glodu[i];
                bool s = wybor_szczescia[i];
                okno.draw(glod[i][g]);
                okno.draw(szczescie[i][s]);
            };
        
        }
        else if (jedzenie_tf) {
            ekran_jedzenia.rysuj_tlo(okno);
            okno.draw(bufet);
        }
        else if (jemy_dania) {
            ekran_dan.rysuj_tlo(okno);
            salatka.rysuj(okno, sf::Vector2f( - 350.f, -25.f ));
        }
        else if (jemy_slodycze) {
            ekran_slodyczy.rysuj_tlo(okno);
            truskawka.rysuj(okno, sf::Vector2f(-150.f, -100.f));
        }
        else if (gramy) {
            pl.rysuj(okno);  
            if (zaklad && opoznienie.getElapsedTime().asSeconds() >= 5) {
                gramy = 0;
                zaklad = 0;
                opoznienie.restart();
                pl.wczytaj_sprite("OBRAZKI/POSTACI/NIEMOWLE_LEWO.png");
            };
        }
        else {
            static bool b = false;
            static bool raz_po = false;

            std::promise<sf::Vector2f> prom;
            std::future<sf::Vector2f> fut = prom.get_future();

            std::promise<sf::Vector2f> prom_sloneczne;
            std::future<sf::Vector2f> fut_sloneczne = prom_sloneczne.get_future();

            std::thread pozycja_sloneczna(pozycja_slonca, std::move(prom_sloneczne), std::ref(*baza_zwierzakow.at(inter.pobierzzalogowany())), std::ref(czas_od_poludnia));

            //static int i = 0;

            if (czas_od_poludnia.getElapsedTime().asSeconds() >= 230)//230
            {
                if (DEBUG) std::cout << (*baza_zwierzakow.at(inter.pobierzzalogowany())).zwroc_wyspany() << std::endl;
                if (!(*baza_zwierzakow.at(inter.pobierzzalogowany())).zwroc_wyspany()) { //jesli nie wyspany
                    
                    //////////////
                    okno.clear(sf::Color(71, 108, 194));//ok
                    okno.draw(duszek_gwiazd);
                    ekran_pokoju.rysuj_tlo(okno);
                    //////////////

                    if (spimy) {//jezeli guzik zostal wcisniety
                        if (!b) budzik.restart(); //raz na spanie
                        (*baza_zwierzakow.at(inter.pobierzzalogowany())).spij(budzik, okno); //w koncu robi sie wyspany
                        b = true;
                    }
                    else {
                        dobranoc.drukujdo(okno); //guzik
                    };
                }
                else { //jezeli wyspany
                    (*(*baza_zwierzakow.at(inter.pobierzzalogowany())).zwroc_sprite()).setPosition(sf::Vector2f(0.f, 0.f));
                    czas_od_poludnia.restart();
                    raz_po = 1;
                }
                //zmieniamy niebo i wyswietlamy przycisk, gdy przycisk animacja spania, reset zegara, reset nieba
            }
            else {
                okno.clear(sf::Color(186, 240, 255)); //ok
                okno.draw(duszek_slonca);
                duszek_slonca.move(fut_sloneczne.get());
                okno.draw(duszek_chmur);
                ekran_pokoju.rysuj_tlo(okno);
                spimy = 0;
                b = false;
            };

            if (raz_po) {
                std::thread pozycja(idle_animation, std::ref(prom), 1);//resetujemy pozycje bobasa
                (*baza_zwierzakow.at(inter.pobierzzalogowany())).drukuj_do(okno, fut.get());
                pozycja.join();
                raz_po = false;
            }
            else if (!spimy) {
                std::thread pozycja(idle_animation, std::ref(prom), 0);
                (*baza_zwierzakow.at(inter.pobierzzalogowany())).drukuj_do(okno, fut.get());
                pozycja.join();
            };

            
            pozycja_sloneczna.join();
        };
        if (wychodzimy)
            ekran_popupu.rysuj_tlo(okno, sf::Vector2f(-200.f, -150.f));
        okno.display(); //zrzut z bufora
    };

    std::cout << "Minelo " << czas.getElapsedTime().asSeconds() << " sekund od uruchomienia programu." << std::endl;
    return 0;
}