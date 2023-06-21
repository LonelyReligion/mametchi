#include <iostream>
#include <string>
#include <filesystem> //do wczytywania spriteu bobasa?
#include <map>

#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>

import jedzenie;

bool DEBUG_Z = true;

export module zwierzak;

export class stworzenie {
private:
	std::string imie_rodzica;
	std::string imie;

	int glod; //0(glodny) - 5(nie glodny)
	int szczescie; //0 - 5
	int wiek;

	//bool chory = 0;
	//bool glodny = 0;
	//bool zmeczony = 0;
	//bool smutny = 0;

	bool zywy = 1;
	bool wyspany = 1;

	std::map <produkt, int> dania;
	std::map <produkt, int> przekaski;
protected:
public:
	stworzenie() : glod(0), szczescie(0), /*chory(0), glodny(1), zmeczony(0), smutny(1), */ imie(""), wiek(0), zywy(1) { if (DEBUG_Z) std::cout << "wywolano konstruktor bezargumentowy klasy stworzenie" << std::endl; };
	virtual void wczytaj_sprite() { if (DEBUG_Z) std::cout << "Wczytuje sprite dla klasy stworzenie" << std::endl; };
	virtual void idle_animation() { if( DEBUG_Z ) std::cout << "Wyswietlam animacje petli dla klasy stworzenie" << std::endl; };
	virtual void drukuj_do(sf::RenderWindow& okno, sf::Vector2f delta) {};
	virtual void spij(sf::Clock& budzik, sf::RenderWindow& okno) {};
	virtual sf::Sprite* zwroc_sprite() { return new sf::Sprite(); };

	void nakarm(const produkt& jedzenie) {
		if (DEBUG_Z) std::cout << "Jemy" << std::endl;
		//if (!chory) {
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
		//};
	}; //zwraca informacje o tym czy sie powiodla akcja

	////////////// get-ery i set-ery
	std::string zwroc_imie_rodzica() { return imie_rodzica; };
	void ustaw_imie_rodzica(const std::string& rodzic) { imie_rodzica = rodzic; };

	std::string zwroc_imie() { return imie; };
	void ustaw_imie(const std::string& miano) { imie = miano; };

	int zwroc_glod() { return glod; };
	void ustaw_glod(const int& am) { glod = am; };

	int zwroc_szczescie() { return szczescie; };
	void ustaw_szczescie(const int& radowanie) { szczescie = radowanie; };

	int zwroc_wiek() { return wiek; };
	void ustaw_wiek(const int& lata) { wiek = lata; };

	bool zwroc_zywy() { return zywy; };
	void ustaw_zywy(const bool& zycie) { zywy = zycie; };

	bool zwroc_wyspany() { return wyspany; };
	void ustaw_wyspany(const bool & spanie) { wyspany = spanie; };

	std::map <produkt, int> zwroc_dania() { return dania; };
	void ustaw_dania(const std::map <produkt, int>& menu) { dania = menu; };

	std::map <produkt, int> zwroc_przekaski() { return przekaski; };
	void ustaw_przekaski(const std::map <produkt, int> & menu) { przekaski = menu; };
	/////////////

	void postarz() { wiek += 1; };
};

export class Bobas : public stworzenie {
private:
	sf::Sprite duszek_bobasa;
	sf::Texture bobas; //musimy przechowywac i teksture i sprite'a poniewaz sprite przechowuje tylko pointer do tekstury

	sf::Sprite duszek_spiacego_bobasa;
	sf::Texture spiacy_bobas;
protected:
public:
	Bobas() { 
		(*this).wczytaj_sprite();
		(*this).ustaw_dania({});
		(*this).ustaw_przekaski({});
	};

	Bobas(const std::string& rodzic, const std::string& miano, const int& glodzik,
		const int& radosc, const int& lata, const bool& zyje, const bool& wypoczety,
		const std::map <produkt, int>& pozywienie, const std::map <produkt, int>& slodycze)
	{
		(*this).ustaw_imie_rodzica(rodzic);
		(*this).ustaw_imie(miano);
		(*this).ustaw_glod(glodzik);
		(*this).ustaw_szczescie(radosc);
		(*this).ustaw_wiek(lata);
		(*this).ustaw_zywy(zyje);
		(*this).ustaw_wyspany(wypoczety);
		(*this).ustaw_dania(pozywienie);
		(*this).ustaw_przekaski(slodycze);

		(*this).wczytaj_sprite();
	};

	sf::Sprite* zwroc_sprite() { return &duszek_bobasa; };
	virtual void wczytaj_sprite() { 
		if (DEBUG_Z) std::cout << "Wczytuje sprite'y dla klasy bobas" << std::endl; 
		if (!bobas.loadFromFile("obrazki/postaci/niemowle.png")) {
			std::cout << "ladowanie tekstury bobasa zakonczone niepowodzeniem" << std::endl;
		};
		bobas.setSmooth(false);
		duszek_bobasa.setTexture(bobas);

		if (DEBUG_Z) std::cout << "Wczytuje pozycje poczatkowa bobasa" << std::endl;
		duszek_bobasa.setOrigin(sf::Vector2f(-300.f, -250.f)); //x, y (0,0) jest w lewym gornym rogu

		if (!spiacy_bobas.loadFromFile("obrazki/postaci/niemowle_spi.png")) {
			std::cout << "ladowanie tekstury spiacego bobasa zakonczone niepowodzeniem" << std::endl;
		};
		spiacy_bobas.setSmooth(false);
		duszek_spiacego_bobasa.setTexture(spiacy_bobas);
		duszek_spiacego_bobasa.setOrigin(sf::Vector2f(-300.f, -250.f)); //x, y (0,0) jest w lewym gornym rogu
	};

	void drukuj_do(sf::RenderWindow& okno, sf::Vector2f delta) {
		duszek_bobasa.move(delta);
		okno.draw(duszek_bobasa);
	};

	void spij(sf::Clock & budzik, sf::RenderWindow & okno) {	
		if (budzik.getElapsedTime().asSeconds() <= 10) {
			okno.draw(duszek_spiacego_bobasa);
		}
		else {
			ustaw_wyspany(true);
		};
		//drukujemy czekamy zmieniamy stan
	};

};