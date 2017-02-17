#include "Game/Game.hpp"
#include "Game/GameCommon.hpp"
#include <cmath>
#include <cstdlib>

const float END_MOVIE_FRAME_SECONDS = 0.05f;

void Game::processSingleBulletCollisionWithAsteroid(int ibullet,int iasteroid)
	{
	Asteroid*& lameduckAsteroid=m_asteroids[iasteroid];
	float bulletToAsteroidScale=BULLET_MASS/lameduckAsteroid->getMass();
	Vector2 asteroidVelocity=lameduckAsteroid->getVelocity();
	Vector2 bulletVelocity = m_bullets[ibullet]->getVelocity();
	bulletVelocity *= bulletToAsteroidScale;
	lameduckAsteroid->setVelocity(asteroidVelocity+bulletVelocity);
	destroyAsteroid(iasteroid);
	destroyBullet(ibullet);
	}

void Game::destroyBullet(int ibullet)
	{
	if(ibullet<0 || ibullet>=AMT_BULLET_SLOTS)
		return;
	if (m_bullets[ibullet] != nullptr)
		{
		delete m_bullets[ibullet];
		m_bullets[ibullet] = nullptr;
		}
	}

void Game::shootBulletFromShip()
	{
	if(m_theShip == nullptr)
		return;
	const float BULLET_SPEED = 1000.f;
	float bulletSpeed=BULLET_SPEED;
	Vector2 shipVelocity = m_theShip->getVelocity();
	Vector2 shipHeading = m_theShip->calcHeadingVector();
	Vector2 bulletVelocity=shipHeading;
	Vector2 bulletStartPos=shipHeading;
	bulletStartPos.SetLength(m_theShip->getGraphicsRadius());
	bulletStartPos+=m_theShip->getpos();
	int ibullet=findFreeBulletSlot();
	bulletVelocity.SetLength(bulletSpeed);
	bulletVelocity+=shipVelocity;
	m_bullets[ibullet] = new Bullet(bulletStartPos,bulletVelocity);
	}

int Game::findFreeBulletSlot()
	{
	for (int ibullet = 0; ibullet < AMT_BULLET_SLOTS; ibullet++)
		{
		if(m_bullets[ibullet]==nullptr)
			return ibullet;
		}
	return freeOldestBullet();
	}

int Game::freeOldestBullet()
	{
	float ageOfOldest = BULLET_LIFESPAN;
	int indexOfOldest=0;
	for (int ibullet = 0; ibullet < AMT_BULLET_SLOTS; ibullet++)
		{
		if (m_bullets[ibullet] == nullptr)
			{
			float thisAge= m_bullets[ibullet]->getTimeLeftToLive();
			if (thisAge> ageOfOldest)
				{
				ageOfOldest=thisAge;
				indexOfOldest=ibullet;
				}
			}
		}
	if (m_bullets[indexOfOldest]!=nullptr)
		{
		delete m_bullets[indexOfOldest];
		m_bullets[indexOfOldest]=nullptr;
		}
	return indexOfOldest;
	}

void Game::togglePause()
	{
	if(m_isPaused)
		m_isPaused = false;
	else
		m_isPaused = true;
	}

void Game::progressPlayStage()
	{
	if (areAllAsteroidsDestroyed())
		{
		if (m_playStage < AMT_PLAY_STAGES-1)
			{
			m_playStage++;
			startNewStage();
			}
		else if (m_theShip != nullptr)
			{
			m_gameState = GAME_STATE_MOVING_TO_CENTER_FOR_END;
			}
		}
	}


void Game::progressEndSequence(float deltaSeconds)
	{
	if (m_gameState == GAME_STATE_MOVING_TO_CENTER_FOR_END)
		{
		if(m_theShip)
			m_theShip->activateMaxForwardThrust();
		if(!isShipAtCenterForWarp())
			moveShipTowardCenter(deltaSeconds);
		else
			{
			m_gameState = GAME_STATE_FIRST_STAR_LINE_FRAME;
			startScreenShake();
			}
		}
	else
		{
		m_endFrameProgression += deltaSeconds;
		while (m_endFrameProgression > END_MOVIE_FRAME_SECONDS)
			{
			if (m_gameState < GAME_STATE_GAME_OVER_STAGE)
				m_gameState++;
			m_endFrameProgression -= END_MOVIE_FRAME_SECONDS;
			}
		if (m_gameState == GAME_STATE_GAME_OVER_STAGE)
			m_isQuittingGame = true;
		if (m_gameState >= GAME_STATE_PLANET_ARRIVAL_STAGE  && m_theShip)
			{
			m_theShip->stopThrusting();
			scaleShipByGameOverStage();
			}
		}
	}


