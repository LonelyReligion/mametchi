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
	bool nakarm(const produkt & jedzenie) { 
		if (!chory) {
			if ((5 - jedzenie.zwroc_wo()) > glod)
				glodny = glod + jedzenie.zwroc_wo();
			else {
				glodny = 5;
			};

			if (!jedzenie.zwroc_r() != 0) { //mamy do czynienia ze slodyczem
				if ((5 - jedzenie.zwroc_r()) > szczescie)
					szczescie = szczescie + jedzenie.zwroc_r();
				else {
					szczescie = 5;
				};
			};

		};
		return true; 
	}; //zwraca informacje o tym czy sie powiodla akcja
	bool baw() { return true; }; //zwraca informacje o tym czy sie powiodla akcja
public:
	stworzenie() : glod(1), szczescie(0), chory(0), glodny(1), zmeczony(0), smutny(1), imie(""), wiek(0), zywy(1) { if (DEBUG_Z) std::cout << "wywolano konstruktor bezargumentowy klasy stworzenie" << std::endl; };
	virtual void wczytaj_sprite() { if (DEBUG_Z) std::cout << "Wczytuje sprite dla klasy stworzenie" << std::endl; };
	virtual void idle_animation() { if( DEBUG_Z ) std::cout << "Wyswietlam animacje petli dla klasy stworzenie" << std::endl; };
	virtual void drukuj_do(sf::RenderWindow& okno) {};

	std::string zwroc_imie() { return imie; };
	void ustaw_imie(const std::string& miano) { imie = miano; };

	int zwroc_wiek() { return wiek; };
	void ustaw_wiek(const int& lata) { wiek = lata; };

	int zwroc_glod() { return glod; };
	void ustaw_glod(const int& am) { glod = am; };

	int zwroc_szczescie() { return szczescie; };
	void ustaw_szczescie(const int& radowanie) { szczescie = radowanie; };
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

	void idle_animation() {
		//if (DEBUG_Z) std::cout << "Wyswietlam animacje petli dla klasy bobas" << std::endl;
		static std::vector<sf::Vector2f> pobierzpozycjebobasa = { { 3.f, -3.f }, { 3.f, 3.f }, { 3.f, -3.f }, { -3.f, 3.f }, { -3.f, -3.f }, { -3.f, 3.f },
																  { -3.f, -3.f }, { -3.f, 3.f }, { -3.f, -3.f }, { 3.f, 3.f }, { 3.f, -3.f }, { 3.f, 3.f } }; //f bo to floaty
		static int i = 0; //iterator
		static int ctr = 0; //liczy do 30

		if (ctr != 30) {
			duszek_bobasa.move(pobierzpozycjebobasa[i]);
			ctr += 1;
		}
		else {
			if (i == pobierzpozycjebobasa.size() - 1) { //todo zmienic na operator trojargumentowy
				i = 0;
			}
			else {
				i++;
			};
			ctr = 0;
			idle_animation();
		};
	};

	void drukuj_do(sf::RenderWindow& okno) {
		okno.draw(duszek_bobasa);
	};
};