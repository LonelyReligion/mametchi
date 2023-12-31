/*********************************************************************
 * @file  interfejs.ixx
 *
 * @brief Implementacja i deklaracja klas uzytkownik, interfejs i ekran.
 *********************************************************************/
#include <iostream>
#include <string>
#include <sstream>
#include <map>
#include <filesystem>
#include <fstream>
#include <typeinfo>

#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>

import pole_tekstowe;
import guzik;
import zwierzak;

bool DEBUG_I = 1;

export module interfejs;

/**
* @class uzytkownik
* @brief Klasa reprezentujaca uzytkownika. 
*/
export class uzytkownik {
private:
	std::string nazwa_uzytkownika;
	std::string haslo;
	int ects = 0;///< waluta

protected:
public:
	uzytkownik() : nazwa_uzytkownika("null"), haslo("null") {};
	uzytkownik(const std::string& nu, const std::string& h, const int & waluta) : nazwa_uzytkownika(nu), haslo(h), ects(waluta) {};
	
	void ustaw_nazwa_uzytkownika(const std::string& nowa_nazwa) {};
	std::string zwroc_nazwa_uzytkownika() { return nazwa_uzytkownika; };

	void ustaw_haslo(const std::string& nowe_haslo) {};
	std::string zwroc_haslo() { return haslo; };

	void dodajects(const int& nagroda)
	{
		ects += nagroda;
	};

	bool usunects(const int& cena) //zwraca false jezeli nie masz wystarczajaco duzo srodkow
	{
		if (ects >= cena) {
			ects -= cena;
			return true;
		}
		else 
			return false;
	};

	int zwrocects() {
		return ects;
	};
};

/**
* @class interfejs
* @brief Klasa reprezentujaca interfejs, zarzadzajaca bazami. 
*/
export class interfejs {
private:
	std::string zalogowany;

	std::map<std::string, uzytkownik> baza_uzytkownikow;
	std::map<std::string, stworzenie*> baza_zwierzakow;

	std::vector<Bobas> bobasy;
	//inne ewolucje beda mialy inne vectory

protected:
public:
	std::string pobierzzalogowany() { return zalogowany; };
	void ustawzalogowany(const std::string & zal) { zalogowany = zal; };

	void dodaj_do_bazy_uzytkownikow(uzytkownik& u) { 
		if (DEBUG_I) std::cout << "dodajemy do bazy uzytkownikow" << std::endl;
		try {
			baza_uzytkownikow.at(u.zwroc_nazwa_uzytkownika());
			std::cout << "OSTRZEZENIE: UZYTKOWNIK O PODANEJ NAZWIE JUZ ISTNIEJE" << std::endl;
			std::cout << "STAN BAZY NIE ULEGNIE ZMIANIE" << std::endl;
		}
		catch (const std::out_of_range& oor) {
			baza_uzytkownikow[u.zwroc_nazwa_uzytkownika()] = u;
		};
	};

	//bool dodaj_do_bazy_zwierzakow(Bobas & bobo) {//dla kazdej klasy-dziecka inna metoda jest konieczna
	//	if (DEBUG_I) std::cout << "dodajemy do bazy zwierzakow" << std::endl;
	//	try {
	//		baza_uzytkownikow.at(bobo.zwroc_imie_rodzica()); //rodzic istnieje
	//	}
	//	catch (const std::out_of_range& oor) {
	//		return false;
	//	};

	//	try {
	//		baza_zwierzakow.at(bobo.zwroc_imie_rodzica());
	//		std::cout << "OSTRZEZENIE: ZWIERZAK O PODANEJ NAZWIE JUZ ISTNIEJE" << std::endl;
	//		std::cout << "STAN BAZY NIE ULEGNIE ZMIANIE" << std::endl;
	//		return false;
	//	}
	//	catch (const std::out_of_range& oor) {
	//		//bobasy.push_back(bobo); ///glFlush()
	//		//baza_zwierzakow[bobo.zwroc_imie_rodzica()] = &bobasy.back();
	//		//bobasy.back().wczytaj_sprite();
	//		return true;
	//	};
	//};

	/**
	* @brief Zapisuje baze uzytkownikow w podanej lokalizacji.
	*
	* @param sciezka pod ktora zapiszemy baze
	*/
	bool zapisz_baze_uzytkownikow(const std::filesystem::path& p){
		std::ofstream os(p);
		os << "nazwa_uzytkownika\t\thaslo\t\tects" << std::endl;
		for (auto u : baza_uzytkownikow) {
			os << u.second.zwroc_nazwa_uzytkownika() << "\t\t" << u.second.zwroc_haslo() << "\t\t" << u.second.zwrocects() << std::endl;
		};
		return true;
	};

	//bool zapisz_baze_zwierzakow(const std::filesystem::path& p){
	//	return true;
	//};

	/**
	* @brief Wczytuje baze uzytkownikow z podanej lokalizacji.
	*
	* @param sciezka, z ktorej wczytujemy baze
	*/
	bool wczytaj_baze_uzytkownikow(const std::filesystem::path & p) {
		if (DEBUG_I) std::cout << "Wczytujemy baze uzytkownikow" << std::endl;
		//sprawdzamy duplikaty
		//ignorujemy puste pola
		//czytamy linijka po linijce
		std::ifstream is(p);
		std::string linijka;
		std::getline(is, linijka);//ignorujemy pierwsza linijke
		while (std::getline(is, linijka))
		{
			std::stringstream ss(linijka);
			std::string nazwa, haslo;
			int waluta;
			if (!(ss >> nazwa >> haslo >> waluta)) {
				if (DEBUG_I) std::cout << "nie wczytalismy uzytkownika" << std::endl;
				break;
			}; // blad: niepelna linijka
			uzytkownik nowy (nazwa, haslo, waluta);
			baza_uzytkownikow[nazwa] = nowy;
			if (DEBUG_I) std::cout << "Wczytalismy uzytkownika o nazwie " << nazwa << std::endl;
		}
		return true;
	};
	