void Game::renderStarLines(int frame) const
	{
	if(m_starField)
		m_starField->renderStarLines(frame);
	if (m_theShip)
		m_theShip->render();
	renderHUD();
	}

void Game::renderPlanet(int frame) const
	{
	if(m_starField)
		m_starField->render();

	if (m_thePlanet != nullptr)
		{
		float scale=(float)frame/(float)(GAME_STATE_FIRST_PLANET_FRAME-GAME_STATE_LAST_PLANET_FRAME);
		//float scale=1.f;
		float x = m_thePlanet->m_center.x;
		float y = m_thePlanet->m_center.y;
		float r = m_thePlanet->m_radius*scale;
		LineSegment2 gradientLine(x+r,y,x-r,y);
		g_theRenderer->drawDisc2WithLinearGradient(x,y,r,DEFAULT_CIRCLE_SIDE_AMT,Rgba::GREEN,Rgba::BLACK,gradientLine);
		if(m_theShip)
			m_theShip->render();
		}
	renderHUD();
	}

void Game::renderGameOver() const
	{
	int iframe = m_gameState - GAME_STATE_PLANET_ARRIVAL_STAGE;
	renderPlanet(GAME_STATE_LAST_PLANET_FRAME - GAME_STATE_FIRST_PLANET_FRAME);
	renderHUD();
	renderFadeToBlack(iframe);
	}

void Game::renderRank() const
	{
	int rank=m_playStage+1;
	if(m_gameState>GAME_STATE_PLAYING)
		rank=5;
	const float STAR_RADIUS=15.f;
	const float SPACE_BETWEEN_STAR_CENTERS = STAR_RADIUS*2.f*1.2f;
	const float INITIAL_STAR_POSITION_XY = SPACE_BETWEEN_STAR_CENTERS*0.5f;
	float x = SPACE_BETWEEN_STAR_CENTERS;
	float y = g_theWorldBounds->m_maxs.y - SPACE_BETWEEN_STAR_CENTERS;
	for (int i = 0; i < rank; i++)
		{
		g_theRenderer->drawStarFivePointed(x,y,STAR_RADIUS,0.f,Rgba::GOLD);
		x += SPACE_BETWEEN_STAR_CENTERS;
		}
	}

void Game::renderFadeToBlack(int iframe) const 
	{
	Rgba fadecolor=Rgba::BLACK;
	float alpha=(float)iframe/(float)(GAME_STATE_GAME_OVER_STAGE-GAME_STATE_PLANET_ARRIVAL_STAGE);
	fadecolor.setAlphaAsFloat(alpha);
	g_theRenderer->drawFilledBox(*g_theWorldBounds,fadecolor);
	}

void Game::renderHUD() const
	{
	renderRank();
	}

bool Game::areAllAsteroidsDestroyed()
	{
	for (int iasteroid = 0; iasteroid < AMT_ASTEROID_SLOTS; iasteroid++)
		{
		if(m_asteroids[iasteroid]!=nullptr)
			return false;
		}

	return true;
	//stub
	}

void Game::spawnThePlanet()
	{
	if (m_thePlanet == nullptr)
		{
		m_thePlanet=new Disc2(g_theWorldBounds->CalcCenter(),g_theWorldBounds->calcHeight()*0.4f);
		}
	}

bool Game::isQuitting()
	{
	return m_isQuittingGame;
	}

void Game::deleteAllAsteroids()
	{
	for (int i = 0; i < AMT_ASTEROID_SLOTS; i++)
		{
		if(m_asteroids[i])
			deleteAsteroid(i);
		}
	}

void Game::processShipCollisionWithAsteroid(int iasteroid)
	{
	destroyTheShip();
	destroyAsteroid(iasteroid);
	startScreenShake();
	}

void Game::startScreenShake()
	{
	m_currentShakeAmount = SCREEN_SHAKE_AMOUNT;
	m_currentShakeTimeLeft = SCREEN_SHAKE_TIME_SPAN;
	}

