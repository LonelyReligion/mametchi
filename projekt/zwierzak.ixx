#include <iostream>
#include <string>
#include <filesystem> //do wczytywania spriteu bobasa?
#include <map>

#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>

import jedzenie;

bool DEBUG_Z = true;

import interfejs;

export module zwierzak;

export class stworzenie {
private:
	std::string imie_rodzica;
	std::string imie;

	int glod; //0(glodny) - 5(nie glodny)
	int szczescie; //0 - 5
	int wiek;

	bool chory = 0;
	bool glodny = 0;
	bool zmeczony = 0;
	bool smutny = 0;

	bool zywy = 1;

	std::map <produkt, int> dania;
	std::map <produkt, int> przekaski;
protected:
public:
	stworzenie() : glod(1), szczescie(0), chory(0), glodny(1), zmeczony(0), smutny(1), imie(""), wiek(0), zywy(1) { if (DEBUG_Z) std::cout << "wywolano konstruktor bezargumentowy klasy stworzenie" << std::endl; };
	virtual void wczytaj_sprite() { if (DEBUG_Z) std::cout << "Wczytuje sprite dla klasy stworzenie" << std::endl; };
	virtual void idle_animation() { if( DEBUG_Z ) std::cout << "Wyswietlam animacje petli dla klasy stworzenie" << std::endl; };
	virtual void drukuj_do(sf::RenderWindow& okno, sf::Vector2f delta) {};

	std::string zwroc_imie() { return imie; };
	void ustaw_imie(const std::string& miano) { imie = miano; };

	int zwroc_wiek() { return wiek; };
	void ustaw_wiek(const int& lata) { wiek = lata; };

	int zwroc_glod() { return glod; };
	void ustaw_glod(const int& am) { glod = am; };

	int zwroc_szczescie() { return szczescie; };
	void ustaw_szczescie(const int& radowanie) { szczescie = radowanie; };

	void nakarm(const produkt& jedzenie) {
		if (DEBUG_Z) std::cout << "Jemy" << std::endl;
		if (!chory) {
			if ((glod + jedzenie.zwroc_wo()) < 5) {
				glod = glod + jedzenie.zwroc_wo();
				if (DEBUG_Z) std::cout << zwroc_glod() << std::endl;
			}
			else {
				glod = 5;
			};

			if (jedzenie.zwroc_r()) { //mamy do czynienia ze slodyczem
				if ((szczescie + jedzenie.zwroc_r()) < 5)
					szczescie = szczescie + jedzenie.zwroc_r();
				else {
					szczescie = 5;
				};
			};
		};
	}; //zwraca informacje o tym czy sie powiodla akcja
	bool baw() { return true; }; //zwraca informacje o tym czy sie powiodla akcja

	friend interfejs;
};

export class Bobas : public stworzenie {
private:
	sf::Sprite duszek_bobasa;
	sf::Texture bobas; //musimy przechowywac i teksture i sprite'a poniewaz sprite przechowuje tylko pointer do tekstury
protected:
public:
	virtual void wczytaj_sprite() { 
		if (DEBUG_Z) std::cout << "Wczytuje sprite dla klasy bobas" << std::endl; 
		sf::Vector2f pozycja_bobasa(-300.f, -250.f);
		if (!bobas.loadFromFile("obrazki/postaci/niemowle.png")) {
			std::cout << "ladowanie tekstury bobasa zakonczone niepowodzeniem" << std::endl;
		};
		bobas.setSmooth(false);
		duszek_bobasa.setTexture(bobas);

		if (DEBUG_Z) std::cout << "Wczytuje pozycje poczatkowa bobasa" << std::endl;
		duszek_bobasa.setOrigin(pozycja_bobasa); //x, y (0,0) jest w lewym gornym rogu
	};

	void drukuj_do(sf::RenderWindow& okno, sf::Vector2f delta) {
		duszek_bobasa.move(delta);
		okno.draw(duszek_bobasa);
	};
};