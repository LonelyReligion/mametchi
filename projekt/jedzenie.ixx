/*********************************************************************
 * @file  jedzenie.ixx
 *
 * @brief Implementacja i deklaracja klasy produkt.
 *********************************************************************/

#include <iostream>
#include <filesystem>

#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>

export module jedzenie;

/**
* @class produkt
* @brief Obiekt klasy produkt reprezentuje rodzaj produktu spozywczego.
*/
export class produkt {
private:
	std::string nazwa;  ///< Unikalna nazwa produktu.
	int wartosc_odzywcza;  ///< Ile punktow zostanie dodanych do statystyk stworzenia.
	int radosc;  ///< Ile punktow zostanie dodanych do statystyk stworzenia.

	sf::Texture tekstura;
	sf::Sprite duch;
	sf::Vector2f pozycja;
public:
	produkt() : wartosc_odzywcza(0), radosc(0), nazwa("") {};
	produkt(const int & wo, const int & r, const std::filesystem::path & p, const std::string & n) : wartosc_odzywcza(wo), radosc(r), nazwa(n) {
		wczytaj(p);
	};

	int zwroc_wo() const { return wartosc_odzywcza; };
	int zwroc_r() const { return radosc; };

	/**
	* @brief Wczytuje teksture z podanej sciezki.
	*
	* @param sciezka.
	*/
	void wczytaj(const std::filesystem::path& sciezka) {
		if (!tekstura.loadFromFile(sciezka.string())) {
			std::cout << "ladowanie tekstury jedzenia zakonczone niepowodzeniem" << std::endl;
		};
		tekstura.setSmooth(false);
		duch.setTexture(tekstura);
	};

	/**
	* @brief Rysuje produkt do okna.
	*
	* @param okno to okno, do ktorego rysujemy produkt.
	* @param gdzie wskazuje na lokalizacje, w ktorej ma sie znalezc sprite. 
	*/
	void rysuj(sf::RenderWindow& okno, sf::Vector2f gdzie) {
		duch.setOrigin(gdzie);
		okno.draw(duch);
	};
};