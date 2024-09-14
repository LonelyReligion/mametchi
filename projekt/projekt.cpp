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
> (refaktoryzacja) zrobic to samo dla truskawki - traktowac ja jak kazdy slodycz i zapisywac w bazie
> miganie przy budzeniu (bug)
> najezdzanie mysza na dobranoc (kosmetyczne)(latwe)
> logowanie jako metoda klasy interface 
> watki do aktualizowania statystyk (przydatne)(trune)
> wskazowka/instrukcja/strzaleczka z esc (niekonieczne)
> przekazywac i trzymac teksty jako referencje zamiast ustawiania (przydatne)(inwazyjne)
> tabela wynikow w grze, z najlepszymi zwierzakami? (ranges)(wakacje)
> rozwazyc dodanie wagi do zwierzaka i jedzenia(wakacje)
> sprzatanie

> przyciski w grze

> liczba jedzenia kupionego
> dzialanie lodow (bonus: dwa razy szybsza minigra czasowo, moze jakis licznik? daje 2x tyle szczescia co normalnie)
> moze jakies ladniejsze tlo do log ina :3
*/

//*inter.zwroc_baze_uzytkownikow() nazwa uzytkownika, uzytkownik
//*inter.zwroc_baze_zwierzakow() nazwa uzytkownika, wzkaznik na zwierzatko (konieczne do zastosowania polimorfizmu, tak aby wykonywaly sie odpowiednie wersje metod)

bool unikatowa_nazwa_zwierzaka(std::string nazwa, interfejs inter) {
    std::map<std::string, stworzenie*>::iterator it = (*inter.zwroc_baze_zwierzakow()).begin();
    while (it != (*inter.zwroc_baze_zwierzakow()).end()) {
        if (it->second->zwroc_imie() == nazwa) {
            return false;
        }
        it++;
    }
    return true;
}

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
    if (czas_od_poludnia.getElapsedTime().asSeconds() < 10) //230
        prom.set_value(sf::Vector2f(0.015f, 0.01f));
    else {
        stwor.ustaw_wyspany(false);
        prom.set_value(sf::Vector2f(0.f, 0.f));
    };
        
};

bool wczytaj_bazy(interfejs * inter, std::map<std::string, produkt> & baza_dan, std::filesystem::path plik_uzytkownikow, std::filesystem::path plik_zwierzakow) {
    produkt truskawka(2, 2, "OBRAZKI/kantyna/truskawka.png", "truskawka", 0);
    produkt salatka(3, 0, "OBRAZKI/kantyna/salatka.png", "Pyszna salatka", 0);
    produkt lazania(3, 1, "OBRAZKI/kantyna/lazania.png", "Wysmienita lazania", 300);
    produkt lody(0, 1, "OBRAZKI/kantyna/smerf.png", "Smerfastyczne lody", 400);

    if (DEBUG) std::cout << "wczystujemy dania do baz" << std::endl;
    baza_dan["truskawka"] = truskawka;
    baza_dan["salatka"] = salatka;
    baza_dan["lazania"] = lazania;
    baza_dan["lody"] = lody;

    if (!(*inter).wczytaj_baze_uzytkownikow(plik_uzytkownikow)) { //musi byc pierwsza!
        std::cout << "Ladowanie bazy uzytkownikow nie powiodlo sie. Nastapi zakonczenie pracy programu." << std::endl;
        std::cout << "Sprawdz poprawnosc danych w pliku i sprobuj ponownie." << std::endl;
        std::cout << "WSKAZOWKA: Sprawdz czy wszystkie wiersze zawieraja nazwe uzytkownika i haslo. " << std::endl;
        std::cout << "WSKAZOWKA: Sprawdz czy nazwy uzytkownikow sie nie powtarzaja. " << std::endl;
        return 0;
    };

    if (!(*inter).wczytaj_baze_zwierzakow(plik_zwierzakow, baza_dan)) {
        std::cout << "Ladowanie bazy zwierzakow nie powiodlo sie. Nastapi zakonczenie pracy programu." << std::endl;
        std::cout << "Sprawdz poprawnosc danych w pliku i sprobuj ponownie." << std::endl;
        return 0;
    };
    return 1;
}

bool jest_lazania(produkt p) {
    return p.zwroc_nazwa() == "Wysmienita lazania";
}

bool sa_lody(produkt p) {
    return p.zwroc_nazwa() == "Smerfastyczne lody";
}