void Game::doScreenShake() const
	{
	float screenShakeX = GetRandomFloatInRange(-m_currentShakeAmount, m_currentShakeAmount);
	float screenShakeY = GetRandomFloatInRange(-m_currentShakeAmount, m_currentShakeAmount);
	g_theRenderer->translate2D(Vector2(screenShakeX, screenShakeY));
	}

void Game::scaleShipByGameOverStage()
	{
	float scale=1.f-(float)(m_gameState-GAME_STATE_PLANET_ARRIVAL_STAGE)/(float)(GAME_STATE_GAME_OVER_STAGE-1-GAME_STATE_PLANET_ARRIVAL_STAGE);
	m_theShip->setDrawScale(scale);
	}

Game::Game()
	: m_asteroids{ nullptr }
	, m_theShip{ nullptr }
	, m_bullets{ nullptr }
	, m_timescale(10)
	, m_isPaused(false)
	, m_gameState(GAME_STATE_PLAYING)
	, m_thePlanet(nullptr)
	, m_isQuittingGame(false)
	, m_playStage(PLAY_STAGE_0)
	, m_currentShakeAmount(0.f)
	, m_currentShakeTimeLeft(0.f)
	{
	init();
	}

int Game::findEmptyAsteroidSlot()
	{
	int result=-1;
	bool wasEmptySlotFound=false;
	for (int i = 0; i < AMT_ASTEROID_SLOTS && !wasEmptySlotFound; i++)
		{
		if(m_asteroids[i]==nullptr)
			{
			result=i;
			wasEmptySlotFound=true;
			}
		}
	return result;
	}

void Game::spawnTheShip()
	{
	if(!m_theShip)
		m_theShip = new Ship(0.5f*(g_theWorldBounds->m_mins + g_theWorldBounds->m_maxs),Vector2::ZERO,90.f,90.f,g_theWorldBounds->calcHeight()*0.025f);
	}

void Game::processCollisions()
	{
	processShipCollisions();
	processAsteroidCollisions();
	processBulletCollisionsWithAsteroids();
	}

void Game::processShipCollisions()
	{
	for (int iasteroid = 0; iasteroid < AMT_ASTEROID_SLOTS && m_theShip != nullptr; iasteroid++)
		{
		if (m_asteroids[iasteroid])
			{
			if (m_theShip->doesCollideWith(*m_asteroids[iasteroid]))
				{
				processShipCollisionWithAsteroid(iasteroid);
				}
			}
		}
	}

void Game::processSingleAsteroidCollision(Asteroid* a, Asteroid* b)
	{
	if (a == nullptr || b == nullptr)
		return;
	separateEntities(*a,*b);
	Vector2 aMomentum = a->calcMomentum();	
	Vector2 bMomentum = b->calcMomentum();
	Vector2 L = b->getpos() - a->getpos();
	L.Normalize();
	float aMomentumAlongL = dotProduct(aMomentum, L);
	float bMomentumAlongL = dotProduct(bMomentum, L);
	Vector2 oldAMomentumL = aMomentumAlongL*L;
	Vector2 oldBMomentumL = bMomentumAlongL*L;
	Vector2 newAMomentumL = oldBMomentumL;
	Vector2 newBMomentumL = oldAMomentumL;
	Vector2 newBMomentumN = aMomentum - oldAMomentumL;
	Vector2 newAMomentumN = bMomentum - oldBMomentumL;
	Vector2 newAMomentum = newAMomentumL + newAMomentumN;
	Vector2 newBMomentum = newBMomentumL + newBMomentumN;
	a->setVelocity(newAMomentum/a->getMass()*ASTEROID_ELASTICITY);
	b->setVelocity(newBMomentum/b->getMass()*ASTEROID_ELASTICITY);
	}

void Game::processAsteroidCollisions()
	{
	for (int iasteroid = 0; iasteroid < AMT_ASTEROID_SLOTS; iasteroid++)
		{
		Asteroid* thisAsteroid=m_asteroids[iasteroid];
		if(thisAsteroid==nullptr)
			continue;
		for (int iOtherAsteroid = iasteroid+1; iOtherAsteroid < AMT_ASTEROID_SLOTS; iOtherAsteroid++)
			{
			Asteroid* otherAsteroid=m_asteroids[iOtherAsteroid];
			if(otherAsteroid==nullptr || otherAsteroid==thisAsteroid)
				continue;
			if (thisAsteroid->doesCollideWith(*otherAsteroid))
				{
				processSingleAsteroidCollision(thisAsteroid,otherAsteroid);
				}
			}
		}
	}

