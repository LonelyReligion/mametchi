#include <SFML/Graphics.hpp>
#include <iostream>
#include <sstream>

#define DELETE 8 //usuwamy a nie wpisujemy
#define ENTER  13
#define ESC 27

export module pole_tekstowe;

export class Pole {
private:
	sf::Text pole;
	std::ostringstream tekst;
	bool zaznaczone = false;
	bool limit = false;
	int limit_int;

	void wejscie(int znak) {
		if (znak != DELETE && znak != ENTER && znak != ESC) {
			tekst << static_cast<char>(znak);
		}
		else if (znak == DELETE) {
			if (tekst.str().length() > 0) {
				usunostatnialitere();
			}
		}
		else if (znak == ENTER) {
		}
		else if (znak == ESC) {
		};
		pole.setString(tekst.str() + "_");
	};

	void  usunostatnialitere() {
		std::string og = tekst.str();
		std::string nowy = "";

		for (int i = 0; i < og.length() - 1; ++i) {
			nowy += og[i];
		};
		tekst.str("");
		tekst << nowy;
		pole.setString(tekst.str());
	};

protected:
public:
	Pole() {};
	Pole(int rozmiar /* rozmiar tekstu */, sf::Color kolor,
		bool znaczenie /*zaznaczony po utworzeniu?*/)
	{
		this->zaznaczone = znaczenie;
		pole.setFillColor(kolor);

		if (znaczenie) {
			pole.setString("_");
		}
		else {
			pole.setString("");
		};
	};

	void ustawFont(sf::Font& font) {
		pole.setFont(font);
	};

	void ustawPozycje(sf::Vector2f pozycja) {
		pole.setPosition(pozycja);
	};

	void ustawLimit(bool TF) {
		this->limit = TF;
	};

	void ustawLimit(bool TF, int limit) {
		this->limit = TF;
		this->limit_int = limit - 1;
	};

	void ustawZaznaczenie(bool znaczenie) {
		zaznaczone = znaczenie;
		if (!znaczenie) {
			std::string og = tekst.str();
			std::string nowy = "";

			for (int i = 0; i < og.length(); ++i) { 
				nowy += og[i];
			};
			pole.setString(nowy);
		};
	};

	bool zwrocZaznaczenie() {
		return zaznaczone;
	};

	std::string zwroctekst() {
		return tekst.str();
	};

	void drukuj_do(sf::RenderWindow& okno) {
		okno.draw(pole);
	};

	void wpisany(sf::Event zdarzenie) {
		if (zaznaczone) {
			int znak = zdarzenie.text.unicode;
			if (znak < 128) { // zgodny znak
				if (limit) {
					if (tekst.str().length() <= limit_int) {
						wejscie(znak);
					}
					else if (tekst.str().length() > limit_int && znak == DELETE) {
						usunostatnialitere();
					};
				} 
				else {
					wejscie(znak);
				};
			};
		};
	};
};