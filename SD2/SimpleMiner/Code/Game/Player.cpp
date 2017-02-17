#include "Game/Player.hpp"
#include "Game/GameCommon.hpp"
#include "Game/BlockInfo.hpp"
#include "Game/World.hpp"
#include "Game/Game.hpp"
#include <sstream>

Player::Player(const Vector3& startingPosition)
	: m_playerSnowman(startingPosition,PLAYER_SNOWMAN_RADIUS,PLAYER_SNOWMAN_HALF_HEIGHT)
	, m_prevPos(Vector3::ZERO)
	, m_accel(Vector3::ZERO)
	, m_vel(Vector3::ZERO)
	, m_moveAccel(Vector3::ZERO)
	, m_yawDegrees(0.f)
	, m_headPitchDegrees(0.f)
	, m_jumpVel(Vector3::ZERO)
	, m_airJumpCount(0)
	, m_flyAccel(Vector3::ZERO)
	, m_isGrounded(false)
	, m_wasGroundedLastFrame(false)
	, m_wallJumpTimeLeftSeconds(0.f)
	, m_moveMode(MOVE::WALK)
	, m_autoJumpCooldownSeconds(0.f)
	, m_walkDistanceLeft(0.f)
	, m_walkStepAudioHandle((AudioChannelHandle)nullptr)
	, m_jumpSound(0)
	, m_jumpAudioHandle((AudioChannelHandle)nullptr)
	{
	init();
	}

void Player::init()
	{
	if (g_theAudioSys)
		m_jumpSound=g_theAudioSys->createOrGetSound(std::string("Data/Audio/jump.ogg"));
	}

void Player::update(float deltaSeconds)
	{
	walkStep(deltaSeconds);

	m_prevPos=m_playerSnowman.m_center;
	if (m_isGrounded || m_wallJumpTimeLeftSeconds>0.f)
		m_airJumpCount=0;
	if (m_jumpVel==Vector3::ZERO)
		jumpAutomatically(deltaSeconds);
	m_wasGroundedLastFrame=m_isGrounded;
	m_isGrounded=false;
	accelerate(deltaSeconds);
	move(deltaSeconds);

	updateDebugMessages();
	updateWallJumpInfo(deltaSeconds);
	debugDisplayAirAndWallJumpInfo();
	}

void Player::accelerate(float deltaSeconds)
	{
	m_accel=calcAcceleration();
	m_vel+=m_accel*deltaSeconds;
	if(m_vel.calcLengthSquared()>WORLD_TERMINAL_SPEED_METERS_PER_SECOND_SQUARED)
		m_vel.setLength(WORLD_TERMINAL_SPEED_METERS_PER_SECOND);
	if(m_moveMode==MOVE::WALK)
		m_vel+=m_jumpVel;

	addVelocityInfoToDebugPrinter();
	}

Vector3 Player::calcAcceleration()
	{

	Vector3 gravAccel=Vector3::ZERO;
	
	if(m_moveMode==MOVE::WALK)
		gravAccel=Vector3(0.f,0.f,-WORLD_GRAVITATIONAL_ACCELERATION_METERS_PER_SECOND);
// 	else if(m_moveMode==MOVE::FLY)
// 		//gravAccel=Vector3(0.f,0.f,0.f)
// 		;
	Vector3 result=gravAccel;
	Vector3 frictionAccel=m_vel*(-MOVEMENT_FRICTION_DECELERATION_COEFFICIENT);
	result+=frictionAccel;
	result+=m_moveAccel;
	if (m_moveMode==MOVE::FLY)
		result+=m_flyAccel;

	return result;
	}

void Player::move(float deltaSeconds)
	{
	Vector3 pos=m_playerSnowman.m_center;
	pos+=m_vel*deltaSeconds;
	m_playerSnowman.reCenter(pos);
	}

