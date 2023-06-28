/*********************************************************************
 * @file  przycisk.ixx
 *
 * @brief Deklaracja i implementacja klasy przycisk.
 *********************************************************************/

#include <SFML/Graphics.hpp>
#include <iostream>
#include <string>

/* TODO:
	- sprawdzic czy nie mozna gdzies wpisac consta i referencji na koniec
*/
bool DEBUG_P = 1;
export module guzik;

/**
* @class przycisk
* @brief Interaktywny przycisk. 
*/
export class przycisk {
private:
	sf::RectangleShape prostokat; ///< Tlo przycisku.
	sf::Text tekst; ///< Tekst na przycisku.
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

	void ustawkolortekstu(sf::Color barwa) { if (DEBUG_P) std::cout << "wywolano zmiane koloru tekstu" << std::endl;  tekst.setFillColor(barwa); };

	void ustawpozycje(sf::Vector2f xy) { 
		prostokat.setPosition(xy); 
		
		float x = (xy.x + prostokat.getLocalBounds().width / 3.5f) -
			(tekst.getLocalBounds().width / 2);
		float y = (xy.y + prostokat.getLocalBounds().height / 3.5f) -
			(tekst.getLocalBounds().height / 2);

		tekst.setPosition({x, y});
	};

	/**
	* @brief Rysuje przycisk do okna.
	*
	* @param okno to okno, do ktorego rysujemy przycisk.
	*/
	void drukujdo(sf::RenderWindow &okno) {
		okno.draw(prostokat);
		okno.draw(tekst);
	};

	/**
	* @brief Sprawdza czy kursor znajduje sie nad przyciskiem.
	*
	* @param okno to okno, do ktorego rysujemy przycisk.
	*/
	bool myszanad(sf::RenderWindow& okno) {
		double myszax = sf::Mouse::getPosition(okno).x;  
		double myszay = sf::Mouse::getPosition(okno).y; 

		double guzikx = prostokat.getPosition().x;
		double guziky = prostokat.getPosition().y;  

		double xplusszer = guzikx + prostokat.getLocalBounds().width;  
		double ypluswys = guziky + prostokat.getLocalBounds().height; 

		if (myszax < xplusszer && myszax > guzikx && myszay < ypluswys && myszay > guziky)
			//na pozniej: czy nie powinno byc <=
			return true;
		else
			return false;
	};
};