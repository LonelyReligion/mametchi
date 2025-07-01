#include <SFML/Graphics.hpp>
#include <iostream>
#include <string>

import klikalny;

/* TODO:
	- sprawdzic czy nie mozna gdzies wpisac consta i referencji na koniec
*/
bool DEBUG_P = 1;
export module guzik;

export class przycisk : public klikalny {
private:
	sf::RectangleShape prostokat;
	sf::Text tekst;

	sf::RectangleShape prostokat_w_zapasie;
	sf::Color kolor_aktywnego_napisu;
protected:
public:
	przycisk() {};
	
	przycisk(std::string tresc, sf::Vector2f rozmiar, int wielkosc, sf::Color tlo, sf::Color barwa, sf::Vector2f xy, sf::Font& font) {
		tekst.setString(tresc);
		tekst.setFillColor(barwa);
		tekst.setCharacterSize(wielkosc);

		prostokat.setFillColor(tlo);
		prostokat.setSize(rozmiar);

		this->ustawpozycje(xy);
		this->ustawFont(font);


		guzikx = prostokat.getPosition().x;
		guziky = prostokat.getPosition().y;
		xplusszer = guzikx + prostokat.getLocalBounds().width;
		ypluswys = guziky + prostokat.getLocalBounds().height;
	};

	void ustawFont(sf::Font& font) {
		tekst.setFont(font);
	};

	void ustawkolortla(sf::Color tlo) { prostokat.setFillColor(tlo); };

	void ustawkolortekstu(sf::Color barwa) { tekst.setFillColor(barwa); };

	void ustawpozycje(sf::Vector2f xy) { 
		prostokat.setPosition(xy); 
		
		float x = (xy.x + prostokat.getLocalBounds().width / 3.5f) -
			(tekst.getLocalBounds().width / 2);
		float y = (xy.y + prostokat.getLocalBounds().height / 3.5f) -
			(tekst.getLocalBounds().height / 2);

		tekst.setPosition({x, y});

		guzikx = prostokat.getPosition().x;
		guziky = prostokat.getPosition().y;
		xplusszer = guzikx + prostokat.getLocalBounds().width;
		ypluswys = guziky + prostokat.getLocalBounds().height;
	};

	void drukujdo(sf::RenderWindow &okno) {
		okno.draw(prostokat);
		okno.draw(tekst);
	};

	std::string zwroc_tekst() {
		return tekst.getString().toAnsiString();
	}

	void dezaktywuj() 
	{
		prostokat_w_zapasie = prostokat;
		kolor_aktywnego_napisu = tekst.getFillColor();
		dezaktywowany = true;
		
		ustawkolortla(sf::Color(197, 197, 197));
		ustawkolortekstu(sf::Color(76, 91, 97));
	}

	void aktywuj() 
	{
		tekst.setFillColor(kolor_aktywnego_napisu); //nie dziala przy spaniu, dlaczego?
		prostokat = prostokat_w_zapasie;

		dezaktywowany = false;
	}

	sf::Color getKolorPostokata() 
	{
		return prostokat.getFillColor();
	}
};