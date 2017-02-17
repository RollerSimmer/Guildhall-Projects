#include "Game/Ship.hpp"
#include "Engine/Renderer/Renderer.hpp"
#include "Game/GameCommon.hpp"
#include "Engine/Math/MathUtil.hpp"

Ship::Ship(Vector2& pos, float headingDegrees)
	: Entity(pos,DEFAULT_SHIP_LENGTH*0.5f,0.f)
	, m_accel(Vector2::ZERO)
	{
	m_orientationDegrees=headingDegrees;
	}

Ship::Ship(Vector2 pos, Vector2 vel/*=Vector2::ZERO*/, float mass/*=0.f*/, float headingDegrees/*=90.f*/, float radius)
	: Entity(pos, radius, 0.f)
	, m_accel(Vector2::ZERO)
	{
	m_pos=pos;
	m_vel=vel;
	m_mass=mass;
	m_orientationDegrees=headingDegrees;
	m_physicsRadius=radius*0.7f;
	m_graphicsRadius=radius;
	m_angularVelocity=0.f;
	}

void Ship::stopThrusting()
	{
	m_analogThrust=0.f;
	}

void Ship::update(float deltaSeconds)
	{
	readInput();
	accelerate(deltaSeconds);
	rotate(deltaSeconds);
	move(deltaSeconds);
	growShipByInput();
	wrapAround();
	}

void Ship::render() const
{
	drawThrust();
	drawShip();
	Entity::render();
	}

void Ship::accelerate(float deltaSeconds)
	{
	m_accel=calcHeadingVector()*m_analogThrust;

	m_vel+=m_accel*deltaSeconds;
	}

void Ship::drawShip() const
{
	float radius=m_graphicsRadius*m_drawScale;
	float halfRadius = 0.5f*radius;
	float quarterRadius = 0.25f*radius;
	const int AMT_VERTICES = 5;
	Vector2 vertices[AMT_VERTICES];
	vertices[0].SetXY(radius,0.f);
	vertices[1].SetXY(-halfRadius, -halfRadius);
	vertices[2].SetXY(-quarterRadius, -quarterRadius);
	vertices[3].SetXY(-quarterRadius, quarterRadius);
	vertices[4].SetXY(-halfRadius, halfRadius);
	g_theRenderer->drawNGon(m_pos,m_orientationDegrees,vertices, AMT_VERTICES, Rgba::SILVER);
	}

void Ship::drawThrust() const
	{
	if(m_analogThrust==0.f)
		return;
	float radius=m_graphicsRadius*m_drawScale;
	float halfRadius = 0.5f*m_graphicsRadius;
	float quarterRadius = 0.25f*m_graphicsRadius;
	const int AMT_VERTICES = 3;
	Vector2 vertices[AMT_VERTICES];
	vertices[0].SetXY(-quarterRadius, -quarterRadius);
	float thrust= clampFloat(m_analogThrust, 0.f, 1.f);
	vertices[1].SetXY(GetRandomFloatInRange(-halfRadius, -0.75f*radius*thrust),0.f);
	vertices[2].SetXY(-quarterRadius, quarterRadius);
	const int AMT_IGNITION_COLORS=3;
	Rgba ignitionColors[AMT_IGNITION_COLORS] = {Rgba::RED,Rgba::YELLOW,Rgba::ORANGE};
	Rgba chosenColor=Rgba::BLACK;
	float colorscale;
	float scalesum=0.f;
	for (int i = 0; i < AMT_IGNITION_COLORS-1; i++)
		{
		colorscale=GetRandomFloatInRange(0.1f,0.49f);
		scalesum+=colorscale;
		chosenColor=chosenColor+colorscale*ignitionColors[i];
		}
	colorscale=1.f-scalesum;
	chosenColor = chosenColor + colorscale*ignitionColors[AMT_IGNITION_COLORS-1];
	chosenColor.m_named.m_a=190;
	//int iIgnitionColor= GetRandomIntInRange(0,AMT_IGNITION_COLORS-1);
	g_theRenderer->drawNGon(m_pos, m_orientationDegrees, vertices, AMT_VERTICES, chosenColor);
	}

void Ship::readInput()
	{
	XboxController* xboxctrl= &g_theInput->getXboxController(XCONTROLLER_0);
	m_analogThrust=0.f;
	float tiltValue = xboxctrl->getStickValue(XSTICK_LEFT,Y_AXIS);
	if(tiltValue<0.f || tiltValue>0.f)
		m_analogThrust=1.5f*tiltValue*SHIP_ACCELERATION_RATE;
	else if (g_theInput->isKeyDown('E') || g_theInput->isKeyDown(KEYCODE_UP) || xboxctrl->isXBoxButtonPressed(XBUTTON_UP))
		m_analogThrust = SHIP_ACCELERATION_RATE;	
	else if (g_theInput->isKeyDown('C') || g_theInput->isKeyDown(KEYCODE_DOWN) || xboxctrl->isXBoxButtonPressed(XBUTTON_DOWN))
		m_analogThrust = -SHIP_ACCELERATION_RATE;
	else 
		m_analogThrust=0.f;

	if (g_theInput->isKeyDown('Z'))
		{
		m_vel=m_accel=Vector2::ZERO;
		m_analogThrust=0.f;
		}

	m_angularVelocity=0.f;
	float turnValue = xboxctrl->getStickValue(XSTICK_LEFT, X_AXIS);
	if (turnValue < 0.f || turnValue > 0.f)
		{
		m_angularVelocity = -2.f*turnValue*SHIP_TURNING_RATE;
		}
	else
		{
		if (g_theInput->isKeyDown('S') || g_theInput->isKeyDown(KEYCODE_LEFT) || g_theInput->getXboxController(XCONTROLLER_0).isXBoxButtonPressed(XBUTTON_LEFT))
			m_angularVelocity = SHIP_TURNING_RATE;
		if (g_theInput->isKeyDown('F') || g_theInput->isKeyDown(KEYCODE_RIGHT) || g_theInput->getXboxController(XCONTROLLER_0).isXBoxButtonPressed(XBUTTON_RIGHT))
			m_angularVelocity = -SHIP_TURNING_RATE;
		}
	}

void Ship::growShipByInput()
	{
	const float RADIUS_STEP=1.f;
	if (g_theInput->isKeyDown(KEYCODE_ADD) || g_theInput->getXboxController(XCONTROLLER_0).isXBoxButtonPressed(XBUTTON_LB))
		m_graphicsRadius += RADIUS_STEP;
	if (g_theInput->isKeyDown(KEYCODE_SUBTRACT) || g_theInput->getXboxController(XCONTROLLER_0).isXBoxButtonPressed(XBUTTON_RB))
		{
		m_graphicsRadius -= RADIUS_STEP;
		if(m_graphicsRadius<0.f)
			m_graphicsRadius=0.f;
		}
	m_physicsRadius = GRAPHICS_TO_PHYSICS_RADIUS_RATIO*m_graphicsRadius;
	}

void Ship::activateMaxForwardThrust()
	{
	m_analogThrust = SHIP_ACCELERATION_RATE;
	}