void zakup_produkt(interfejs & inter, std::map<std::string, produkt> baza_dan, bool czy_danie, sf::Font & font, ekran & sklepu, std::string nazwa_produktu, sf::Text & nowy) {
    std::vector<produkt> v;
    bool mamy_kase = inter.zwroc_baze_uzytkownikow()->at(inter.pobierzzalogowany()).zwrocects() >= baza_dan.at(nazwa_produktu).zwroc_cene();
    bool nie_mamy_produktu;

    if (!czy_danie) {
        v = inter.zwroc_baze_zwierzakow()->at(inter.pobierzzalogowany())->pobierz_przekaski();
        nie_mamy_produktu = std::find_if(v.begin(), v.end(), sa_lody) == v.end(); //cos tu nie dziala - sprawdzic
    }
    else {
        v = inter.zwroc_baze_zwierzakow()->at(inter.pobierzzalogowany())->pobierz_dania();
        nie_mamy_produktu = std::find_if(v.begin(), v.end(), jest_lazania) == v.end();
    }

    if (mamy_kase && nie_mamy_produktu) {
        nowy = sf::Text("Dziekujemy za zakupy\nw Gratce.", font, 35);
        nowy.setOrigin(sf::Vector2f(-40.f, -25.f));
        nowy.setFillColor(sf::Color(195, 239, 150));

        sklepu.ustaw_napis(0, nowy);
        inter.zwroc_baze_uzytkownikow()->at(inter.pobierzzalogowany()).usunects(baza_dan.at(nazwa_produktu).zwroc_cene());
        
        if(czy_danie)
            inter.zwroc_baze_zwierzakow()->at(inter.pobierzzalogowany())->dodaj_danie(baza_dan.at(nazwa_produktu));
        else
            inter.zwroc_baze_zwierzakow()->at(inter.pobierzzalogowany())->dodaj_przekaske(baza_dan.at(nazwa_produktu));
    }
    else if (!mamy_kase) {
        sf::Text nowy = sf::Text("Nie stac cie.\nPrzyjdz do mnie\njak bedziesz\nmiau wiecej\nkasy.", font, 35);
        nowy.setOrigin(sf::Vector2f(-30.f, -25.f));
        nowy.setFillColor(sf::Color(195, 239, 150));
        sklepu.ustaw_napis(0, nowy);
    }
}