void Player::jumpAutomatically(float deltaSeconds)
	{
	if (m_autoJumpCooldownSeconds>0.f)
		{
		m_autoJumpCooldownSeconds-=deltaSeconds;
		return;
		}
	else 
		m_autoJumpCooldownSeconds=0.f;

	float jumpCheckIntervalSeconds=maxOf(deltaSeconds,0.25f);

	if(!m_isGrounded)
		return;
	const float MOVEMENT_THRESHOLD=0.25f;
	Vector3 moveDisp=m_vel*jumpCheckIntervalSeconds;
	if(moveDisp==Vector3::ZERO)
		return;
	Vector3 moveDir=moveDisp.getNormalized();
	float moveDist=moveDisp.calcLength();
	if(moveDist<MOVEMENT_THRESHOLD)
		return;
	bool shouldTraceRay=false;
	RaycastResult footRayRes=g_theWorld->raycast(m_playerSnowman.m_spheres[0].m_center,moveDir,moveDist,shouldTraceRay);
	RaycastResult headRayRes=g_theWorld->raycast(m_playerSnowman.m_spheres[2].m_center,moveDir,moveDist,shouldTraceRay);
	if (footRayRes.wasHit())
		{
		if (!headRayRes.wasHit() || headRayRes.m_impactFraction>footRayRes.m_impactFraction)
			{
			float backwardsFactor=-0.75f;
			neutralizeVelocityVerticalityBeforeJumping();
			m_jumpVel=Vector3(m_vel.x*backwardsFactor,m_vel.y*backwardsFactor,AUTO_JUMP_SPEED);
			playJumpSound();
			m_autoJumpCooldownSeconds=AUTO_JUMP_COOLDOWN_DURATION_SECONDS;
			}
		}

	}

void Player::updateDebugMessages()
	{
	if (g_theDebugMessagePrinter)
		{
		if (m_moveMode==MOVE::WALK)
			{
			g_theDebugMessagePrinter->addMessage(std::string("Player is Walking."),Rgba::ORANGE);
			std::stringstream ss;
			ss.str("");
			ss.precision(3);
			ss<<"Auto-jump will be ready in "<<m_autoJumpCooldownSeconds<<" seconds.";
			if (g_theDebugMessagePrinter)
				g_theDebugMessagePrinter->addMessage(ss.str(),Rgba::GOLD);
			}
		if (m_moveMode==MOVE::FLY)
			g_theDebugMessagePrinter->addMessage(std::string("Player is Flying."),Rgba::CYAN);
		}
	}

void Player::updateWallJumpInfo(float deltaSeconds)
	{
	m_wallJumpTimeLeftSeconds-=deltaSeconds;
	if (m_wallJumpTimeLeftSeconds<0.f)
		m_wallJumpTimeLeftSeconds=0.f;
	}

void Player::detectAndCorrectCollisions(float deltaSeconds)
	{
	if(deltaSeconds<=0.f)
		return;
	Vector3 moveAmt=m_vel*deltaSeconds;
	Vector3 centerDuringLastFrame=m_playerSnowman.getCenter()-moveAmt;
	Vector3 oldCenter=m_playerSnowman.getCenter();
	BlockInfo bi=g_theWorld->getBlockInfoAtWorldPos(oldCenter);
	bi.detectAndCorrectNeighborhoodCollisions(deltaSeconds,m_playerSnowman,m_vel);
	Vector3 newCenter=m_playerSnowman.getCenter();
	Vector3 correction=newCenter-oldCenter;
/*	Vector3 correctionVel=correction/deltaSeconds;*/
	float correctionVerticality=correction.dotProduct(Vector3::Z_AXIS_VECTOR);
 	float correctionHorizontality=correction.dotProduct(Vector3(m_vel.x,m_vel.y,0.f));

	bool didNotMoveX=centerDuringLastFrame.x==newCenter.x;
	bool didNotMoveY=centerDuringLastFrame.y==newCenter.y;
	if (didNotMoveX)
		m_vel.x*=0.5f;
	if (didNotMoveY)
		m_vel.y*=0.5f;


	m_isGrounded=correctionVerticality>0.f && m_vel.z<0.f ;
	bool isHittingHead=correctionVerticality<0.f && m_vel.z>0.f;
	if (g_theDebugMessagePrinter)
		{
		if(didJustLand())
			g_theDebugMessagePrinter->addMessage(std::string("Player just landed"),Rgba::YELLOW);
		else if (m_isGrounded)
			g_theDebugMessagePrinter->addMessage(std::string("Player is grounded"),Rgba::LIME);
		else
			g_theDebugMessagePrinter->addMessage(std::string("Player is NOT grounded"),Rgba::RED);
		}
	if(m_isGrounded || isHittingHead)
		m_vel.z=0.f;

	bool isAgainstWall=correctionHorizontality<0.f && (m_vel.x>0.f || m_vel.y>0.f);
	if (isAgainstWall)
		m_wallJumpTimeLeftSeconds=PLAYER_WALL_JUMP_WINDOW_SECONDS;
	}

