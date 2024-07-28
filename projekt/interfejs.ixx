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
import jedzenie;

bool DEBUG_I = 1;

export module interfejs;

export class uzytkownik {
private:
	std::string nazwa_uzytkownika;
	std::string haslo;
	int ects = 0;//waluta

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

	bool dodaj_do_bazy_zwierzakow(Bobas & bobo) {//dla kazdej klasy-dziecka inna metoda jest konieczna
		if (DEBUG_I) std::cout << "dodajemy do bazy zwierzakow" << std::endl;
		try {
			baza_uzytkownikow.at(bobo.zwroc_imie_rodzica()); ///rodzic istnieje
		}
		catch (const std::out_of_range& oor) {
			return false;
		};

		try {
			baza_zwierzakow.at(bobo.zwroc_imie_rodzica());
			std::cout << "OSTRZEZENIE: ZWIERZAK O PODANEJ NAZWIE JUZ ISTNIEJE" << std::endl;
			std::cout << "STAN BAZY NIE ULEGNIE ZMIANIE" << std::endl;
			return false;
		}
		catch (const std::out_of_range& oor) {
			//bobasy.push_back(bobo); ///glFlush()
			//baza_zwierzakow[bobo.zwroc_imie_rodzica()] = &bobasy.back();
			//bobasy.back().wczytaj_sprite();
			return true;
		};
	};

	bool zapisz_baze_uzytkownikow(const std::filesystem::path& p){
		std::ofstream os(p, std::ofstream::out);
		os << "nazwa_uzytkownika\t\thaslo\t\tects" << std::endl;
		for (auto & u : baza_uzytkownikow) {
			os << u.second.zwroc_nazwa_uzytkownika() << "\t\t" << u.second.zwroc_haslo() << "\t\t" << u.second.zwrocects() << std::endl;
		};
		return true;
	};

	bool zapisz_baze_zwierzakow(const std::filesystem::path& p){
		std::ofstream os(p, std::ofstream::out);
		os << "typ\timie_rodzica\timie\tglod\tszczescie\twiek\tzywy\twyspany\tdania\tprzekaski" << std::endl;
		for (auto& z : baza_zwierzakow) {
			std::string typ;
			if (dynamic_cast<const Bobas*>(z.second) != nullptr)
				typ = "bobas";
			os << typ << "\t" << z.second->zwroc_imie_rodzica() << "\t" 
				<< z.second->zwroc_imie() << "\t" << z.second->zwroc_glod() << "\t" 
				<< z.second->zwroc_szczescie() << "\t" << z.second->zwroc_wiek() << "\t"
				<< z.second->zwroc_zywy() << "\t" << z.second->zwroc_wyspany() << "\t";
			for (auto danie : z.second->zwroc_dania()) {
				if(z.second->zwroc_dania().back().zwroc_nazwa() != danie.zwroc_nazwa())
					os << danie.zwroc_nazwa() << ",";
				else
					os << danie.zwroc_nazwa() << ";";
			}
			for (auto przekaska : z.second->zwroc_przekaski()) {
				if (z.second->zwroc_przekaski().back().zwroc_nazwa() != przekaska.zwroc_nazwa())
					os << przekaska.zwroc_nazwa() << ",";
				else
					os << przekaska.zwroc_nazwa() << ";";
			}
		};
		return true;
	};

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
	
	bool wczytaj_baze_zwierzakow(const std::filesystem::path& p, const std::map<std::string, produkt*>& baza_dan) {
		if (DEBUG_I) std::cout << "Wczytujemy baze zwierzakow" << std::endl;
		baza_zwierzakow.clear();
		bobasy.clear();

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
	void dodajUzytkownika(uzytkownik nowy) { baza_uzytkownikow[nowy.zwroc_nazwa_uzytkownika()] = nowy; };
	void dodajZwierzaka(stworzenie* nowe) { baza_zwierzakow[nowe->zwroc_imie_rodzica()] = nowe; }
};

export class ekran {
private:
	sf::Sprite duch;
	sf::Texture tekstura;
	std::vector<sf::Text> teksty;
	std::vector <przycisk*> guziki;
protected:
public:
	ekran() {};
	ekran(const std::filesystem::path& sciezka, std::vector<sf::Text> t = {}, std::vector<przycisk*> p = {}) :teksty(t), guziki(p) { wczytaj_tlo(sciezka); };
	void wczytaj_tlo(const std::filesystem::path & sciezka){
		if (!tekstura.loadFromFile(sciezka.string())){
			std::cout << "ladowanie tekstury tla zakonczone niepowodzeniem" << std::endl;
		};
		tekstura.setSmooth(false);
		duch.setTexture(tekstura);
	};

	void rysuj_tlo(sf::RenderWindow& okno, const sf::Vector2f& pozycja_tla = {0.f, 0.f}) {
		duch.setOrigin(pozycja_tla);
		okno.draw(duch);
		for(auto & tekst : teksty)
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
};