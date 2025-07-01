#include <SFML/Graphics.hpp>

export module klikalny;

export class klikalny {
private:
protected:
public:
	bool dezaktywowany = false;
	double guzikx;
	double guziky;
	double xplusszer;
	double ypluswys;

	bool myszanad(sf::RenderWindow& okno) {
		if (!dezaktywowany)
		{
			double myszax = sf::Mouse::getPosition(okno).x;
			double myszay = sf::Mouse::getPosition(okno).y;

			if (myszax < xplusszer && myszax > guzikx && myszay < ypluswys && myszay > guziky) //czy jest w konturze
				return true;
			else
				return false;
		}
		return false;
	};

};