void Player::render() const
	{

	if(g_theGame->getCamera().m_mode!=CAMERA::FIRST_PERSON)
		g_theRenderer->drawWireframeSnowman(m_playerSnowman);
	}

void Player::teleportTo(const Vector3& pos)
	{
	m_playerSnowman.reCenter(pos);
	m_vel=m_accel=m_jumpVel=Vector3::ZERO;
	}

Vector3 Player::getPosition()
	{
	return m_playerSnowman.getCenter();
	}

Vector3 Player::calcEyePosition()
	{
	float halfHeight=m_playerSnowman.m_halfHeight;
	float fullHeight=halfHeight*2.f;
	Vector3 crown=m_playerSnowman.getCenter()+Vector3(0.f,0.f,halfHeight);
	Vector3 verticalEyeDispFromCrown=Vector3(0.f,0.f,-(1.f/7.f)*fullHeight);
// 	Vector3 horizDisp=calcHeadForwardVector();
	Vector3 horizDisp=Vector3::ZERO;
	Vector3 result=crown+verticalEyeDispFromCrown+horizDisp;
	return result;
	}

Vector3 Player::calcFromBehindPosition()
	{
	float halfHeight=m_playerSnowman.m_halfHeight;
	float fullHeight=halfHeight*2.f;
	Vector3 crown=m_playerSnowman.getCenter()+Vector3(0.f,0.f,halfHeight);
	Vector3 verticalEyeDispFromCrown=Vector3(0.f,0.f,-(1.f/7.f)*fullHeight);
	Vector3 horizDisp=Vector3(Vector3::X_AXIS_VECTOR);
	horizDisp.rotateDegrees(m_headPitchDegrees,Y_AXIS_3D);
	horizDisp.rotateDegrees(m_yawDegrees,Z_AXIS_3D);
	horizDisp*=-m_playerSnowman.m_radius*12.f;
	Vector3 result=crown+verticalEyeDispFromCrown+horizDisp;
	return result;
	}

void Player::setPosition(const Vector3& pos)
	{
	m_playerSnowman.reCenter(pos);
	}

Vector3 Player::calcHeadForwardVector()
	{
	Vector3 result=Vector3(Vector3::X_AXIS_VECTOR);
	result.rotateDegrees(m_headPitchDegrees,Y_AXIS_3D);
	result.rotateDegrees(m_yawDegrees,Z_AXIS_3D);
	result*=m_playerSnowman.m_radius*1.1f;
	return result;
	}

void Player::cycleMovementMode()
	{
	m_moveMode++;
	if(m_moveMode>=MOVE::AMT_MODES)
		m_moveMode=MOVE::WALK;
	}

void Player::startJump()
	{
	neutralizeVelocityVerticalityBeforeJumping();
	m_jumpVel=Vector3(0.f,0.f,JUMP_SPEED);
	if (m_wallJumpTimeLeftSeconds>0.f)
		{
		Vector3 latVel=calcHeadForwardVector();
		latVel.z=0.f;
		latVel.normalize();
		latVel*=(-JUMP_SPEED*0.1f);
		m_jumpVel+=latVel;
		}
	if(!m_isGrounded)
		m_airJumpCount++;
	m_wallJumpTimeLeftSeconds=0.f;

	playJumpSound();
	}

