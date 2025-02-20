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

	int wygrane_pod_rzad = 0; /*liczba wygranych gier prawo-lewo pod rzad, decyduje o bonusie, roboczo: resetuj¹ sie przy ka¿dej nowej sesji gry*/

	//bool chory = 0;
	//bool glodny = 0;
	//bool zmeczony = 0;
	//bool smutny = 0;

	bool zywy = 1;
	bool wyspany = 1;

	std::vector <produkt> dania;
	std::vector <produkt> przekaski;
protected:
	sf::Sprite duszek;
	sf::Texture tekstura; //musimy przechowywac i teksture i sprite'a poniewaz sprite przechowuje tylko pointer do tekstury

	sf::Sprite duszek_spiacego;
	sf::Texture tekstura_spiacego;

public:
	std::filesystem::path lewy_profil;
	std::filesystem::path wygrana;
	std::filesystem::path przegrana;

	stworzenie() : glod(0), szczescie(0), /*chory(0), glodny(1), zmeczony(0), smutny(1), */ imie(""), wiek(0), zywy(1) { if (DEBUG_Z) std::cout << "wywolano konstruktor bezargumentowy klasy stworzenie" << std::endl; };
	virtual void wczytaj_sprite() { if (DEBUG_Z) std::cout << "Wczytuje sprite dla klasy stworzenie" << std::endl; };
	virtual void idle_animation() { if( DEBUG_Z ) std::cout << "Wyswietlam animacje petli dla klasy stworzenie" << std::endl; };
	virtual void drukuj_do(sf::RenderWindow& okno, sf::Vector2f delta) {};
	virtual bool spij(sf::Clock& budzik, sf::RenderWindow& okno) { return false; };
	virtual sf::Sprite* zwroc_sprite() { return new sf::Sprite(); };

	void zeruj_wygrane_pod_rzad() {
		wygrane_pod_rzad = 0;
	}
	void inkrementuj_wygrane_pod_rzad() {
		wygrane_pod_rzad += 1;
	}
	int get_wygrane_pod_rzad() {
		return wygrane_pod_rzad;
	}
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

	std::vector <produkt> zwroc_dania() { return dania; };
	void ustaw_dania(const std::vector <produkt>& menu) { dania = menu; };

	std::vector <produkt> zwroc_przekaski() { return przekaski; };
	void ustaw_przekaski(const std::vector <produkt> & menu) { przekaski = menu; };

	void dodaj_danie(produkt p) { dania.push_back(p); };
	std::vector<produkt> pobierz_dania() { return dania; };

	void dodaj_przekaske(produkt p) { przekaski.push_back(p); };
	std::vector<produkt> pobierz_przekaski() { return przekaski; };
	/////////////

	void postarz() { wiek += 1; };
};

export class Bobas : public stworzenie {
private:
protected:
public:
	Bobas() { 
		lewy_profil = "OBRAZKI/POSTACI/NIEMOWLE_LEWO.png";
		wygrana = "OBRAZKI/POSTACI/NIEMOWLE_RADOSC.png";
		przegrana = "OBRAZKI/POSTACI/NIEMOWLE_SMUTEK.png";

		(*this).ustaw_dania({});
		(*this).ustaw_przekaski({});
	};

	Bobas(const std::string rodzic, const std::string miano, const int& glodzik,
		const int& radosc, const int& lata, const bool& zyje, const bool& wypoczety,
		const std::vector <produkt>& pozywienie, const std::vector <produkt>& slodycze)
	{
		lewy_profil = "OBRAZKI/POSTACI/NIEMOWLE_LEWO.png";
		wygrana = "OBRAZKI/POSTACI/NIEMOWLE_RADOSC.png";
		przegrana = "OBRAZKI/POSTACI/NIEMOWLE_SMUTEK.png";

		ustaw_imie_rodzica(rodzic);
		ustaw_imie(miano);

		(*this).ustaw_glod(glodzik);
		(*this).ustaw_szczescie(radosc);
		(*this).ustaw_wiek(lata);
		(*this).ustaw_zywy(zyje);
		(*this).ustaw_wyspany(wypoczety);
		(*this).ustaw_dania(pozywienie);
		(*this).ustaw_przekaski(slodycze);
	};

	sf::Sprite* zwroc_sprite() { return &duszek; };

	virtual void wczytaj_sprite() { 
		if (DEBUG_Z) std::cout << "Wczytuje sprite'y dla klasy bobas" << std::endl; 
		if (!tekstura.loadFromFile("obrazki/postaci/niemowle.png")) {
			std::cout << "ladowanie tekstury bobasa zakonczone niepowodzeniem" << std::endl;
		};
		tekstura.setSmooth(false);
		duszek.setTexture(tekstura);

		if (DEBUG_Z) std::cout << "Wczytuje pozycje poczatkowa bobasa" << std::endl;
		duszek.setOrigin(sf::Vector2f(-300.f, -250.f)); //x, y (0,0) jest w lewym gornym rogu

		if (DEBUG_Z) std::cout << "Wczytuje sprite'y dla klasy bobas" << std::endl;
		if (!tekstura_spiacego.loadFromFile("obrazki/postaci/niemowle_spi.png")) {
			std::cout << "ladowanie tekstury spiacego bobasa zakonczone niepowodzeniem" << std::endl;
		};
		tekstura_spiacego.setSmooth(false);
		duszek_spiacego.setTexture(tekstura_spiacego);

		if (DEBUG_Z) std::cout << "Wczytuje pozycje poczatkowa bobasa" << std::endl;
		duszek_spiacego.setOrigin(sf::Vector2f(-300.f, -250.f)); //x, y (0,0) jest w lewym gornym rogu
	};

