#include <SFML/Graphics.hpp>
#include <box2d/box2d.h>
#include<iostream>

#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600


// Pixels per meter. Box2D uses metric units, so we need to define a conversion
#define PPM 30.0F
// SFML uses degrees for angles while Box2D uses radians
#define DEG_PER_RAD 57.2957795F

// Box2D world for physics simulation, gravity = 9 m/s^2
b2World world(b2Vec2(0, -9));

// A structure with all we need to render a box
const struct Box
{
	float width;
	float height;
	sf::Color color;
	b2Body* body;
};
 
Box createBox(float x, float y, float width, float height, float density, float friction, sf::Color color)
{
	// Body definition
	b2BodyDef boxBodyDef;
	boxBodyDef.position.Set(x / PPM, y / PPM);
	boxBodyDef.type = b2_dynamicBody;

	// Shape definition
	b2PolygonShape boxShape;
	boxShape.SetAsBox(width / 2 / PPM, height / 2 / PPM);

	// Fixture definition
	b2FixtureDef fixtureDef;
	fixtureDef.density = density;
	fixtureDef.friction = friction;
	fixtureDef.shape = &boxShape;

	// Now we have a body for our Box object
	b2Body* boxBody = world.CreateBody(&boxBodyDef);
	// Lastly, assign the fixture
	boxBody->CreateFixture(&fixtureDef);

	return Box{ width, height, color, boxBody };
}

Box createGround(float x, float y, float width, float height, sf::Color color)
{
	// Static body definition
	b2BodyDef groundBodyDef;
	groundBodyDef.position.Set(x / PPM, y / PPM);

	// Shape definition
	b2PolygonShape groundBox;
	groundBox.SetAsBox(width / 2 / PPM, height / 2 / PPM);

	// Now we have a body for our Box object
	b2Body* groundBody = world.CreateBody(&groundBodyDef);
	// For a static body, we don't need a custom fixture definition, this will do:
	groundBody->CreateFixture(&groundBox, 0.0f);

	return Box{ width, height, color, groundBody };
}

void render(sf::RenderWindow& w, std::vector<Box>& boxes)
{
	sf::Text text;

	sf::Font font;
	font.loadFromFile("shortbaby.ttf");

	text.setFont(font);

	text.setString("Tap to Start");

	text.setCharacterSize(50);

	text.setFillColor(sf::Color::Blue);

	text.setPosition(265.f, 100.f);
	w.clear();
	for (const auto& box : boxes)
	{
		sf::RectangleShape rect;

		// For the correct Y coordinate of our drawable rect, we must substract from WINDOW_HEIGHT
		// because SFML uses OpenGL coordinate system where X is right, Y is down
		// while Box2D uses traditional X is right, Y is up
		rect.setPosition(box.body->GetPosition().x * PPM, WINDOW_HEIGHT - (box.body->GetPosition().y * PPM));

		// We also need to set our drawable's origin to its center
		// because in SFML, "position" refers to the upper left corner
		// while in Box2D, "position" refers to the body's center
		rect.setOrigin(box.width / 2, box.height / 2);

		rect.setSize(sf::Vector2f(box.width, box.height));

		// For the rect to be rotated in the crrect direction, we have to multiply by -1
		rect.setRotation(-1 * box.body->GetAngle() * DEG_PER_RAD);

		rect.setFillColor(box.color);
		w.draw(rect);
	}
	w.draw(text);
	w.display();
}

int main()
{
	// Setup SFML window
	
	sf::RenderWindow w(sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), "SFML + Box2D", sf::Style::Close);
	w.setFramerateLimit(60);
	int count = 0;
	
	// Container to hold all the boxes we create
	std::vector<Box> boxes;

	// Generate ground
	boxes.push_back(createGround(300, 20, 1000, 100, sf::Color::Magenta));


	// Generate a lot of boxes
	for (int i = 0; i < 250; i++)
	{
		// Starting positions are randomly generated: x between 50 and 550, y between 70 and 550
		auto&& box = createBox(150 + (std::rand() % (550 - 50 + 1)), 70 + (std::rand() % (550 - 70 + 1)), 15, 15, 1.f, 0.4f, sf::Color::Red);
		boxes.push_back(box);
	}
	/*if (sf::Mouse::isButtonPressed(sf::Mouse::Left))
	{
		auto&& box = createBox((float)localPosition.x, (float)localPosition.y, 15.f, 15.f, 1.f, 0.7f, sf::Color::Red);
		boxes.push_back(box);
	}*/


	// Create a big blue box
	//auto&& box = createBox(70, 200, 64, 64, 10.f, 0.7f, sf::Color::Blue);
	//boxes.push_back(box);

	//auto&& boxs = createBox(700, 200, 64, 64, 10.f, 0.7f, sf::Color::Blue);
	//boxes.push_back(boxs);

	// Yeet it leftwards to collide with the smaller boxes
	//box.body->ApplyForceToCenter(b2Vec2(100000, 10), false);
	//boxs.body->ApplyForceToCenter(b2Vec2(-100000, 10), false);
	//just a comment for idea

	/** GAME LOOP **/
	while (w.isOpen())
	{
		sf::Event evnt;
		
		if (sf::Mouse::isButtonPressed(sf::Mouse::Left))
		{

			float localPositionx = sf::Mouse::getPosition(w).x;
			float localPositiony = sf::Mouse::getPosition(w).y;
			
			auto&& box = createBox(localPositionx, WINDOW_HEIGHT - localPositiony, 15.f, 15.f, 1.f, 0.7f, sf::Color::Red);
			boxes.push_back(box);

		}
		
		
		while (w.pollEvent(evnt))
		{
			switch (evnt.type) {
			case sf::Event::Closed:
				w.close();
			case sf::Event::TextEntered:
				if (evnt.text.unicode < 128) {
					printf("%c", evnt.text.unicode);
				}

			}

			}
		
		// Update the world, standard arguments
		world.Step(1 / 60.f, 6, 3);
		
		// Render everything
		render(w, boxes);
		

		
	}

	return 0;
}