void Game::processBulletCollisionsWithAsteroids()
	{
	for (int ibullet = 0; ibullet < AMT_BULLET_SLOTS; ibullet++)
		{
		Bullet* thisBullet=m_bullets[ibullet];
		if(thisBullet==nullptr)
			continue;
		for (int iasteroid = 0; iasteroid < AMT_ASTEROID_SLOTS; iasteroid++)
			{
			Asteroid* thatAsteroid=m_asteroids[iasteroid];
			if(thatAsteroid==nullptr)
				continue;
			if (thisBullet->doesCollideWith(*thatAsteroid))
				{
				processSingleBulletCollisionWithAsteroid(ibullet,iasteroid);
				if(m_bullets[ibullet]==nullptr)
					break;
				}
			}
		}	
	}

void Game::destroyTheShip()
	{
	if (m_theShip)
		{
		delete m_theShip;
		m_theShip=nullptr;
		}
	}

void Game::init()
	{
	m_gameState=GAME_STATE_PLAYING;
	m_endFrameProgression=0.f;
	startNewStage();
	spawnTheShip();
	spawnThePlanet();
	m_starField=new Starfield();

	//m_theShip=new Ship(0.5f*(g_theWorldBounds->m_mins + g_theWorldBounds->m_maxs));
	g_doShowDebugGraphics = false;
	}

void Game::startNewStage()
	{
	if(m_playStage>=AMT_PLAY_STAGES)
		return;
	deleteAllAsteroids();
	const int AMT_INITIAL_ASTEROIDS[AMT_PLAY_STAGES] = { 4,6,8,12 };
	for (int i = 0; i < AMT_INITIAL_ASTEROIDS[m_playStage]; i++)
		spawnNewAsteroid();
	}

void Game::update(float deltaSeconds)
	{
	updateScreenShakeTimer(deltaSeconds);
	if (m_gameState == GAME_STATE_PLAYING)
		{
		if (g_theInput->wasKeyJustPressed(KEYCODE_PAUSE) || g_theInput->wasKeyJustPressed('P') || g_theInput->getXboxController(XCONTROLLER_0).wasXboxButtonJustPressed(XBUTTON_START))
			togglePause();
		if(m_isPaused)
			deltaSeconds=0.f;	
		if (g_theInput->isKeyDown('T'))
			deltaSeconds *= 0.1f;
		if (g_theInput->isKeyDown('R'))
			deltaSeconds *= 10.f;
		if (g_theInput->isKeyDown('W'))
			deltaSeconds *= 100.f;

		for (int i = 0; i < AMT_ASTEROID_SLOTS; i++)
			{
			if (m_asteroids[i] != nullptr)
				m_asteroids[i]->update(deltaSeconds);
			}

		if( g_theInput->wasKeyJustPressed( 'I' ) || g_theInput->getXboxController(XCONTROLLER_0).wasXboxButtonJustPressed(XBUTTON_Y))
			spawnNewAsteroid();
		if (g_theInput->wasKeyJustPressed('O') || g_theInput->getXboxController(XCONTROLLER_0).wasXboxButtonJustPressed(XBUTTON_X))
			deleteFurthestAsteroid();	
		if (g_theInput->wasKeyJustPressed('N') || g_theInput->getXboxController(XCONTROLLER_0).wasXboxButtonJustPressed(XBUTTON_B))
			spawnTheShip();
		if (g_theInput->wasKeyJustPressed('D'))
			toggleDebugGraphics();
		if (g_theInput->wasKeyJustPressed(KEYCODE_SPACE) 
			|| g_theInput->getXboxController(XCONTROLLER_0).wasXboxButtonJustPressed(XBUTTON_A))
			shootBulletFromShip();


		for (int ibullet = 0; ibullet < AMT_BULLET_SLOTS; ibullet++)
			{
			if (m_bullets[ibullet] != nullptr)
				{
				m_bullets[ibullet]->update(deltaSeconds);
				if(m_bullets[ibullet]->isDead())
					destroyBullet(ibullet);
				}
			}

		if(m_theShip)
			m_theShip->update(deltaSeconds);
		processCollisions();
		deleteOutOfBoundsAsteroids(deltaSeconds);
		progressPlayStage();		
		}
	else
		{
		progressEndSequence(deltaSeconds);
		}
	}