	/**
	* @brief Wczytuje baze zwierzakow z podanej lokalizacji.
	*
	* @param sciezka, z ktorej wczytujemy baze
	* @param baza zawierajaca w sobie wszytskie dostepne dania
	*/
	bool wczytaj_baze_zwierzakow(const std::filesystem::path& p, const std::map<std::string, produkt*>& baza_dan) {
		if (DEBUG_I) std::cout << "Wczytujemy baze zwierzakow" << std::endl;
		std::ifstream is(p);
		std::string linijka;
		std::getline(is, linijka);//ignorujemy pierwsza linijke
		while (std::getline(is, linijka))
		{
			std::stringstream ss(linijka);

			std::string typ, rodzic, imie;
			int glod, szczescie, wiek;
			bool zywy, wyspany;
			std::vector <produkt> dania;
			std::vector <produkt> przekaski;

			if (!(ss >> typ >> rodzic>> imie >> glod >> szczescie >> wiek >> zywy >> wyspany)) {
				if (DEBUG_I) std::cout << "nie wczytalismy zwierzaka" << std::endl;
				break;
			}; // blad: niepelna linijka
			
			char c;
			std::string jedzenie;
			while (ss >> c && c != ';' ) {
				if (c == ',') {
					try {
						dania.push_back(*baza_dan.at(jedzenie));
					}
					catch (const std::out_of_range& oor) {
						if (DEBUG_I) std::cout << "nie wczytalismy jedzenia (danie), bo nie istnieje w bazie" << std::endl;
					};
					jedzenie = "";
				}
				else {
					jedzenie += c;
				};
			};
			jedzenie = "";
			while (ss >> c && c != ';') {
				if (c == ',') {
					try {
						przekaski.push_back(*baza_dan.at(jedzenie));
					}
					catch (const std::out_of_range& oor) {
						if (DEBUG_I) std::cout << "nie wczytalismy jedzenia (slodycz), bo nie istnieje w bazie" << std::endl;
					};
					jedzenie = "";
				}
				else {
					jedzenie += c;
				};
			};

			if (typ == "bobas") {
				Bobas nowy(rodzic, imie, glod, szczescie, wiek, zywy, wyspany, dania, przekaski);
				bobasy.push_back(nowy);
				bobasy.back().wczytaj_sprite();
				baza_zwierzakow[rodzic] = &bobasy.back();
			};

			if (DEBUG_I) std::cout << "Wczytalismy zwierzaka o imieniu  " << imie << std::endl;
		}
		
		return true;
	};

	std::map<std::string, uzytkownik>* zwroc_baze_uzytkownikow() { return &baza_uzytkownikow; };
	std::map<std::string, stworzenie*>* zwroc_baze_zwierzakow() { return &baza_zwierzakow; };

};

/**
* @class ekran
* @brief Klasa reprezentujaca przyciski, tlo i teksty wyswietlane na ekranie. 
*/
export class ekran {
private:
	sf::Sprite duch; ///< sprite tla
	sf::Texture tekstura;
	std::vector<sf::Text> teksty;
	std::vector <przycisk*> guziki;
	bool aktywny;
protected:
public:
	ekran() {};
	ekran(const std::filesystem::path& sciezka, std::vector<sf::Text> t = {}, std::vector<przycisk*> p = {}) :teksty(t), guziki(p) { wczytaj_tlo(sciezka); };
	void wczytaj_tlo(const std::filesystem::path& sciezka) {
		if (!tekstura.loadFromFile(sciezka.string())) {
			std::cout << "ladowanie tekstury tla zakonczone niepowodzeniem" << std::endl;
		};
		tekstura.setSmooth(false);
		duch.setTexture(tekstura);
	};

	void rysuj_tlo(sf::RenderWindow& okno, const sf::Vector2f& pozycja_tla = { 0.f, 0.f }) {
		duch.setOrigin(pozycja_tla);
		okno.draw(duch);
		aktywny = true;
		for (auto& tekst : teksty)
			okno.draw(tekst);
		for (auto& guzik : guziki)
			(*guzik).drukujdo(okno);
	};

	void dodaj_tekst(const sf::Text& tekst) {
		teksty.push_back(tekst);
	};

	std::vector<sf::Text> zwroc_napis() { return teksty; };
	std::vector<przycisk*> zwroc_przyciski() { return guziki; };
	przycisk* zwroc_przycisk(int indeks) { return guziki.at(indeks); };
	void ustaw_napis(const int& indeks, sf::Text& napis) { teksty.at(indeks) = napis; };

	void ustaw_teksty(std::vector<sf::Text> t) { teksty = t; };
	void ustaw_przyciski(std::vector <przycisk*> p) { guziki = p; };

	void wylacz() { aktywny = false; };
	void wlacz() { aktywny = true; };
	bool czy_aktywny() { return aktywny; };
};