	void drukuj_do(sf::RenderWindow& okno, sf::Vector2f delta) {
		duszek.move(delta);
		okno.draw(duszek);
	};

	//czy nast¹pi ewolucja
	bool spij(sf::Clock & budzik, sf::RenderWindow & okno) {	
		if (budzik.getElapsedTime().asSeconds() <= 10) {
			okno.draw(duszek_spiacego);
		}
		else if(!zwroc_wyspany()){
			ustaw_wyspany(true);
			ustaw_wiek(zwroc_wiek()+1);
			std::cout << "nowy wiek zwierzaka: " << zwroc_wiek() << "\n";

			if (zwroc_wiek() == 3) {
				//pora na ewolucje
				return true;
			}
		};
		//drukujemy czekamy zmieniamy stan
		return false;
	};

};

export class Podrostek : public stworzenie {	
	public:

	Podrostek() {
		lewy_profil = "OBRAZKI/POSTACI/podrostek.png";
		wygrana = "OBRAZKI/POSTACI/podrostek.png";
		przegrana = "OBRAZKI/POSTACI/podrostek.png";

		(*this).ustaw_dania({});
		(*this).ustaw_przekaski({});
	};

	Podrostek(stworzenie bazowe) {
		lewy_profil = "OBRAZKI/POSTACI/podrostek.png";
		wygrana = "OBRAZKI/POSTACI/podrostek.png";
		przegrana = "OBRAZKI/POSTACI/podrostek.png";

		ustaw_imie_rodzica(bazowe.zwroc_imie_rodzica());
		ustaw_imie(bazowe.zwroc_imie());

		(*this).ustaw_glod(bazowe.zwroc_glod());
		(*this).ustaw_szczescie(bazowe.zwroc_szczescie());
		(*this).ustaw_wiek(bazowe.zwroc_wiek());
		(*this).ustaw_zywy(bazowe.zwroc_zywy());
		(*this).ustaw_wyspany(bazowe.zwroc_wyspany());
		(*this).ustaw_dania(bazowe.zwroc_dania());
		(*this).ustaw_przekaski(bazowe.zwroc_przekaski());;
	}

	Podrostek(const std::string rodzic, const std::string miano, const int& glodzik,
		const int& radosc, const int& lata, const bool& zyje, const bool& wypoczety,
		const std::vector <produkt>& pozywienie, const std::vector <produkt>& slodycze)
	{
		lewy_profil = "OBRAZKI/POSTACI/podrostek.png";
		wygrana = "OBRAZKI/POSTACI/podrostek.png";
		przegrana = "OBRAZKI/POSTACI/podrostek.png";

		ustaw_imie_rodzica(rodzic);
		ustaw_imie(miano);

		(*this).ustaw_glod(glodzik);
		(*this).ustaw_szczescie(radosc);
		(*this).ustaw_wiek(lata);
		(*this).ustaw_zywy(zyje);
		(*this).ustaw_wyspany(wypoczety);
		(*this).ustaw_dania(pozywienie);
		(*this).ustaw_przekaski(slodycze);
	};

	void drukuj_do(sf::RenderWindow& okno, sf::Vector2f delta) {
		duszek.move(delta);
		okno.draw(duszek);
	};

	virtual void wczytaj_sprite() {
		if (DEBUG_Z) std::cout << "Wczytuje sprite'y dla klasy bobas" << std::endl;
		if (!tekstura.loadFromFile("obrazki/postaci/podrostek.png")) {
			std::cout << "ladowanie tekstury bobasa zakonczone niepowodzeniem" << std::endl;
		};
		tekstura.setSmooth(false);
		duszek.setTexture(tekstura);

		if (DEBUG_Z) std::cout << "Wczytuje pozycje poczatkowa bobasa" << std::endl;
		duszek.setOrigin(sf::Vector2f(-300.f, -250.f)); //x, y (0,0) jest w lewym gornym rogu

		if (DEBUG_Z) std::cout << "Wczytuje sprite'y dla klasy bobas" << std::endl;
		if (!tekstura_spiacego.loadFromFile("obrazki/postaci/podrostek.png")) {
			std::cout << "ladowanie tekstury spiacego bobasa zakonczone niepowodzeniem" << std::endl;
		};
		tekstura_spiacego.setSmooth(false);
		duszek_spiacego.setTexture(tekstura_spiacego);

		if (DEBUG_Z) std::cout << "Wczytuje pozycje poczatkowa bobasa" << std::endl;
		duszek_spiacego.setOrigin(sf::Vector2f(-300.f, -250.f)); //x, y (0,0) jest w lewym gornym rogu
	};
};