void Game::updateScreenShakeTimer(float deltaSeconds)
	{
	if (m_currentShakeTimeLeft > 0.f)
		m_currentShakeTimeLeft -= (m_currentShakeTimeLeft > deltaSeconds ? deltaSeconds : m_currentShakeTimeLeft);
	if (m_currentShakeTimeLeft == 0.f)
		m_currentShakeAmount = 0.f;
	}

bool Game::isShipAtCenterForWarp()
	{
	if(!m_theShip)
		return false;
	return IsMostlyEqual(m_theShip->getpos(),g_theWorldBounds->CalcCenter()) 
		&& IsMostlyEqual(fmod(m_theShip->getOrientationDegrees(),360.f),90.f);
	}

void Game::moveShipTowardCenter(float deltaSeconds)
	{
	//move()
	const float MOVE_RATE=150.f;
	Vector2 moveVector = g_theWorldBounds->CalcCenter()- m_theShip->getpos();
	if (moveVector != Vector2::ZERO)
		{
		if(isNan(moveVector.x)||isNan(moveVector.y))
			return;
		float distLeft=moveVector.CalcLength();
		float moveLen=MOVE_RATE*deltaSeconds;
		if(moveLen>distLeft)
			moveLen=distLeft;
		moveVector.SetLength(moveLen);
		m_theShip->adjustPosition(moveVector);
		}
	//rotate()
	float orientationDegrees = m_theShip->getOrientationDegrees();
	float degreesLeft = 90.f - orientationDegrees;
	if (degreesLeft != 0.f)
		{
		const float ROT_RATE = 180.f;
		float angStep = ROT_RATE*deltaSeconds;
		while (orientationDegrees < -90.f)
			orientationDegrees += 360.f;
		while (orientationDegrees > 270.f)
			orientationDegrees -= 360.f;
		float sign = degreesLeft > 0.f ? 1.f : -1.f;
		angStep = min(abs(degreesLeft), angStep);
		angStep *= sign;
		m_theShip->adjustRotation(angStep);
		}
	}

int Game::findFurthestAsteroid(Vector2 center)
	{
	int ifurthest = -1;
	float furthestDist = 0;
	for (int iasteroid = 0; iasteroid < AMT_ASTEROID_SLOTS; iasteroid++)
		{
		if (m_asteroids[iasteroid])
			{
			float curdist = CalcDistance(m_asteroids[iasteroid]->getpos(), center);
			if (curdist > furthestDist)
				{
				furthestDist = curdist;
				ifurthest = iasteroid;
				}
			}
		}
	return ifurthest;
	}

void Game::deleteFurthestAsteroid()
	{
	Vector2 center((g_theWorldBounds->m_mins+g_theWorldBounds->m_maxs)/2.f);
	if(m_theShip)
		center=m_theShip->getpos();
	int ifurthest=this->findFurthestAsteroid(center);
	if (ifurthest!=-1 && m_asteroids[ifurthest])
		{
		deleteAsteroid(ifurthest);
		return;
		}
	}

void Game::deleteOutOfBoundsAsteroids(float deltaSeconds)
	{
	for (int i = 0; i < AMT_ASTEROID_SLOTS; i++)
		{
		if (m_asteroids[i])
			{
			if (m_asteroids[i]->isPartiallyOutOfBounds())
				{
				m_asteroids[i]->incOutOfBoundsCounter(deltaSeconds);
				}
			else
				{
				m_asteroids[i]->resetOutOfBoundsCounter();
				}
			if (m_asteroids[i]->hasBeenOutOfBoundsTooLong())
				{
				deleteAsteroid(i);
				}
			}
		}
	}

int Game::findEmptyAsteroidSlotOrDestroyFurthest()
	{
	int i;
	do {
		i = findEmptyAsteroidSlot();
		if (i == -1)
			deleteFurthestAsteroid();
		} while (i == -1);	
	return i;
	}

