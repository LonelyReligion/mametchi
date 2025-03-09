#include <SFML/Graphics.hpp>
#include <iostream>
#include <string>

/* TODO:
	- sprawdzic czy nie mozna gdzies wpisac consta i referencji na koniec
*/
bool DEBUG_P = 1;
export module guzik;

export class przycisk {
private:
	sf::RectangleShape prostokat;
	sf::Text tekst;

	sf::RectangleShape prostokat_w_zapasie;
	bool dezaktywowany = false;
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
	};

	void drukujdo(sf::RenderWindow &okno) {
		okno.draw(prostokat);
		okno.draw(tekst);
	};

	bool myszanad(sf::RenderWindow& okno) {
		if (!dezaktywowany)
		{
			double myszax = sf::Mouse::getPosition(okno).x;
			double myszay = sf::Mouse::getPosition(okno).y;

			double guzikx = prostokat.getPosition().x;
			double guziky = prostokat.getPosition().y;

			double xplusszer = guzikx + prostokat.getLocalBounds().width;
			double ypluswys = guziky + prostokat.getLocalBounds().height;

			if (myszax < xplusszer && myszax > guzikx && myszay < ypluswys && myszay > guziky) //czy jest w konturze
				return true;
			else
				return false;
		}
		return false;
	};

	std::string zwroc_tekst() {
		return tekst.getString().toAnsiString();
	}

	void dezaktywuj() 
	{
		prostokat_w_zapasie = prostokat;

		dezaktywowany = true;
		
		ustawkolortla(sf::Color(197, 197, 197));
		ustawkolortekstu(sf::Color(76, 91, 97));
	}

	void aktywuj() 
	{
		prostokat = prostokat_w_zapasie;
		dezaktywowany = false;
	}
};