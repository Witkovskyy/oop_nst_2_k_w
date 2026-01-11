/**
 * @brief Test helper: main.
 *
 * @details Used by the unit/integration test suite.
 * @return Integer result.
 */


﻿#include <SFML/Graphics.hpp>

/**
 * @brief Test helper: main.
 *
 * @details Used by the unit/integration test suite.
 * @return Integer result.
 */
int main()
{
    sf::RenderWindow okno(sf::VideoMode(800, 800), sf::String("Szachy"));

    while (okno.isOpen()) {
        sf::Event event; // <- Zmienna lokalna w pętli

        while (okno.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                /**
 * @brief Test helper: if.
 *
 * @details Used by the unit/integration test suite.
 * @param type Input event to process.
 * @return Result of the operation.
 */
                okno.close();

            if (event.type == sf::Event::MouseButtonPressed) {
                sf::Vector2i pos = sf::Mouse::getPosition(okno);
                // obsługa kliknięć
            }
        }

        /**
         * @brief Perform `clear`.
         *
         * @details Documentation for `clear`.
         * @param White Parameter.
         * @return Result of the operation.
         */
        okno.clear(sf::Color::White);
        okno.display();
    }

    return 0;
}