void Game::spawnNewAsteroid()
	{
	int i = findEmptyAsteroidSlotOrDestroyFurthest();
	Vector2 asteroidPos = g_theWorldBounds->GenerateEdgePoint(ASTEROID_RADIUS_LARGE, *g_theWorldBounds);
	m_asteroids[i] = new Asteroid(asteroidPos);
	}

void Game::destroyAsteroid(int iasteroid)
	{
	if (iasteroid >= 0 && iasteroid < AMT_ASTEROID_SLOTS)
		{
		Asteroid*& thisAsteroid=m_asteroids[iasteroid];
		if (thisAsteroid!=nullptr)
			{
			if (thisAsteroid->hasReachedSmallestSize())
				{
				deleteAsteroid(iasteroid);
				}
			else
				{
				thisAsteroid->shrinkAsteroidForSplit();
				int i = findEmptyAsteroidSlotOrDestroyFurthest();
				Asteroid*& newAsteroid=m_asteroids[i];
				newAsteroid=new Asteroid(*thisAsteroid);
				Vector2 totalVelocity=thisAsteroid->getVelocity();
				const float MAX_SPEED = __max(200.f,thisAsteroid->getSpeed());
				Vector2 newThisVelocity=makeVector2(GetRandomFloatInRange(0.f, MAX_SPEED), GetRandomFloatInRange(0.f, MAX_SPEED));
				Vector2 newOtherVelocity=totalVelocity-newThisVelocity;
				newAsteroid->setVelocity(newOtherVelocity);
				thisAsteroid->setVelocity(newThisVelocity);
				newAsteroid->initVertexOffsets();
				thisAsteroid->initVertexOffsets();
				}
			}
		}
	}

void Game::deleteAsteroid(int iasteroid)
	{
	delete m_asteroids[iasteroid];
	m_asteroids[iasteroid] = nullptr;	
	}

void Game::rotateAsteroids(float deltaSeconds)
	{
	for (int i = 0; i < AMT_ASTEROID_SLOTS; i++)
		{
		if(m_asteroids[i]!=nullptr)
			m_asteroids[i]->rotate(deltaSeconds);
		}
	}

void Game::moveAsteroids(float deltaSeconds)
	{
	for (int i = 0; i < AMT_ASTEROID_SLOTS; i++)
		{
		if (m_asteroids[i] != nullptr)
			m_asteroids[i]->move(deltaSeconds);
		}
	}

void Game::render() const
	{

	g_theRenderer->setOrthoProjection(Vector2::ZERO, Vector2(g_worldWidth, g_worldHeight));
	doScreenShake();
	if (m_gameState == GAME_STATE_PLAYING)
		{
		renderGameplay();
		}
	else if (m_gameState == GAME_STATE_MOVING_TO_CENTER_FOR_END)
		{
		renderShipCentering();
		}
	else if (m_gameState >= GAME_STATE_FIRST_STAR_LINE_FRAME&&m_gameState <= GAME_STATE_LAST_STAR_LINE_FRAME)
		{
		renderStarLines(m_gameState-GAME_STATE_FIRST_STAR_LINE_FRAME);
		}
	else if (m_gameState >= GAME_STATE_FIRST_PLANET_FRAME && m_gameState <= GAME_STATE_LAST_PLANET_FRAME)
		{
		renderPlanet(m_gameState-GAME_STATE_FIRST_PLANET_FRAME);
		}
	else if (m_gameState >= GAME_STATE_PLANET_ARRIVAL_STAGE && m_gameState <= GAME_STATE_GAME_OVER_STAGE)
		{
		renderGameOver();
		}
	}

void Game::renderGameplay() const
	{
	if (m_starField)
		m_starField->render();

	for (int i = 0; i < AMT_ASTEROID_SLOTS; i++)
		{
		Asteroid* asteroid = m_asteroids[i];
		if (asteroid)
			{
			asteroid->render();
			}
		}
	for (int i = 0; i < AMT_BULLET_SLOTS; i++)
		{
		Bullet* bullet = m_bullets[i];
		if (bullet)
			{
			bullet->render();
			}
		}
	if (m_theShip)
		m_theShip->render();
	renderHUD();
	}

void Game::renderShipCentering() const
	{
	if (m_starField)
		m_starField->render();
	if (m_theShip)
		m_theShip->render();
	renderHUD();
	}

void Game::toggleDebugGraphics()
	{
	g_doShowDebugGraphics = !g_doShowDebugGraphics;
	}