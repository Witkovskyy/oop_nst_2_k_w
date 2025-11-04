#include <SFML/Graphics.hpp>
int main() {
  sf::RenderWindow w(sf::VideoMode(800, 600), "SFML 2.6.2 (Docker)");
  sf::CircleShape s(120.f); s.setFillColor({90,180,250});
  while (w.isOpen()) { sf::Event e; while (w.pollEvent(e)) if (e.type==sf::Event::Closed) w.close();
    w.clear({30,30,40}); w.draw(s); w.display(); }
}
