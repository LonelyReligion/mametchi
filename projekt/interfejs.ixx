#include <iostream>
#include <string>
//#include <cstring>
#include <map>
#include <filesystem>
#include <fstream>

#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>

import pole_tekstowe;
import guzik;
import zwierzak;

export module interfejs;

export class uzytkownik {
private:
	std::string nazwa_uzytkownika;
	std::string haslo;
	int ects = 0;//waluta

protected:
public:
	uzytkownik() : nazwa_uzytkownika("null"), haslo("null") {};
	uzytkownik(const std::string& nu, const std::string& h) : nazwa_uzytkownika(nu), haslo(h) {};
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

	std::vector<stworzenie> stworzenia;
	std::vector<Bobas> bobasy;

protected:
public:
	std::string pobierzzalogowany() { return zalogowany; };
	void ustawzalogowany(const std::string & zal) { zalogowany = zal; };

	bool wczytaj_baze_uzytkownikow(const std::filesystem::path & p) {
		//sprawdzamy duplikaty
		//ignorujemy puste pola
		//czytamy linijka po linijce
		std::ifstream is(p);
		return true;
	};
	
	bool wczytaj_baze_zwierzakow(const std::filesystem::path& p) {
		//do zaimplementowania
		std::ifstream is(p);
		return true;
	};

	bool wczytaj_baze_jedzenia(const std::filesystem::path& p) {
		//do zaimplementowania
		std::ifstream is(p);
		return true;
	};
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