bool Player::canJump()
	{
	bool result;
	if(m_isGrounded || m_wallJumpTimeLeftSeconds>0.f)
		result=true;
	else
		result=m_airJumpCount<PLAYER_MAX_AIR_JUMP_COUNT;
	return result;
	}

void Player::debugDisplayAirAndWallJumpInfo()
{
	std::stringstream ss;
	ss.str("");
	ss<<"airJumpCount="<<m_airJumpCount<<"; canJump? "<<(canJump()?"Yes":"No");
	ss<<"; ";
	ss<<"wallJumpTimer="<<m_wallJumpTimeLeftSeconds;
	if(g_theDebugMessagePrinter)
		g_theDebugMessagePrinter->addMessage(ss.str(),Rgba::getGray(90*255/100,255));
	}

void Player::playJumpSound()
	{
	if (m_jumpAudioHandle)
		g_theAudioSys->stopChannel(m_jumpAudioHandle);
	m_jumpAudioHandle=g_theAudioSys->playSound(m_jumpSound,0.5f);
	}

void Player::neutralizeVelocityVerticalityBeforeJumping()
	{
	m_vel.z*=0.5f;
	}

void Player::playWalkSound()
	{
	std::vector<int> groundTypesUnderMe=getGroundTypesUnderMe();
	if(groundTypesUnderMe.size()==0)
		return;
	int indexOfPickedGroundType=GetRandomIntInRange(0,groundTypesUnderMe.size()-1);
	int pickedBlockType=groundTypesUnderMe[indexOfPickedGroundType];
	BlockDefinition bdef=BlockDefinition::s_definitions[pickedBlockType];
// 	if(g_theDebugMessagePrinter)
// 		g_theDebugMessagePrinter->addMessage(std::string("Walk sound is being played for ")+bdef.m_nameString,Rgba::GOLD);
	SoundID groundWalkSound=BlockDefinition::s_definitions[pickedBlockType].m_walkSound;

	if (g_theAudioSys)
		{
		if (m_walkStepAudioHandle)
			g_theAudioSys->stopChannel(m_walkStepAudioHandle);
		m_walkStepAudioHandle=g_theAudioSys->playSound(groundWalkSound,0.5f);
		}
	resetWalkStepCountdown();
	}

std::vector<int> Player::getGroundTypesUnderMe()
	{
	std::vector<int> result;

	if(!m_isGrounded)
		return result;

	std::vector<BlockInfo> groundNeighbors=getMyGroundNeighbors();

	bool hasTypeAtIndexBeenAdded[BLOCK::AMT_TYPES];
	for(int i=0;i<BLOCK::AMT_TYPES;i++)
		hasTypeAtIndexBeenAdded[i]=false;

	// 678
	// 345
	// 012

	for (int i=0;i<(int)groundNeighbors.size();i++)
		{
		AABB3 neighborBounds=groundNeighbors.at(i).calcBounds();
		AABB2 neighborBounds2(Vector2(neighborBounds.m_mins.x,neighborBounds.m_mins.y),
							  Vector2(neighborBounds.m_maxs.x,neighborBounds.m_maxs.y));
		Disc2 playerDisc(Vector2(m_playerSnowman.m_center.x,m_playerSnowman.m_center.y),m_playerSnowman.m_radius);
		if (playerDisc.doesOverlapWith(neighborBounds2))
			{
			unsigned char btype=groundNeighbors.at(i).getBlock()->m_type;
			if(!hasTypeAtIndexBeenAdded[i])
				result.push_back(btype);
			hasTypeAtIndexBeenAdded[btype]=true;
			}
		}

	while(result.size()>=2)
		break;

	return result;
	}

