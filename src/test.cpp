#include <SFML/Graphics.hpp>

int main()
{
    sf::RenderWindow okno(sf::VideoMode(800, 800), sf::String("Szachy"));

    while (okno.isOpen()) {
        sf::Event event; // <- Zmienna lokalna w pętli

        while (okno.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                okno.close();

            if (event.type == sf::Event::MouseButtonPressed) {
                sf::Vector2i pos = sf::Mouse::getPosition(okno);
                // obsługa kliknięć
            }
        }

        okno.clear(sf::Color::White);
        okno.display();
    }

    return 0;
}
