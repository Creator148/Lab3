#include <iostream>

#include "SFML/Graphics.hpp"
#include "imgui.h"
#include "imgui-SFML.h"
#include "SFML/Graphics/RenderWindow.hpp"
#include "SFML/Graphics/Text.hpp"
#include "SFML/Graphics/Image.hpp"
#include "SFML/Graphics/Texture.hpp"
#include "SFML/Graphics/Sprite.hpp"
#include "SFML/Graphics/CircleShape.hpp"
#include "SFML/Graphics/RenderTexture.hpp"
#include "SFML/System/Clock.hpp"
#include "SFML/Window/Event.hpp"
#include "string"
#include "functional"
#include "cmath"

using namespace sf;



class RFuncSprite : public Sprite
{
public:
	
	std::vector<std::vector<float>> n1Values;
	std::vector<std::vector<float>> n2Values;
	void Create(const Vector2u &size) 
	{
		_Auall.create(size.x, size.y, Color::White); // Выставим цвет по умолчанию
		_textureAuall.loadFromImage(_Auall); // Проинициализируем текстуру
		_pathImage.create(_Auall.getSize().x, _Auall.getSize().y, Color::Transparent);
		n1Values.resize(size.x, std::vector<float>(size.y));
		n2Values.resize(size.x, std::vector<float>(size.y));
	}
	void SetAuAll()
	{
		setTexture(_textureAuall); // Выставим текстуру для нашего спрайта
	}
	void SetPath()
	{
		setTexture(_pathTexture); // Выставим текстуру для нашего спрайта
	}
	
	void saveAu() 
	{
		_Auall.saveToFile("./Source/Auall.jpg");
	}

	void processPixel(sf::Image &image, int x, int y, float n, sf::Uint8 r, sf::Uint8 g, sf::Uint8 b)
	{
		sf::Color pixelColor;
		pixelColor.r = static_cast<sf::Uint8>(((r) * (n + 1.f)) / 2.f);
		pixelColor.g = static_cast<sf::Uint8>(((g) * (n + 1.f)) / 2.f);
		pixelColor.b = static_cast<sf::Uint8>(((b) * (n + 1.f)) / 2.f);
		image.setPixel(x, y, pixelColor);
	}
	void DrawRFunc(const std::function<float(const sf::Vector2f &)> &rfunc, const sf::FloatRect &subSpace, float r,float g,float b)
		{
			for (unsigned x = 0; x < _Auall.getSize().x; ++x)
			{
				for (unsigned y = 0; y < _Auall.getSize().y; ++y)
				{
					Vector2f spaceStep = {
						subSpace.width / static_cast<float>(_Auall.getSize().x),
						subSpace.height / static_cast<float>(_Auall.getSize().y),
					};
					Vector2f spacePoint = {
						subSpace.left + static_cast<float>(x) * spaceStep.x,
						subSpace.top + static_cast<float>(y) * spaceStep.y,
					};

					const sf::Vector2f spacePoint1 = {spacePoint.x + spaceStep.x, spacePoint.y};
					const sf::Vector2f spacePoint2 = {spacePoint.x, spacePoint.y + spaceStep.y};

					Vector3f v1 = {	spacePoint.x, spacePoint.y,rfunc(spacePoint)};

					Vector3f v2 = {	spacePoint1.x, 
									spacePoint1.y,
									rfunc(spacePoint1)};

					Vector3f v3 = {	spacePoint2.x, 
									spacePoint2.y,
									rfunc(spacePoint2)};

					float A = v1.y * (v2.z - v3.z) - v2.y * (v1.z - v3.z) + v3.y * (v1.z - v2.z);
					float B = -(v1.x * (v2.z - v3.z) - v2.x * (v1.z - v3.z) + v3.x * (v1.z - v2.z));
					float C = v1.x * (v2.y - v3.y) - v2.x * (v1.y - v3.y) + v3.x * (v1.y - v2.y);
					float D = -(v1.x*(v2.y * v3.z - v3.y * v2.z) - v2.x * (v1.y * v3.z - v3.y * v1.z) +
									  v3.x * (v1.y * v2.z - v2.y * v1.z));

					float n1 = A / (sqrt(A * A + B * B + C * C + D * D));
					float n2 = B / (sqrt(A * A + B * B + C * C + D * D));
					n1Values[x][y] = n1;
					n2Values[x][y] = n2;
					processPixel(_Auall, x, y, n1, r, g, b);
				}
			}
			_textureAuall.loadFromImage(_Auall);
		}
		float getMvalue(unsigned int x, unsigned int y, int n)
		{
			if (n == 1)
			{
				return n1Values[x][y];
			}
			else if (n == 2 )
			{
				return n2Values[x][y];
			}
		}
		void findLocalMinimum(float startX,float startY, float step)
		{
			sf::Color pixelColor;
			pixelColor.r = 230;
			pixelColor.g = 13;
			pixelColor.b = 13;
			float x = startX;
			float y = startY;
			while (true)
			{
				if (x < 0 || y < 0 || x >= _Auall.getSize().x || y >= _Auall.getSize().y)
				{
					break;
				}
				_pathImage.setPixel(x, y, pixelColor);
				float currentCx = getMvalue((int)x, (int)y, 1);
				float currentCy = getMvalue((int)x, (int)y, 2);
				x += (step * currentCx);
				y += (step * currentCy);
			}
			_pathTexture.loadFromImage(_pathImage);
		}
		void clear()
		{
			_pathImage.create(_Auall.getSize().x, _Auall.getSize().y, Color::Transparent);
			_pathTexture.loadFromImage(_pathImage);
		}
	private:
	Texture _textureAuall;
	Texture _pathTexture;
	Image _pathImage;
	Image _Auall;
};