int main()
{
    bool pw = false; //co to jest?
    std::filesystem::path plik_uzytkownikow("bazy/baza_uzytkownikow.txt");
    std::filesystem::path plik_zwierzakow("bazy/baza_stworzen.txt");

    sf::Clock czas_od_poludnia; //zrobic zegar i sprawdzac interwaly zamiast mnozyc zegary?
    sf::Clock czas; //_od_wlaczenia_programu;
    sf::Clock budzik;

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
    if(!wczytaj_bazy(&inter, baza_dan, plik_uzytkownikow, plik_zwierzakow))
        return 1; //blad

    //Bobas testowy_bobas("admin1", "portos", 4, 1, 3, 1, 1, { salatka }, { truskawka });
    //inter.dodaj_do_bazy_zwierzakow(testowy_bobas);

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
    ekran ekran_pokoju("obrazki/pokoj.png", {}, { &staty, &lodow, &zabaw, /*&sprza,*/ &wczyt, &zapis});

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
    bool kupujemy = 0;
    bool wychodzimy = 0;

    std::vector<sf::Text> informacje_o_daniu = {sf::Text("", font, 20), sf::Text("", font, 20)};
    informacje_o_daniu[0].setOrigin(sf::Vector2f(-10.f, -475.f));
    informacje_o_daniu[1].setOrigin(sf::Vector2f(-10.f, -500.f));

    sf::Text informacje_o_przekasce = sf::Text("", font, 20);
    informacje_o_przekasce.setOrigin(sf::Vector2f(-300.f, -25.f));

    przycisk dania("Dania", rozmiar_przyciskow, 20, kolor_tla_przyciskow, kolor_tekstu_przyciskow, { 300, 250 }, font);
    przycisk desery("Desery", rozmiar_przyciskow, 20, kolor_tla_przyciskow, kolor_tekstu_przyciskow, { 300, 350 }, font);
    przycisk sklep("Sklep", rozmiar_przyciskow, 20, kolor_tla_przyciskow, kolor_tekstu_przyciskow, { 300, 450 }, font);
    
    sf::Text bufet("Bufet", font, 50);
    bufet.setFillColor(rozowy);
    bufet.setOrigin(sf::Vector2f(-350.f, -150.f));

    ekran ekran_jedzenia("OBRAZKI/kantyna/tlo.png", { bufet }, { &dania, &desery, &sklep });

    przycisk truskawka_zaznaczenie("Truskawka", rozmiar_przyciskow, 20, kolor_tla_przyciskow, kolor_tekstu_przyciskow, { 150, 275 }, font);

    ekran ekran_dan("OBRAZKI/kantyna/lodowka.png", { informacje_o_daniu }, {});
    ekran ekran_slodyczy("OBRAZKI/kantyna/taca.png", { informacje_o_przekasce }, { &truskawka_zaznaczenie });
    
    sf::Text meow = sf::Text("", font, 35);
    meow.setOrigin(sf::Vector2f(-100.f, -25.f));
    przycisk produkt_1("Kup teraz!", { 150, 50 }, 20, sf::Color(137, 222, 116), sf::Color(82, 81, 116), {70, 530}, font);
    przycisk produkt_2("Kup teraz!", { 150, 50 }, 20, sf::Color(137, 222, 116), sf::Color(82, 81, 116), { 310, 530 }, font);
    przycisk produkt_3("Kup teraz!", { 150, 50 }, 20, sf::Color(137, 222, 116), sf::Color(82, 81, 116), { 560, 530 }, font);
    ekran sklepu("OBRAZKI/kantyna/sklep.png", { meow }, {&produkt_1, &produkt_2, &produkt_3 });

    sf::Clock opoznienie;
    std::vector<przycisk> przyciski;

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
                if (zdarzenie.key.code == sf::Keyboard::Escape && !jedzenie_tf && !wyswietl_statystyki && !jemy_slodycze && !jemy_dania && !gramy && !kupujemy)//wychodzimy
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
                else if (kupujemy)
                    kupujemy = 0;
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
                else if (kupujemy) {
                    if (produkt_1.myszanad(okno)) {
                        produkt_1.ustawkolortekstu(sf::Color(73, 54, 81));
                        produkt_1.ustawkolortla(sf::Color(80, 141, 161));

                        sf::Text nowy = sf::Text(baza_dan.at("lazania").zwroc_opis() + "\n" + std::to_string(baza_dan.at("lazania").zwroc_cene()) + "E", font, 35);
                        nowy.setOrigin(sf::Vector2f(-480.f, -20.f));
                        nowy.setFillColor(sf::Color(195, 239, 150));
                        sklepu.ustaw_napis(0, nowy);

                        produkt_2.ustawkolortekstu(sf::Color(82, 81, 116));
                        produkt_2.ustawkolortla(sf::Color(137, 222, 116));

                        produkt_3.ustawkolortekstu(sf::Color(82, 81, 116));
                        produkt_3.ustawkolortla(sf::Color(137, 222, 116));
                    }
                    else if (produkt_2.myszanad(okno)) {
                        produkt_2.ustawkolortekstu(sf::Color(73, 54, 81));
                        produkt_2.ustawkolortla(sf::Color(80, 141, 161));

                        sf::Text nowy = sf::Text(baza_dan.at("lody").zwroc_opis() + "\n" + std::to_string(baza_dan.at("lody").zwroc_cene()) + "E", font, 35);
                        nowy.setOrigin(sf::Vector2f(-480.f, -20.f));
                        nowy.setFillColor(sf::Color(195, 239, 150));
                        sklepu.ustaw_napis(0, nowy);

                        produkt_1.ustawkolortekstu(sf::Color(82, 81, 116));
                        produkt_1.ustawkolortla(sf::Color(137, 222, 116));

                        produkt_3.ustawkolortekstu(sf::Color(82, 81, 116));
                        produkt_3.ustawkolortla(sf::Color(137, 222, 116));
                    }
                    else if (produkt_3.myszanad(okno)) {
                        produkt_3.ustawkolortekstu(sf::Color(73, 54, 81));
                        produkt_3.ustawkolortla(sf::Color(80, 141, 161));

                        produkt_1.ustawkolortekstu(sf::Color(82, 81, 116));
                        produkt_1.ustawkolortla(sf::Color(137, 222, 116));

                        produkt_2.ustawkolortekstu(sf::Color(82, 81, 116));
                        produkt_2.ustawkolortla(sf::Color(137, 222, 116));
                    }
                    else {
                        produkt_1.ustawkolortekstu(sf::Color(82, 81, 116));
                        produkt_1.ustawkolortla(sf::Color(137, 222, 116));

                        if (sklepu.zwroc_napis()[0].getString().toAnsiString() != "Nie stac cie.\nPrzyjdz do mnie\njak bedziesz\nmiau wiecej\nkasy.") {
                            sf::Text nowy = sf::Text("Cos wpadlo ci w oko?", font, 35);
                            nowy.setOrigin(sf::Vector2f(-480.f, -80.f));
                            nowy.setFillColor(sf::Color(195, 239, 150));
                            sklepu.ustaw_napis(0, nowy);
                        }

                        produkt_2.ustawkolortekstu(sf::Color(82, 81, 116));
                        produkt_2.ustawkolortla(sf::Color(137, 222, 116));

                        produkt_3.ustawkolortekstu(sf::Color(82, 81, 116));
                        produkt_3.ustawkolortla(sf::Color(137, 222, 116));
                    }
                   
                }
                else if (jemy_dania) {
                    bool znalezione = false;
                    int i = 0;
                    std::vector<przycisk*> cpy = ekran_dan.zwroc_przyciski();

                    for (produkt p : inter.zwroc_baze_zwierzakow()->at(inter.pobierzzalogowany())->pobierz_dania()) {
                        if ((*ekran_dan.zwroc_przyciski()[i]).myszanad(okno)) {
                            (*cpy[i]).ustawkolortla(kolor_tla_wcisniete);
                            ekran_dan.ustaw_przyciski(cpy);

                            informacje_o_daniu[0].setString(p.zwroc_nazwa());
                            informacje_o_daniu[1].setString(p.zwroc_opis());

                            znalezione = true;
                        }
                        i++;
                    }

                    if (!znalezione) {
                        informacje_o_daniu[0].setString("");
                        informacje_o_daniu[1].setString("");

                        //pobieramy dania zwierzaka, ustawiamy kolor tla przyciskow dan 
                        int i = 0;
                        for (produkt p : inter.zwroc_baze_zwierzakow()->at(inter.pobierzzalogowany())->pobierz_dania()) {
                            (*ekran_dan.zwroc_przyciski()[i]).ustawkolortla(kolor_tla_przyciskow);
                            i++;
                        }
                    }
                    ekran_dan.ustaw_napis(0, informacje_o_daniu[0]);
                    ekran_dan.ustaw_napis(1, informacje_o_daniu[1]);
                }
                else if (jemy_slodycze) {
                    if (truskawka_zaznaczenie.myszanad(okno)) {
                        truskawka_zaznaczenie.ustawkolortla(kolor_tla_wcisniete);
                        informacje_o_przekasce.setString(baza_dan.at("truskawka").zwroc_opis());

                        if (ekran_slodyczy.zwroc_przyciski().size() > 1)
                            (*ekran_slodyczy.zwroc_przycisk(1)).ustawkolortla(kolor_tla_przyciskow);
                    }
                    else if (ekran_slodyczy.zwroc_przyciski().size() > 1 && (*ekran_slodyczy.zwroc_przycisk(1)).myszanad(okno)) {
                        truskawka_zaznaczenie.ustawkolortla(kolor_tla_przyciskow);

                        (*ekran_slodyczy.zwroc_przycisk(1)).ustawkolortla(kolor_tla_wcisniete);
                        informacje_o_przekasce.setString(baza_dan.at("lody").zwroc_opis());
                    }
                    else {
                        truskawka_zaznaczenie.ustawkolortla(kolor_tla_przyciskow);

                        if (ekran_slodyczy.zwroc_przyciski().size() > 1)
                            (*ekran_slodyczy.zwroc_przycisk(1)).ustawkolortla(kolor_tla_przyciskow);
                        informacje_o_przekasce.setString("");
                    };
                    ekran_slodyczy.ustaw_napis(0, informacje_o_przekasce);
                }
                else if (gramy && !zaklad) {
                    if ((*pl.zwroc_przyciski()[0]).myszanad(okno)) {
                        (*pl.zwroc_przyciski()[0]).ustawkolortla(sf::Color(17, 26, 0));
                        (*pl.zwroc_przyciski()[0]).ustawkolortekstu(sf::Color(238, 255, 204));
                    }
                    else if ((*pl.zwroc_przyciski()[1]).myszanad(okno)) {
                        (*pl.zwroc_przyciski()[1]).ustawkolortla(sf::Color(17, 26, 0));
                        (*pl.zwroc_przyciski()[1]).ustawkolortekstu(sf::Color(238, 255, 204));
                    }
                    else {
                        (*pl.zwroc_przyciski()[1]).ustawkolortla(sf::Color(238, 255, 204));
                        (*pl.zwroc_przyciski()[1]).ustawkolortekstu(sf::Color(17, 26, 0));

                        (*pl.zwroc_przyciski()[0]).ustawkolortla(sf::Color(238, 255, 204));
                        (*pl.zwroc_przyciski()[0]).ustawkolortekstu(sf::Color(17, 26, 0));
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
                    (*pl.zwroc_przyciski()[1]).ustawkolortla(sf::Color(238, 255, 204));
                    (*pl.zwroc_przyciski()[1]).ustawkolortekstu(sf::Color(17, 26, 0));

                    (*pl.zwroc_przyciski()[1]).ustawkolortla(sf::Color(238, 255, 204));
                    (*pl.zwroc_przyciski()[0]).ustawkolortekstu(sf::Color(17, 26, 0));
                };
                break;
            case sf::Event::MouseButtonPressed:
                if (wychodzimy) {
                    if (tak.myszanad(okno) && !jedzenie_tf && !wyswietl_statystyki && !jemy_slodycze && !jemy_dania && !gramy) {
                        return 0;
                    }
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
                        kupujemy = 1;
                        jedzenie_tf = 0;
                    }
                    else {
                        dania.ustawkolortla(kolor_tla_przyciskow);
                        desery.ustawkolortla(kolor_tla_przyciskow);
                        sklep.ustawkolortla(kolor_tla_przyciskow);
                    };
                }
                else if (jemy_slodycze && truskawka_zaznaczenie.myszanad(okno) && !wychodzimy) {
                    (*(*inter.zwroc_baze_zwierzakow()).at(inter.pobierzzalogowany())).nakarm(baza_dan.at("truskawka"));
                    if (DEBUG) std::cout << "Nasz zwierzak zjadl truskawke" << std::endl;
                    jemy_slodycze = 0;
                }
                else if (jemy_slodycze && (*ekran_slodyczy.zwroc_przycisk(1)).myszanad(okno) && !wychodzimy) {
                    (*(*inter.zwroc_baze_zwierzakow()).at(inter.pobierzzalogowany())).nakarm(baza_dan.at("lody"));
                    if (DEBUG) std::cout << "Nasz zwierzak zjadl lody" << std::endl;
                    jemy_slodycze = 0;
                }
                else if (jemy_dania && inter.zwroc_baze_zwierzakow()->at(inter.pobierzzalogowany())->zwroc_dania().size() != 0 && !wychodzimy) {
                    for (przycisk* p : ekran_dan.zwroc_przyciski()) {
                        if (p->myszanad(okno)) {
                            std::string nazwa_dania = (p->zwroc_tekst()).substr((p->zwroc_tekst()).find_first_of(" \t") + 1);
                            (*(*inter.zwroc_baze_zwierzakow()).at(inter.pobierzzalogowany())).nakarm(baza_dan.at(nazwa_dania));
                            if (DEBUG) std::cout << "Nasz zwierzak zjadl " + p->zwroc_tekst() << std::endl;
                        }
                    }
                    jemy_dania = 0;
                }
                else if (kupujemy && produkt_1.myszanad(okno) && !wychodzimy) {
                    sf::Text nowy;
                    zakup_produkt(inter, baza_dan, true, font, sklepu, "lazania", nowy);
                }
                else if (kupujemy && produkt_2.myszanad(okno) && !wychodzimy) {
                    sf::Text nowy;
                    zakup_produkt(inter, baza_dan, false, font, sklepu, "lody", nowy);
                }
                else if (kupujemy && produkt_3.myszanad(okno) && !wychodzimy) {
                }
                else if (staty.myszanad(okno) && !wychodzimy && !jemy_slodycze && !wyswietl_statystyki && !jemy_dania && !gramy && !jedzenie_tf) {
                    if (DEBUG) std::cout << "staty przycisniete" << std::endl;
                    wybor_glodu.clear();
                    wybor_szczescia.clear();
                    std::cout << "Poziom najedzenia zwierzaka " << (*(*inter.zwroc_baze_zwierzakow()).at(inter.pobierzzalogowany())).zwroc_glod() << std::endl;
                    std::cout << "Poziom szczescia zwierzaka " << (*(*inter.zwroc_baze_zwierzakow()).at(inter.pobierzzalogowany())).zwroc_szczescie() << std::endl;
                    stworzenie* zal = (*inter.zwroc_baze_zwierzakow()).at(inter.pobierzzalogowany());
                    for (int i = 1; i < 6; i++) {
                        wybor_glodu.push_back((*zal).zwroc_glod() < i);
                        wybor_szczescia.push_back((*zal).zwroc_szczescie() < i);
                    };
                    suma.setString(std::to_string((*inter.zwroc_baze_uzytkownikow())[inter.pobierzzalogowany()].zwrocects()));
                    ekran_statystyk.ustaw_napis(3, suma);
                    wyswietl_statystyki = true;
                } 
                else if (lodow.myszanad(okno) && !wychodzimy && !jemy_slodycze && !wyswietl_statystyki &&  !jemy_dania && !gramy && !jedzenie_tf) {
                    std::cout << "lodow przycisniety" << std::endl;
                    jedzenie_tf = 1;
                }
                else if (zabaw.myszanad(okno) && !wychodzimy && !jemy_slodycze && !wyswietl_statystyki  && !jemy_dania && !gramy && !jedzenie_tf) {
                    std::cout << "zabaw przycisniety" << std::endl;
                    gramy = 1;
                }
                else if (sprza.myszanad(okno) && !wychodzimy && !jemy_slodycze && !wyswietl_statystyki  && !jemy_dania && !gramy && !jedzenie_tf ) {
                    //std::cout << "sprza przycisnieta" << std::endl;
                }
                else if (wczyt.myszanad(okno) && !wychodzimy && !jemy_slodycze && !wyswietl_statystyki  && !jemy_dania && !gramy && !jedzenie_tf) {
                    std::cout << "wczytano bazy" << std::endl;
                    inter.wczytaj_baze_uzytkownikow(plik_uzytkownikow);
                    inter.wczytaj_baze_zwierzakow(plik_zwierzakow, baza_dan);
                    pw = true;

                }
                else if (zapis.myszanad(okno) && !wychodzimy && !jemy_slodycze && !wyswietl_statystyki && !jemy_dania && !gramy && !jedzenie_tf ) {
                    std::cout << "zapisano baze uzytkownikow i zwierzakow" << std::endl;
                    inter.zapisz_baze_uzytkownikow(plik_uzytkownikow);
                    inter.zapisz_baze_zwierzakow(plik_zwierzakow);
                }
                else if (dobranoc.myszanad(okno) && !wychodzimy) {
                    std::cout << "spimy przycisniete" << std::endl;
                    spimy = 1;
                }
                else if (gramy && !zaklad) {
                    opoznienie.restart();
                    if ((*pl.zwroc_przyciski()[0]).myszanad(okno)) {
                        (*inter.zwroc_baze_uzytkownikow())[inter.pobierzzalogowany()].dodajects(pl.zwroc_nagrode(false, (*inter.zwroc_baze_zwierzakow()).at(inter.pobierzzalogowany()))); //int
                        zaklad = 1;
                    }
                    else if ((*pl.zwroc_przyciski()[1]).myszanad(okno)) {
                        (*inter.zwroc_baze_uzytkownikow())[inter.pobierzzalogowany()].dodajects(pl.zwroc_nagrode(true, (*inter.zwroc_baze_zwierzakow()).at(inter.pobierzzalogowany()))); //int
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
                if (!zalogowany && !wychodzimy) { //logowanie
                    if (zdarzenie.key.code == sf::Keyboard::LShift || zdarzenie.key.code == sf::Keyboard::RShift) { //zatwierdzony
                        if (!mamylogin) {
                            std::cout << "Mamy login: " << login.zwroctekst() << std::endl;
                            nazwa_uzytkownika = login.zwroctekst();
                            if (!std::regex_match(login.zwroctekst(), puste) && !login.zwroctekst().empty()) {
                                if ((*inter.zwroc_baze_uzytkownikow()).contains(nazwa_uzytkownika))
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
                            if ((*inter.zwroc_baze_uzytkownikow()).contains(nazwa_uzytkownika)) {
                                //powracajacy uzytkownik
                                std::cout << "Mamy haslo: " << login.zwroctekst() << std::endl;
                                kod = login.zwroctekst();
                                if (kod == (*inter.zwroc_baze_uzytkownikow()).at(nazwa_uzytkownika).zwroc_haslo()) {
                                    inter.ustawzalogowany((*inter.zwroc_baze_uzytkownikow()).at(nazwa_uzytkownika).zwroc_nazwa_uzytkownika());
                                    zalogowany = 1;
                                    try { 
                                        (*inter.zwroc_baze_zwierzakow()).at(inter.pobierzzalogowany());
                                    }
                                    catch(const std::out_of_range& oor) {
                                        instrukcja_logowania.setString("Nadaj imie swojemu pupilowi!");
                                        ekran_logowania.ustaw_napis(0, instrukcja_logowania);
                                        Bobas* bby = new Bobas();
                                        bby->dodaj_danie(baza_dan["salatka"]);
                                        (*bby).ustaw_imie_rodzica(nazwa_uzytkownika);
                                        inter.dodajZwierzaka(bby);
                                    };

                                    
                                    imie.setString("imie: " + (*(*inter.zwroc_baze_zwierzakow()).at(inter.pobierzzalogowany())).zwroc_imie());
                                    wiek.setString("wiek: " + std::to_string((*(*inter.zwroc_baze_zwierzakow()).at(inter.pobierzzalogowany())).zwroc_wiek()));
                                    
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

                                    inter.dodajUzytkownika(uzytkownik(nazwa_uzytkownika, kod, 0));
                                    Bobas* bobo = new Bobas();
                                    (*bobo).ustaw_imie_rodzica(nazwa_uzytkownika);
                                    bobo->dodaj_danie(baza_dan["salatka"]);
                                    inter.dodajZwierzaka(bobo);
                                    inter.ustawzalogowany(nazwa_uzytkownika);

                                    std::map<std::string, stworzenie*> bz = *inter.zwroc_baze_zwierzakow();
                                    stworzenie* tmp = bz.at(inter.pobierzzalogowany());
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
                //blad - naprawiony? 31.07: po powrocie do ekranu logowania bez zapisania zwierzaka i kliknieciu escape jest wyjatek
                else if (zalogowany && ((*inter.zwroc_baze_zwierzakow()).find(inter.pobierzzalogowany()) != (*inter.zwroc_baze_zwierzakow()).end()) && (*(*inter.zwroc_baze_zwierzakow()).at(inter.pobierzzalogowany())).zwroc_imie() == "") {
                    if (zdarzenie.key.code == sf::Keyboard::LShift || zdarzenie.key.code == sf::Keyboard::RShift) { //zatwierdzony
                        if (unikatowa_nazwa_zwierzaka(login.zwroctekst(), inter)) {
                            std::map<std::string, stworzenie*> baza_zwierzakow = *inter.zwroc_baze_zwierzakow();
                            stworzenie* nasze = baza_zwierzakow.at(inter.pobierzzalogowany());
                            (*nasze).ustaw_imie(login.zwroctekst());
                            (*nasze).wczytaj_sprite();
                            if (DEBUG) std::cout << "Twoj zwierzak ma na imie " << (*(*inter.zwroc_baze_zwierzakow()).at(inter.pobierzzalogowany())).zwroc_imie() << std::endl;

                            imie.setString("imie: " + (*(*inter.zwroc_baze_zwierzakow()).at(inter.pobierzzalogowany())).zwroc_imie());
                            wiek.setString("wiek: " + std::to_string((*(*inter.zwroc_baze_zwierzakow()).at(inter.pobierzzalogowany())).zwroc_wiek()));

                            ekran_statystyk.ustaw_napis(1, imie);
                            ekran_statystyk.ustaw_napis(2, wiek);
                        }
                        else {
                            std::cout << "zwierzak o podanej nazwie juz istnieje" << std::endl;
                            instrukcja_logowania.setString("Zwierze o podanej nazwie juz istnieje.");
                            ekran_logowania.ustaw_napis(0, instrukcja_logowania);
                        }
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
        //naprawiony blad - 31.07: wyjatek przy wczytywaniu baz, gdy zwierzak nie zostal do niej nigdy zapisany
        else if (((*inter.zwroc_baze_zwierzakow()).find(inter.pobierzzalogowany()) == (*inter.zwroc_baze_zwierzakow()).end()) || ((*(*inter.zwroc_baze_zwierzakow()).at(inter.pobierzzalogowany())).zwroc_imie() == "")) {
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
            int licznik_1 = 0;
            std::vector<przycisk*> przyciski_dania;
            std::vector<produkt> dania_uzytkownika = inter.zwroc_baze_zwierzakow()->at(inter.pobierzzalogowany())->pobierz_dania();

            //ten licznik jest :/, pewnie zostanie zastapiony tablica jak w pozycji slonca
            for (produkt danie : dania_uzytkownika) {
                if (dania_uzytkownika.size() != ekran_dan.zwroc_przyciski().size()) {
                    przycisk p(danie.zwroc_nazwa(), rozmiar_przyciskow, 20, kolor_tla_przyciskow, kolor_tekstu_przyciskow, { 350.f + 220 * licznik_1, 200 + 20.f * ((licznik_1 - 1) % 2) }, font);
                    przyciski_dania = ekran_dan.zwroc_przyciski();
                    przyciski.push_back(p);
                    przyciski_dania.push_back(&przyciski.back());
                    ekran_dan.ustaw_przyciski(przyciski_dania);
                }
                licznik_1++;
            }
            ekran_dan.rysuj_tlo(okno);

            int licznik = 0;
            for (produkt danie : inter.zwroc_baze_zwierzakow()->at(inter.pobierzzalogowany())->pobierz_dania()) {
                danie.rysuj(okno, sf::Vector2f(-350.f - 200.f * licznik, -25.f - 20.f * (licznik % 2))); //niepoprawne wartosci na razie 
                licznik++;
            }
        }
        else if (jemy_slodycze) {
            int licznik_1 = 1;
            std::vector<przycisk*> przyciski_dania;
            std::vector<produkt> dania_uzytkownika = inter.zwroc_baze_zwierzakow()->at(inter.pobierzzalogowany())->pobierz_przekaski();

            //ten licznik jest :/, pewnie zostanie zastapiony tablica jak w pozycji slonca
            for (produkt danie : dania_uzytkownika) {
                if (dania_uzytkownika.size() + 1 != ekran_slodyczy.zwroc_przyciski().size()) {
                    przycisk p(danie.zwroc_nazwa(), { 300, 50 }, 20, kolor_tla_przyciskow, kolor_tekstu_przyciskow, { 180.f + 220 * licznik_1, 280 + 20.f * ((licznik_1 - 1) % 2) }, font);
                    przyciski_dania = ekran_slodyczy.zwroc_przyciski();
                    przyciski.push_back(p);
                    przyciski_dania.push_back(&przyciski.back());
                    ekran_slodyczy.ustaw_przyciski(przyciski_dania);
                }
                licznik_1++;
            }
            ekran_slodyczy.rysuj_tlo(okno);
            (baza_dan.at("truskawka")).rysuj(okno, sf::Vector2f(-150.f, -100.f));

            int licznik = 1;
            for (produkt danie : inter.zwroc_baze_zwierzakow()->at(inter.pobierzzalogowany())->pobierz_przekaski()) {
                danie.rysuj(okno, sf::Vector2f(-260.f - 200.f * licznik, -80.f - 20.f * (licznik % 2))); //niepoprawne wartosci na razie 
                licznik++;
            }
        }
        else if (kupujemy) {
            sklepu.rysuj_tlo(okno);
            (baza_dan.at("lazania")).rysuj(okno, sf::Vector2f(-50.f, -350.f));
            (baza_dan.at("lody")).rysuj(okno, sf::Vector2f(-290.f, -350.f));
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

            std::thread pozycja_sloneczna(pozycja_slonca, std::move(prom_sloneczne), std::ref(*(*inter.zwroc_baze_zwierzakow()).at(inter.pobierzzalogowany())), std::ref(czas_od_poludnia));

            //static int i = 0;

            if (czas_od_poludnia.getElapsedTime().asSeconds() >= 10)//230
            {
                if (DEBUG) std::cout << (*(*inter.zwroc_baze_zwierzakow()).at(inter.pobierzzalogowany())).zwroc_wyspany() << std::endl;
                if (!(*(*inter.zwroc_baze_zwierzakow()).at(inter.pobierzzalogowany())).zwroc_wyspany()) { //jesli nie wyspany
                    
                    //////////////
                    okno.clear(sf::Color(71, 108, 194));//ok
                    okno.draw(duszek_gwiazd);
                    ekran_pokoju.rysuj_tlo(okno);
                    //////////////

                    if (spimy) {//jezeli guzik zostal wcisniety
                        if (!b) budzik.restart(); //raz na spanie
                        (*(*inter.zwroc_baze_zwierzakow()).at(inter.pobierzzalogowany())).spij(budzik, okno); //w koncu robi sie wyspany
                        b = true;
                    }
                    else {
                        dobranoc.drukujdo(okno); //guzik
                    };
                }
                else { //jezeli wyspany
                    (*(*(*inter.zwroc_baze_zwierzakow()).at(inter.pobierzzalogowany())).zwroc_sprite()).setPosition(sf::Vector2f(0.f, 0.f));
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

            if (raz_po || pw) {
                std::thread pozycja(idle_animation, std::ref(prom), 1);//resetujemy pozycje bobasa
                (*(*inter.zwroc_baze_zwierzakow()).at(inter.pobierzzalogowany())).drukuj_do(okno, fut.get());
                pozycja.join();
                raz_po = false;
                pw = false;
            }
            else if (!spimy) {
                std::thread pozycja(idle_animation, std::ref(prom), 0);
                (*(*inter.zwroc_baze_zwierzakow()).at(inter.pobierzzalogowany())).drukuj_do(okno, fut.get());
                pozycja.join();
            };

            
            pozycja_sloneczna.join();
        };
        if (wychodzimy)
            ekran_popupu.rysuj_tlo(okno, sf::Vector2f(-200.f, -150.f));
        okno.display(); //zrzut z bufora
    };

    std::cout << "Minelo " << czas.getElapsedTime().asSeconds() << " sekund od uruchomienia programu." << std::endl;
    inter.zapisz_baze_uzytkownikow(plik_uzytkownikow);
    return 0;
}