void Player::resetWalkStepCountdown()
	{
	m_walkDistanceLeft=0.f;
	Vector2 hvel(m_vel.x,m_vel.y);
	float speed=hvel.octagonEstimateLength();
	if(speed<=0.01f)
		return;
	//=0.75+(A6-0.75)/0.75
	m_walkDistanceLeft=speed*0.5f;
	m_walkDistanceLeft=GetRandomFloatInRange(m_walkDistanceLeft*0.8f,m_walkDistanceLeft*1.2f);
	m_walkDistanceLeft=clampFloat(m_walkDistanceLeft,
										  PLAYER_MIN_WALK_STRIDE_LENGTH,
										  PLAYER_MAX_WALK_STRIDE_LENGTH);
	}

bool Player::didJustLand()
	{
	bool result=!m_wasGroundedLastFrame && m_isGrounded;
	return result;
	}

void Player::playBlockRemovalSound(unsigned char blockType)
	{
	if(g_theAudioSys==nullptr)
		return;
	SoundID removalSound=BlockDefinition::s_definitions[blockType].m_removalSound;
	if(m_removalAudioHandle)
		g_theAudioSys->stopChannel(m_removalAudioHandle);
	m_removalAudioHandle=g_theAudioSys->playSound(removalSound);
	}

void Player::playBlockPlacementSound(unsigned char blockType)
	{
	if (g_theAudioSys==nullptr)
		return;
	SoundID placementSound=BlockDefinition::s_definitions[blockType].m_placementSound;
	if (m_placementAudioHandle)
		g_theAudioSys->stopChannel(m_placementAudioHandle);
	m_placementAudioHandle=g_theAudioSys->playSound(placementSound);
	}

bool Player::isWalking()
	{
	Vector2 vel2=Vector2(m_vel.x,m_vel.y);
	bool result = m_moveMode==MOVE::WALK && vel2.calcLengthSquared()>0.01f;
	if (g_theDebugMessagePrinter)
		{
		if (result)
			g_theDebugMessagePrinter->addMessage(std::string("Walking foward."),Rgba::LIME);
		else
			g_theDebugMessagePrinter->addMessage(std::string("NOT walking foward."),Rgba::RED);
		}
	return result;
	}

bool Player::shouldPlayWalkSound()
	{
	return (didJustLand() || isWalking()) &&  m_walkDistanceLeft<=0.f;
	}

void Player::walkStep(float deltaSeconds)
{
	Vector2 hvel(m_vel.x,m_vel.y);
	float speed=hvel.octagonEstimateLength();
	m_walkDistanceLeft-=speed*deltaSeconds;
	if(!m_isGrounded)
		m_walkDistanceLeft=0.f;
	else if (shouldPlayWalkSound())
		playWalkSound();
	}

void Player::addVelocityInfoToDebugPrinter()
	{
	if(g_theDebugMessagePrinter==nullptr)
		return;
	std::stringstream ss("");
	ss.str("");
	ss.precision(4);
	ss<<"Player Velocity: "<<m_vel.asString();
	ss<<"; Player Speed: "<<m_vel.calcLength();
	ss<<"\n";
	ss.precision(4);
	ss<<"Walk step distance left : "<<m_walkDistanceLeft<<" meters";
/*	ss<<std::endl;*/
	g_theDebugMessagePrinter->addMessage(ss.str(),Rgba::CYAN);
	}

std::vector<BlockInfo> Player::getMyGroundNeighbors()
	{
	std::vector<BlockInfo> result;

	if (!m_isGrounded)
		return result;

	result.resize(9);

	// 678
	// 345
	// 012

	BlockInfo bi=g_theWorld->getBlockInfoAtWorldPos(this->m_playerSnowman.m_spheres[0].m_center);
	bi.moveDown();
	result[4]=bi;
	result[1]=bi.getSouth();
	result[7]=bi.getNorth();
	for (int i=1;i<=7;i+=3)
		{
		result[i-1]=result[i].getWest();
		result[i+1]=result[i].getEast();
		}

	return result;
	}