float RAnd(float w1, float w2) { return w1 + w2 - sqrt(w1 * w1 + w2 * w2); }
float ROr(float w1, float w2)
{ return w1 + w2 + sqrt(w1 * w1 + w2 * w2);}
float Head(const Vector2f &point) { return 1 - point.x * point.x / 6.25 - (point.y + 2) * (point.y + 2)/4; }
float Ear1(const Vector2f &point) { return 1 - (point.x - 2) * (point.x - 2) - (point.y + 3.5) * (point.y + 3.5)/0.25; }
float Ear2(const Vector2f &point)
{
	return 1 - (point.x + 2) * (point.x + 2) - (point.y + 3.5) * (point.y + 3.5) / 0.25;
}
float Hand1(const Vector2f &point)
{
	return 1 - (point.x - 3.5) * (point.x - 3.5) - (point.y - 1.5) * (point.y - 1.5) / 0.25;
}
float Hand2(const Vector2f &point)
{
	return 1 - (point.x + 3.5) * (point.x + 3.5) - (point.y - 1.5) * (point.y - 1.5) / 0.25;
}
float Body(const Vector2f &point) { return 1 - point.x * point.x / 9 - (point.y - 1.5) * (point.y - 1.5)/9; }
float Leg1(const Vector2f &point)
{
	return 1 - (point.x - 1.5) * (point.x - 1.5) - (point.y - 4.5) * (point.y - 4.5) / 0.25;
}
float Leg2(const Vector2f &point)
{
	return 1 - (point.x + 1.5) * (point.x + 1.5) - (point.y - 4.5) * (point.y - 4.5) / 0.25;
}
float Legs(const Vector2f &point) { return ROr(Leg1(point), Leg2(point)); }
float Hands(const Vector2f &point) { return ROr(Hand1(point), Hand2(point)); }
float Ears(const Vector2f &point) { return ROr(Ear1(point), Ear2(point)); }

float myFunc(const Vector2f &point)
{
	return ROr(ROr(ROr(Ears(point), Hands(point)), Legs(point)), ROr(Body(point), Head(point)));
}


RFuncSprite rFuncSprite;


void HandleUserInput(sf::RenderWindow &window, const sf::Event &event)
{
	switch (event.type)
	{
	case Event::Closed:
			window.close();
			break;
	case sf::Event::MouseButtonPressed:
	{
			if (event.key.code == Mouse::Left)
			{
				rFuncSprite.findLocalMinimum((float)Mouse::getPosition(window).x, (float)Mouse::getPosition(window).y,
											 5);
				rFuncSprite.SetPath();
				window.draw(rFuncSprite);
			}
			if (event.key.code == Mouse::Middle)
			{
				rFuncSprite.clear();
			}
	}		
	default:
			break;
	}
}

int main()
{
	rFuncSprite.Create(Vector2u(800,800));
	RenderWindow window(sf::VideoMode(800, 800), "Geometry modeling 2");
	window.setFramerateLimit(60);
	while (window.isOpen()){
		Event event;
		while (window.pollEvent(event))
		{
			HandleUserInput(window, event);
		}
		rFuncSprite.DrawRFunc(&myFunc, FloatRect(-7.5, -7.5, 15, 15),255,255,255);
		window.clear(Color::White);
		rFuncSprite.setPosition(0, 0);
		rFuncSprite.SetAuAll();
		window.draw(rFuncSprite);
		rFuncSprite.findLocalMinimum(0,0,1);
		rFuncSprite.SetPath();
		window.draw(rFuncSprite);
		window.display();
	}
	return 0;
};

